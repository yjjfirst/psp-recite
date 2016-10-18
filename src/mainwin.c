#include <string.h>
#include <stdio.h>
#include <pspctrl.h>

#include "window.h"
#include "mainwin.h"
#include "wordlist.h"
#include "wordlib.h"
#include "version.h"
#include "stack.h"
#include "mp3.h"

static char *cur_content_pos;

PSP_WINDOW main_window;

void show_title()
{

	int x = 2;
	int y = (TITLE_HEIGHT - DISP_FONTSIZE) / 2;
	char str[128];
	int len;
	PSP_WINDOW_P w = &main_window;

	disp_fillrect(0, 0, w->pos.right, TITLE_HEIGHT, 
		      w->colors.title);
	disp_rectangle(0, 0, w->pos.right, TITLE_HEIGHT, 
		       w->colors.line);

	sprintf(str, "%s V%s.%s", w->title, VER_MAJOR, VER_MINOR);
	disp_putstring(x, y, w->colors.text, (const byte *)str);

	sprintf(str, "%s list%02d", lib_active_desp(), words_list.list + 1);
	len = strlen(str);
	disp_putstring(w->pos.right - len * DISP_FONTSIZE/2 - 2, y,
		       w->colors.text, (const byte *)str);

}

void show_wordlist()
{
	int i;
	int active = words_list.active;
	WORD_STRUCTURE_P word = NULL;
	int x, y;
	int begin;
	char str[64];
	PSP_WINDOW_P w = &main_window;

	disp_fillrect(0, TITLE_HEIGHT, WORD_LIST_WIDTH, w->pos.bottom, 
		      w->colors.bg);

        begin = active / LIST_WORDS_COUNT;
	for (i = 0; i <  LIST_WORDS_COUNT; i++) {
		if (i + begin * LIST_WORDS_COUNT >= words_list.count) {
			break;
		}
		word = &words_list.words[i + begin * LIST_WORDS_COUNT];
		x = 2;
		y = TITLE_HEIGHT +10 + i * 20;
		disp_putstring(x, y, w->colors.text, (const byte *)word->spell);
		if (i + begin*LIST_WORDS_COUNT== words_list.active) {
			disp_putstring(x, y, w->colors.hi_text, 
				       (const byte *)word->spell);			
		}
	}

	disp_rectangle(0, TITLE_HEIGHT, WORD_LIST_WIDTH, w->pos.bottom, 
		       w->colors.line);

	sprintf(str, "%02d/120", words_list.active + 1);
	x = (WORD_LIST_WIDTH - strlen(str) * DISP_FONTSIZE / 2)/2;
	y = (TITLE_HEIGHT - DISP_FONTSIZE) / 2 + w->pos.bottom - 20;

	disp_fillrect(0, w->pos.bottom - 20, WORD_LIST_WIDTH, w->pos.bottom, 
		      w->colors.title);
	disp_rectangle(0, w->pos.bottom - 20, WORD_LIST_WIDTH, w->pos.bottom, 
		       w->colors.line);
	disp_putstring(x, y, w->colors.text, (const byte *)str);
}

void show_ch(int x, int y)
{
	int active = words_list.active;

	disp_putstring(x, y, main_window.colors.hi_text, 
		       (const byte *)words_list.words[active].ch);

}

void show_symbol(int x, int y)
{
	int ch_len;
	int ch_remain;
	int symbol_off;
	int active = words_list.active;
	char symbol[64];
	ch_len = strlen(words_list.words[active].ch) * DISP_FONTSIZE / 2;
	ch_remain = ch_len - (main_window.pos.right - WORD_LIST_WIDTH);	

	if (ch_remain <= 0) {
		symbol_off = 0;
	} else {
		symbol_off = ch_remain % (main_window.pos.right - WORD_LIST_WIDTH) + 4;
	}

	if (words_list.words[active].symbol[0] != '[') {
		sprintf(symbol, "[%s]", &words_list.words[active].symbol[1]);
	} else {
		strcpy(symbol, words_list.words[active].symbol);
	}
	symbol_draw_string(x + symbol_off + 8, y, main_window.colors.hi_text,
			   &symbol[1]);
	disp_putstring(x + symbol_off, y+2, main_window.colors.hi_text, 
		       (const byte *)"[");		
}


void draw_word_content(int x, int y, const byte *ptr)
{
	disp_putstring(x, y, main_window.colors.text, ptr);
}

void show_word_content()
{
	int x, y;

	x = WORD_LIST_WIDTH + 2;
	y = TITLE_HEIGHT + 2;
	int active = words_list.active;
	
	disp_fillrect(x, y,  main_window.pos.right - 1 ,main_window.pos.bottom -1, 
		      main_window.colors.bg);
	if (strlen(words_list.words[active].ch) != 0) { 
		show_ch(x, y);
		y = y + 20;
	}

	show_symbol(x, y);
	y = y + 20;
	draw_word_content(x, y, (const byte *)words_list.words[active].content);
	cur_content_pos = words_list.words[active].content;

	while(!stack_empty()){
		stack_pop();
	}
}

/**
 * @param direct 1-down 0-up
 */
void scroll_up_content()
{
	char *ptr;
	int x, y;

	y = TITLE_HEIGHT + 2 + 20;
	x = WORD_LIST_WIDTH + 2;
        int active = words_list.active;

	if (strlen(words_list.words[active].ch) != 0) { 
		y += 20;
	}
	ptr = strstr(cur_content_pos, "\r\n");

	if (ptr == NULL) {
		ptr = cur_content_pos;
	} else {
		ptr += 2;
		stack_push(cur_content_pos);
		cur_content_pos = ptr;	
	}

	disp_fillrect(x, y,  main_window.pos.right - 1 ,main_window.pos.bottom -1, 
		      main_window.colors.bg);
	draw_word_content(x, y, (const byte *)ptr);
}

void scroll_down_content()
{
        char *ptr;
        int x, y;
        int active = words_list.active;


        y = TITLE_HEIGHT + 2 + 20;
        x = WORD_LIST_WIDTH + 2;

	if (strlen(words_list.words[active].ch) != 0) { 
		y += 20;
	}

	if (!stack_empty()){
		ptr = stack_pop();
	} else {
		ptr = words_list.words[active].content;
		
	}

	cur_content_pos = ptr;
	disp_fillrect(x, y,  main_window.pos.right - 1 ,main_window.pos.bottom -1, 
		      main_window.colors.bg);
	draw_word_content(x, y, (const byte *)ptr);

}


void show_mainwin()
{
	disp_rectangle(0, 0, main_window.pos.right, main_window.pos.bottom, 
		       main_window.colors.line);

	show_title();
	show_wordlist();
	show_word_content();
}

void mainwin_L_trigger_pushed()
{
	scroll_down_content();
}

void mainwin_R_trigger_pushed()
{
	scroll_up_content();
}

void mainwin_right_pushed()
{
	wordlist_next_page();

	show_wordlist();
	show_word_content();
	play_word();

}

void mainwin_left_pushed()
{
	wordlist_previous_page();

	show_wordlist();
	show_word_content();
	play_word();

}

void mainwin_circle_pushed()
{
	if (words_list.count < WORDLIST_LENGTH) {
		return;
	}
	words_list.list ++;
	words_list.count = 0;
	words_list.active = 0;
	wordlist_reload();
	show_mainwin();
}

void mainwin_cross_pushed()
{
	if (words_list.list == 0){
		return;
	}
	words_list.list --;
	words_list.count = 0;
	words_list.active = 0;
	wordlist_reload();
	show_mainwin();
}


void mainwin_up_pushed() 
{	
	wordlist_previous_word();

	show_wordlist();
	show_word_content();
	play_word();

}

void mainwin_down_pushed()
{
	wordlist_next_word();

	show_wordlist();
	show_word_content();
	play_word();
}

void mainwin_square_pushed()
{
	play_word();
}

void mainwin_event_handle(dword key) 
{
	if ( key== PSP_CTRL_LTRIGGER) {
		mainwin_L_trigger_pushed();
	} else if (key == PSP_CTRL_RTRIGGER) {
		mainwin_R_trigger_pushed();
	} else if (key == PSP_CTRL_CIRCLE) {
		mainwin_circle_pushed();
	} else if (key == PSP_CTRL_CROSS) {
		mainwin_cross_pushed();
	} else if (key == PSP_CTRL_UP) {
		mainwin_up_pushed();
	} else if (key == PSP_CTRL_DOWN) {
		mainwin_down_pushed();
	} else if (key == PSP_CTRL_LEFT) {
		mainwin_left_pushed();
	} else if (key == PSP_CTRL_RIGHT) {
		mainwin_right_pushed();
	} else if (key == PSP_CTRL_SQUARE) {
		mainwin_square_pushed();
	} else if (key == PSP_CTRL_TRIANGLE) {
		active_libsel_window();
	} else if (key == PSP_CTRL_START) {
		active_auto_window();
	}

}

void init_main_window()
{
	PSP_WINDOW_P w;
	w = &main_window;

	strncpy(w->title, "PSP ±³µ¥´Ê", sizeof(w->title) - 1);
	
	w->pos.top = 0;
	w->pos.left = 0;
	w->pos.bottom = SCR_HEIGHT - 1;
	w->pos.right = SCR_WIDTH - 1;

	w->handle_event = mainwin_event_handle;
	w->show = show_mainwin;

	w->colors.bg = BLACK;
	w->colors.text = WHITE;
	w->colors.title = BLACK;
	w->colors.line = LIGHT_GRAY;
	w->colors.hi_text = RED;	
}

