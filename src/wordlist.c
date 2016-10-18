#include <stdio.h>
#include <string.h>
#include <pspkernel.h>
#include <stdlib.h>

#include "wordlist.h"
#include "wordlib.h"
#include "./common/log.h"
#include "./common/datatype.h"
#include "mp3.h"
#include "config.h"

static const char* wl_module = "wordlist";

WORD_LIST words_list;

void wordlist_init()
{
	words_list.active = config.activeword;
	words_list.list = config.wordlist;
	wordlist_load(word_libs[config.wordlib].desc);
}

void wordlist_reload()
{
	wordlist_load(word_libs[config.wordlib].desc);
}

void wordlist_reset()
{
	words_list.list = 0;
	words_list.count = 0;
	words_list.active = 0;
}

void dump_words_list() 
{
	int i;
	WORD_STRUCTURE_P word = NULL;

	for (i = 0; i < words_list.count; i ++) {
		word = &words_list.words[i];

		log_msg(wl_module, "spell: %s", word->spell);
		log_msg(wl_module, "symbol: %s", word->symbol);
		log_msg(wl_module, "ch: %s", word->ch);
		log_msg(wl_module, "content: %s", word->content);
	}
}

int in_range(int order)
{
        if (order < words_list.list * WORDLIST_LENGTH)
                return GRE_WORD_RANGE_LOWER;

        if (order >= (words_list.list + 1) * WORDLIST_LENGTH)
                return GRE_WORD_RANGE_UPPER;

        return GRE_WORD_RANGE_IN;
}

int wordlist_next_word(void)
{
	words_list.active ++ ;
	words_list.active %= words_list.count;

	return 0;
}

int wordlist_previous_word(void)
{
	words_list.active -- ;
	if (words_list.active < 0)
		words_list.active = words_list.count - 1;

	return 0;
}

int wordlist_next_page(void)
{
	words_list.active += LIST_WORDS_COUNT;
	words_list.active %= words_list.count;

	words_list.active = words_list.active - 
		words_list.active % LIST_WORDS_COUNT;
	return 0;
}

int wordlist_previous_page(void)
{

	words_list.active -= LIST_WORDS_COUNT;
	if (words_list.active < 0) {
		words_list.active = words_list.count - 1;
	}

	words_list.active = words_list.active - 
		words_list.active % LIST_WORDS_COUNT;

	return 0;
}
void wordlist_end()
{
	config.wordlist = words_list.list;
	config.activeword = words_list.active;
}

void play_word()
{
	char file[64];
	char zipfile[64];
	char *spell = words_list.words[words_list.active].spell;

	sprintf(file, "%c/%s.mp3", spell[0], spell);
	sprintf(zipfile, "%c.zip", spell[0]);
	mp3_play(file, zipfile);
}

