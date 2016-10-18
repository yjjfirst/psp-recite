#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <pspkernel.h>

#include "wordlib.h"
#include "wordlist.h"
#include "./common/datatype.h"
#include "./common/log.h"

static char *desp = "Ì«Éµµ¥´ÊGRE";
static byte word_boundary[BOUNDARY_LENGTH];

byte *gre_ustrstr (byte* buf, dword buf_len, byte *sub, word sub_len)
{
        byte *bp;
        byte *sp;
        int i, j;

        for (i = 0; i < buf_len ; i++) {
                bp = buf;
                sp = sub;
                j = 0;

                while(*bp++ == *sp++) {
                        j++;
                } ;

                if (j == sub_len) {
                        return buf;
                }
                buf += 1;
        }

        return NULL;
}


int gre_parse_word(byte *word)
{
        char *pos;
	int count = words_list.count;
	
	WORD_STRUCTURE_P wordp = &words_list.words[count];

	pos = (char *)(word + 3);
	strcpy(wordp->spell, pos);

	pos = (char *)(strlen(pos) + pos + 1);
	strcpy(wordp->symbol, pos);

	pos = (char *)(strlen(pos) + pos + 1);
	strcpy(wordp->ch, pos);

	pos = (char *)(strlen(pos) + pos +1);
	strcpy(wordp->content, pos);

	words_list.count ++;

	return 1;

}

void gre_wordlist_load(char *filename) 
{
	int fd = -1;
	dword size;
	byte *words;
	byte *pos;
	byte *start_ptr;
	int count;
	char file[256];
	char appdir[256];

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, "words/gre");

	if ( (fd = sceIoOpen(file, PSP_O_RDONLY, 0777)) < 0) {
		log_msg("gre", "Can not open file %s.\n", file);
		return ;
	}
	size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	if ( (words = (byte*)calloc(1, size)) == NULL) {
		log_msg("gre", "Not enough memory.");
		goto fail;		
	}

	sceIoLseek32(fd, 0, PSP_SEEK_SET);	
	sceIoRead(fd, words, size);

	memcpy(word_boundary, words, BOUNDARY_LENGTH);

	count = 0;
	start_ptr = words;
	while ( (pos = gre_ustrstr(start_ptr, size, word_boundary, BOUNDARY_LENGTH)) != NULL) {
		pos += BOUNDARY_LENGTH;
		GRE_WORD_RANGE range = in_range(count);
		if ( range == GRE_WORD_RANGE_IN) {
			gre_parse_word(pos);
		} else if (range == GRE_WORD_RANGE_UPPER) {
			break;
		}
		count ++;
		size -= (pos - start_ptr);
		start_ptr = pos;
	}

	free(words);
fail:	
	sceIoClose(fd);	
}

void gre_registe_lib() 
{
	WORD_LIB *lib =  &word_libs[0];

	strncpy(lib->desc, desp, sizeof(lib->desc) - 1);
        lib->wordlist_load = gre_wordlist_load; 
}


