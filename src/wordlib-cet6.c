#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pspkernel.h>

#include "wordlib.h"
#include "wordlist.h"
#include "wordlib-cet6.h"
#include "./common/log.h"

static char *desp = "¥Û—ß”¢”Ô¡˘º∂¥ ª„";

void cet6_parse_words(char *words)
{
	char *ptr = words;
	char *pos_start;
	char *pos_end;
	char buf[1024];
	long i;
	long len;
	long count = 0;	
	
	while ( (pos_start = strstr(ptr, "<<<")) != NULL) {		

		WORD_STRUCTURE word;

		memset(&word, 0, sizeof(word));
		pos_start += 3;
		ptr = pos_start;
		memset(buf, 0, sizeof(buf));
		if ( (pos_end = strstr(ptr, "<<<")) == NULL) {
			pos_end = ptr + strlen(ptr);	
		}
		
		i = 0;
		while (*(pos_start) != '/') {
			buf[i] = *pos_start;
			i ++;
			pos_start ++;	
		}
		pos_start ++;
                strcpy(word.spell, buf);

		i = 0;
		memset(buf, 0, sizeof(buf));
		while (*(pos_start) != '/') {
			buf[i] = *pos_start;
			i ++;
			pos_start ++;
		}
		pos_start ++;
		
		strcpy(word.symbol, buf);

		strcpy(word.ch,"");
		while ( *(pos_start) == 0x0d || *(pos_start) == 0x0a) pos_start ++;	

		memset(buf, 0, sizeof(buf));
		if (sizeof(word.content) - 1 > (pos_end - pos_start)) {
			len = pos_end - pos_start;
		} else {
			len = sizeof(word.content) - 1;
		}

		strncpy(word.content, pos_start, len); 
		word.content[len-1] = '\0';
		GRE_WORD_RANGE range = in_range(count);
		count ++;

		if (range == GRE_WORD_RANGE_LOWER) {
			continue;
		} else if (range == GRE_WORD_RANGE_UPPER){
			break;
		}

		WORD_STRUCTURE_P wordp = &words_list.words[words_list.count];
		memcpy(wordp, &word, sizeof(word));

	        words_list.count ++;

		ptr = pos_end;
	}					
}


void cet6_wordlist_load(char *filename)
{

	int fd = -1;
	int size;
	char *words;
	char file[256];
	char appdir[256];

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, "words/cet6");

        if ( (fd = sceIoOpen(file, PSP_O_RDONLY, 0777)) < 0) {
                log_msg("gre", "Can not open file %s.\n", file);
                return ;
        }

        size = sceIoLseek32(fd, 0, PSP_SEEK_END);
	if ( (words =(char *) calloc(1, size + 1)) == NULL ) {
		printf("not enough memory.\n");
		goto fail;
	}

	words[size] = '\0';

        sceIoLseek32(fd, 0, PSP_SEEK_SET);
        sceIoRead(fd, words, size);
	cet6_parse_words(words);
	
	free(words);
fail:
       sceIoClose(fd);
}

void cet6_registe_lib()
{
        WORD_LIB *lib =  &word_libs[2];

        strncpy(lib->desc, desp, sizeof(lib->desc) - 1);
        lib->wordlist_load = cet6_wordlist_load;
}


