#ifndef _WORDLIB_H_
#define _WORDLIB_H_

#define MAX_WORDLIB 16

typedef struct _WORD_LIB {
	char desc[64];
	void (*wordlist_load)(char *filename);
} WORD_LIB, *WORD_LIB_P;

extern WORD_LIB word_libs[MAX_WORDLIB];

extern void set_active_lib(int a); 
extern int get_active_lib();
extern int wordlist_load(char *filename);
extern int wordlib_init();
extern char *lib_active_desp(); 
extern int get_lib_count();
extern void wordlib_end();
#endif
