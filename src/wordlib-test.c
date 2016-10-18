#include <string.h>

#include "wordlib.h"
#include "wordlist.h"
#include "wordlib-test.h"

static char *desp = "²âÊÔ´Ê¿â";

void test_wordlist_load()
{
	strcpy(words_list.words[0].spell, "a");
	strcpy(words_list.words[0].ch, "haha");
	strcpy(words_list.words[0].symbol, "eee");
	strcpy(words_list.words[0].content, "this is a test word 1");

	strcpy(words_list.words[1].spell, "b");
	strcpy(words_list.words[1].ch, "haha");
	strcpy(words_list.words[1].symbol, "bbb");
	strcpy(words_list.words[1].content, "this is a test word 2");

	words_list.count = 2;
}

void test_registe_lib()
{
	WORD_LIB *lib =  &word_libs[1];

	strncpy(lib->desc, desp, sizeof(lib->desc) - 1);
        lib->wordlist_load = test_wordlist_load; 
}



