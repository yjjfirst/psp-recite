#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <pspkernel.h>

#include "wordlib.h"
#include "wordlist.h"
#include "wordlib-cet4.h"
#include "./common/log.h"

static char *desp = "大学英语四级词汇";

extern void cet6_parse_words(char *words);

void cet4_wordlist_load(char *filename)
{

	int fd = -1;
	int size;
	char *words;
	char file[256];
	char appdir[256];

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, "words/cet4");

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


void cet4_registe_lib()
{
        WORD_LIB *lib =  &word_libs[1];

        strncpy(lib->desc, desp, sizeof(lib->desc) - 1);
        lib->wordlist_load = cet4_wordlist_load;
}

