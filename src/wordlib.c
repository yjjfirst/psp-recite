#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pspkernel.h>

#include "wordlib.h"
#include "wordlib-gre.h"
#include "wordlib-cet4.h"
#include "wordlib-cet6.h"
#include "config.h"
#include "./common/datatype.h"
#include "./common/log.h"
#include "fat.h"

WORD_LIB word_libs[MAX_WORDLIB];
static int active_lib;

void set_active_lib(int a) 
{
	active_lib = a;
}

int get_active_lib()
{
	return active_lib;
}

int wordlist_load(char *filename)
{
 
	word_libs[active_lib].wordlist_load(filename);

	return 0;
}

char *lib_active_desp() 
{

	return word_libs[active_lib].desc;
}

int get_lib_count()
{
	int i = 0;
	while (word_libs[i].wordlist_load != NULL) {
		i ++;
	}

	return i;
}

void customer_wordlist_load(char *filename)
{
	int fd = -1;
	int size;
	char *words;
	char file[256];
	char appdir[256];

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, "words/customer/");
	strcat(file, filename);

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

void register_customer_lib(char *name, int pos)
{
	WORD_LIB *lib =  &word_libs[pos];

	strncpy(lib->desc, name, sizeof(lib->desc) - 1);
	lib->wordlist_load = customer_wordlist_load;

}

int wordlist_init_customer_lib()
{
    char file[256];
    char appdir[256];
    char spath[256];
    p_fat_info info;
    int count;
    int i;
    int pos;

    getcwd(appdir, 256);
    strcat(appdir,"/");
    strcpy(file, appdir);
    strcat(file, "words/customer");
    
    if ( (count = fat_readdir(file, spath, &info)) == -1) {
	    return -1;
    }

    pos = 0;
    for (i = 0; i < count; i++) {
	    if (strcmp(info[i].filename, "..") == 0) {
		    continue;
	    }

	    register_customer_lib(info[i].filename, pos + 3);
	    pos ++;
    }

    free((void *)info);
    return 0;
}

int wordlib_init() 
{
	gre_registe_lib();
	cet4_registe_lib();
	cet6_registe_lib();
//	test_registe_lib();
	wordlist_init_customer_lib();

	set_active_lib(config.wordlib);

	return 0;
}

void wordlib_end()
{
	config.wordlib = active_lib;
}
