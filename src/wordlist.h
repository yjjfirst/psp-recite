#ifndef _WORDLIST_H_
#define _WORDLIST_H_

#define BOUNDARY_LENGTH 26
#define WORDLIST_LENGTH 120

typedef struct _WORD_STRUCTURE {
        char  spell[64];
        char  symbol[64];
        char  ch[128];
        char  content[1200];
}WORD_STRUCTURE, *WORD_STRUCTURE_P;

typedef struct _WORD_LIST {
	int list;
	int count;
	int active;
	char name[48];
	WORD_STRUCTURE words[WORDLIST_LENGTH];
	void (*draw_content)(void);
} WORD_LIST, *WORD_LIST_P;

/**
 * @return 1 - in range; 0 - upper; -1 - lower
 */
#define LIST_WORDS_COUNT 10

typedef enum _GRE_WORD_RANGE{
        GRE_WORD_RANGE_LOWER,
        GRE_WORD_RANGE_IN,
        GRE_WORD_RANGE_UPPER
} GRE_WORD_RANGE;

extern WORD_LIST words_list;

extern void wordlist_init();

extern void wordlist_reload();
extern int in_range(int order);
extern void wordlist_reset();

extern int wordlist_next_word(void);
extern int wordlist_previous_word(void);
extern int wordlist_next_page(void);
extern int wordlist_previous_page(void);
extern void play_word();
extern void wordlist_end();
//extern void wordlist_load(const char *file);

#endif
