#include <string.h>
#include <stdio.h>
#include <pspctrl.h>

#include "window.h"
#include "autowin.h"
#include "wordlist.h"
#include "wordlib.h"
#include "version.h"
#include "stack.h"
#include "mp3.h"
#include "ttf.h"
#include "wordlist.h"
#include "wordlib.h"
#include "common/log.h"
#include "recite-timer.h"

PSP_WINDOW auto_window;
static struct recite_timer auto_win_timer; 

#define HIGH_LIGHT_MARK  "<*=*>"
#define SINGLE_LINE_LEN 64
#define TOTAL_LEN       140

void update_autowin(void) 
{
	int active = words_list.active;
	WORD_STRUCTURE_P word;
	char *ptr;
	char text[TOTAL_LEN];
	char line_text[SINGLE_LINE_LEN];
	char temp[8];
	int x_off = 10;	
	int i;
	int line;
	

	word = &words_list.words[active];
	disp_fillrect(0, 0, auto_window.pos.right, auto_window.pos.bottom, 
			   auto_window.colors.bg);
	disp_rectangle(0, 0, auto_window.pos.right, auto_window.pos.bottom, 
			   auto_window.colors.line);
	disp_rectangle(0, 0, auto_window.pos.right, 60, 
			   auto_window.colors.line);

	ttf_draw_string(x_off,10, (unsigned char *)word->spell);
	ttf_draw_string(x_off,70, (unsigned char *)word->ch);
	
	ptr = word->content;
	line = 0;
	bzero(text, sizeof(text));
	while ( (ptr = strstr(ptr, HIGH_LIGHT_MARK)) != NULL && line < 4) {
		ptr += strlen(HIGH_LIGHT_MARK);
		i = 0;
		bzero(line_text, sizeof(line_text));
		while(*ptr != 0x0d && i < SINGLE_LINE_LEN - 1) {
			line_text[i++] = *ptr++;
		}

		if (strlen(text) + strlen(line_text) > TOTAL_LEN){
			break;
		}

		line ++;
		sprintf(temp, " %d.", line);
		strcat(text, temp);
		strcat(text, line_text);
	}

	ttf_draw_string(x_off, 60, (unsigned char *)text);
}

void autowin_next_word(void)
{
	wordlist_next_word();
	update_autowin();
	play_word();

}

void autowin_previous_word(void)
{
	wordlist_previous_word();
	update_autowin();
	play_word();

}

void timer_handler(void *data)
{
	autowin_next_word();
	register_timer(&auto_win_timer);
	return;
}

void show_autowin(void)
{
	update_autowin();
	init_timer(&auto_win_timer, 5000, timer_handler);
	register_timer(&auto_win_timer);
}


void autowin_event_handle(dword key)
{
	if (key == PSP_CTRL_CROSS) {
		remove_timer(auto_win_timer.id);
		close_auto_window();
	} else if (key == PSP_CTRL_DOWN) {
		autowin_next_word();
	} else if (key == PSP_CTRL_UP) {
		autowin_previous_word();
	}
}

void init_auto_window(void)
{
	PSP_WINDOW_P w;
	w = &auto_window;

	strncpy(w->title, "auto browse", sizeof(w->title) - 1);
	
	w->pos.top = 0;
	w->pos.left = 0;
	w->pos.bottom = SCR_HEIGHT -1;
	w->pos.right = SCR_WIDTH -1;

	w->handle_event = autowin_event_handle;
	w->show = show_autowin;

	w->colors.bg = BLACK;
	w->colors.text = WHITE;
	w->colors.title = BLACK;
	w->colors.line = LIGHT_GRAY;
	w->colors.hi_text = RED;	
}
