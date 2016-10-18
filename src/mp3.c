#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <pspkernel.h>
#include <pspaudiolib.h>
#include <pspaudio.h>
#include "include/id3tag.h"
#include "include/mad.h"
#include "common/utils.h"
#include "mp3.h"
#include "include/unzip.h"
#include "common/log.h"

#define MP3_MAX_ENTRY 256
#define MAXVOLUME 0x8000

#define INPUT_BUFFER_SIZE	(8*1024)
#define OUTPUT_BUFFER_SIZE	4*1024	/* Must be an integer multiple of 4. */
#define MAX_RESAMPLEFACTOR 6
#define MAX_NSAMPLES (1152 * MAX_RESAMPLEFACTOR)

struct RESAMPLE_STATE {
	mad_fixed_t ratio;
	mad_fixed_t step;
	mad_fixed_t last;
};

static mad_fixed_t  (*resampled)[2][MAX_NSAMPLES];
static struct RESAMPLE_STATE resample[2];

static struct OUTPUT_PCM{
    unsigned int nsamples;
	mad_fixed_t const * samples[2];
} out_pcm;

static int mp3_handle = 0;
static bool isPlaying = false, eos = true, isPause = true;

static int mp3_bitrate = 0, mp3_sample = 0, mp3_length = 0, mp3_filesize = 0;
static struct mad_stream stream;
static struct mad_frame frame;
static struct mad_synth synth;
static mad_timer_t timer;

static signed short output_buffer[4][OUTPUT_BUFFER_SIZE];
static byte input_buffer[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];
static byte *output_ptr = (byte *)&output_buffer[0][0];
static byte *GuardPtr = NULL;
static byte *output_buffer_end = (byte *)&output_buffer[0][0] + OUTPUT_BUFFER_SIZE * 2;
static int out_buf_index = 0;

static unzFile ziped_voice_file;

static int resample_init(struct RESAMPLE_STATE *state, unsigned int oldrate, unsigned int newrate)
{
	mad_fixed_t ratio;
	
	if (newrate == 0)
		return -1;
	
	ratio = mad_f_div(oldrate, newrate);
	if (ratio <= 0 || ratio > MAX_RESAMPLEFACTOR * MAD_F_ONE)
		return -1;
	
	state->ratio = ratio;
	
	state->step = 0;
	state->last = 0;
	
	return 0;
}

static unsigned int resample_block(struct RESAMPLE_STATE *state, unsigned int nsamples, mad_fixed_t const *old, mad_fixed_t *new)
{
	mad_fixed_t const *end, *begin;
	
	if (state->ratio == MAD_F_ONE) {
		memcpy(new, old, nsamples * sizeof(mad_fixed_t));
		return nsamples;
	}
	
	end   = old + nsamples;
	begin = new;
	
	if (state->step < 0) {
		state->step = mad_f_fracpart(-state->step);
		
		while (state->step < MAD_F_ONE) {
			*new++ = state->step ?
				state->last + mad_f_mul(*old - state->last, state->step) : state->last;
			
			state->step += state->ratio;
			if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
				state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
		}
		
		state->step -= MAD_F_ONE;
	}
	
	while (end - old > 1 + mad_f_intpart(state->step)) {
		old        += mad_f_intpart(state->step);
		state->step = mad_f_fracpart(state->step);
		
		*new++ = state->step ?
			*old + mad_f_mul(old[1] - old[0], state->step) : *old;
		
		state->step += state->ratio;
		if (((state->step + 0x00000080L) & 0x0fffff00L) == 0)
			state->step = (state->step + 0x00000080L) & ~0x0fffffffL;
	}
	
	if (end - old == 1 + mad_f_intpart(state->step)) {
		state->last = end[-1];
		state->step = -state->step;
	}
	else
		state->step -= mad_f_fromint(end - old);
	
	return new - begin;
}

static signed short mad_fixed2_sshort(mad_fixed_t fixed)
{
	if (fixed >= MAD_F_ONE)
		return (SHRT_MAX);
	if (fixed <= -MAD_F_ONE)
		return (-SHRT_MAX);

	fixed = fixed >> (MAD_F_FRACBITS - 15);
	return ((signed short) fixed);
}

bool mp3_load(const char *file)
{
	unz_file_info file_info;
	if (unzLocateFile(ziped_voice_file, file, 2) != UNZ_OK) {
		//log_msg("mp3", "can not locate file %s\n", file);
		return false;
	}
	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);
	mad_stream_init(&stream);
	mad_frame_init(&frame);
	mad_synth_init(&synth);
	mad_timer_reset(&timer);

	if (unzOpenCurrentFile(ziped_voice_file) != UNZ_OK) {
		//log_msg("mp3", "can not open current ziped file.");
		return false;
	}

	unzGetCurrentFileInfo(ziped_voice_file, &file_info, NULL, 0, NULL, 0, NULL, 0);
	mp3_filesize = file_info.uncompressed_size;
	memset(&output_buffer[0][0], 0, 4 * 2 * OUTPUT_BUFFER_SIZE);
	output_ptr = (byte *)&output_buffer[0][0];
	output_buffer_end = output_ptr + OUTPUT_BUFFER_SIZE * 2;
	mp3_bitrate = 0;
	mp3_sample = 0;
	mp3_length = 0;
	eos = false;

	return true;
}

static bool form_pcm()
{
	if(synth.pcm.samplerate == 44100) {
		out_pcm.nsamples = synth.pcm.length;
		out_pcm.samples[0] = synth.pcm.samples[0];
		out_pcm.samples[1] = synth.pcm.samples[1];
	} else {
		if (resample_init(&resample[0], synth.pcm.samplerate, 44100) == -1 ||
		    resample_init(&resample[1], synth.pcm.samplerate, 44100) == -1)
			return false;
		out_pcm.nsamples = resample_block(&resample[0], synth.pcm.length, 
						  synth.pcm.samples[0], (*resampled)[0]);
		resample_block(&resample[1], synth.pcm.length, synth.pcm.samples[1], (*resampled)[1]);

		out_pcm.samples[0] = (*resampled)[0];
		out_pcm.samples[1] = (*resampled)[1];
	}

	return true;
}

static void output_pcm()
{
	int i;
	signed short    sample;

	if (out_pcm.nsamples <= 0) {
		eos = 1;
		return;
	}

	if (MAD_NCHANNELS(&frame.header) == 2) {
		for (i = 0; i < out_pcm.nsamples; i++) {
			/* Left channel */
			sample = mad_fixed2_sshort(out_pcm.samples[0][i]);
			*(output_ptr++) = sample & 0xff;
			*(output_ptr++) = (sample >> 8);                        
			sample = mad_fixed2_sshort(out_pcm.samples[1][i]);
			*(output_ptr++) = sample & 0xff;
			*(output_ptr++) = (sample >> 8);

			if (output_ptr != output_buffer_end) continue;
			sceAudioOutputPannedBlocking(mp3_handle, MAXVOLUME, MAXVOLUME, 
						     (char *)output_buffer[out_buf_index]);
			out_buf_index = (out_buf_index + 1) & 0x03;
			output_ptr = (byte *)&output_buffer[out_buf_index][0];
			output_buffer_end = output_ptr + OUTPUT_BUFFER_SIZE;
			
		}
	} else {	
		for (i = 0; i < out_pcm.nsamples; i++) {
			signed short sample;
			/* Left channel */
			sample = mad_fixed2_sshort(out_pcm.samples[0][i]);
			*(output_ptr++) = sample & 0xff;
			*(output_ptr++) = (sample >> 8);                        
			*(output_ptr++) = sample & 0xff;
			*(output_ptr++) = (sample >> 8);

			if (output_ptr < output_buffer_end) continue;
			sceAudioOutputPannedBlocking(mp3_handle, MAXVOLUME, MAXVOLUME, 
						     (char *)output_buffer[out_buf_index]);
			out_buf_index = (out_buf_index + 1) & 0x03;
			output_ptr = (byte *)&output_buffer[out_buf_index][0];
			output_buffer_end = output_ptr + OUTPUT_BUFFER_SIZE;
		}
	}
}

void mp3_play(const char *file, const char *zip)
{
	char appdir[256];
	char zipfile[256];

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(zipfile, appdir); 
	strcat(zipfile, "voice/");
	strcat(zipfile, zip);

	if ( (ziped_voice_file = unzOpen(zipfile)) == NULL) {		
		//log_msg("mp3", "can not open voice file.");
		return;
	}

	if (!mp3_load(file)) return;

	memset(resampled, 0, sizeof(*resampled));
	memset(output_buffer, 0, sizeof(output_buffer));
	memset(input_buffer, 0, sizeof(input_buffer));

	while (!eos) {
		if (stream.buffer == NULL || stream.error == MAD_ERROR_BUFLEN) {
			size_t ReadSize, Remaining = 0, BufSize;
			byte * ReadStart;
			if(stream.next_frame != NULL) {
				Remaining = stream.bufend - stream.next_frame;
				memmove(input_buffer, stream.next_frame, Remaining);
				ReadStart = input_buffer + Remaining;
				ReadSize = INPUT_BUFFER_SIZE - Remaining;
			} else {
				ReadSize = INPUT_BUFFER_SIZE;
				ReadStart = input_buffer;
				Remaining = 0;
			}

			BufSize = unzReadCurrentFile(ziped_voice_file, ReadStart, ReadSize);
			//BufSize = sceIoRead(fd, ReadStart, ReadSize);
			if(BufSize == 0) {
				eos = true;
				continue;
			}
			if(BufSize < ReadSize) {
				GuardPtr = ReadStart + ReadSize;
				memset(GuardPtr, 0, MAD_BUFFER_GUARD);
				ReadSize += MAD_BUFFER_GUARD;
			}
			mad_stream_buffer(&stream, input_buffer, ReadSize + Remaining);
			stream.error = 0;
		}

		if (mad_frame_decode(&frame, &stream) == -1) {
			if(MAD_RECOVERABLE(stream.error) || stream.error == MAD_ERROR_BUFLEN){
				continue;
			} else {							
				eos = true;
				continue;
			}
		}

		if(mp3_bitrate == 0) {
			mp3_bitrate = frame.header.bitrate;
			mp3_sample = frame.header.samplerate;
			if(mp3_bitrate != 0) {
				mp3_length = mp3_filesize * 8 / mp3_bitrate;
			}	
		}

		mad_timer_add(&timer, frame.header.duration);

		mad_synth_frame(&synth, &frame);

		if (!form_pcm()) continue;
		output_pcm();

	}
	unzCloseCurrentFile(ziped_voice_file);
	unzClose(ziped_voice_file);
	return;
}

bool mp3_init()
{
	isPlaying = true;
	isPause = true;
	eos = true;
	resampled = (mad_fixed_t (*)[2][MAX_NSAMPLES])malloc(sizeof(*resampled));
	mp3_handle = sceAudioChReserve( PSP_AUDIO_NEXT_CHANNEL, OUTPUT_BUFFER_SIZE / 4, 0 );
	return true;
}

void mp3_freetune()
{
	mad_synth_finish(&synth);
	mad_frame_finish(&frame);
	mad_stream_finish(&stream);

	free((void *)resampled);

}

void mp3_end()
{
//	if(fd >= 0)
//		sceIoClose(fd);
	mp3_freetune();
	sceAudioChRelease(mp3_handle);
}



