#ifndef _MAINWIN_H_
#define _MAINWIN_H_
#include "display.h"
#include "window.h"

#define WORD_LIST_WIDTH 120

extern void show_mainwin();
extern void show_wordlist();
extern void show_word_content();
extern void scroll_up_content();
extern void scroll_down_content();
extern void init_main_window();

extern PSP_WINDOW main_window;
#endif //_MAINWIN_H_

