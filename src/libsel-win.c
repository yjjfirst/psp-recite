#include <pspctrl.h>
#include "display.h"
#include "libsel-win.h"
#include "window.h"
#include "wordlib.h"
#include "wordlist.h"

PSP_WINDOW libsel_window;
int active_item;

static void libsel_draw_title()
{
	POSITION_P pos = &libsel_window.pos;
	disp_fillrect(pos->left, pos->top, pos->right, pos->top+TITLE_HEIGHT, 
		      libsel_window.colors.title);
	disp_rectangle(pos->left, pos->top, pos->right, pos->top+TITLE_HEIGHT, 
		       libsel_window.colors.line);
	disp_putstring(pos->left + 2, pos->top + 2, libsel_window.colors.text,
		       (const byte *)"ÇëÑ¡Ôñ´Ê¿â");

}

static void libsel_draw_content()
{
	int i;

	POSITION_P pos = &libsel_window.pos;

	disp_fillrect(pos->left, pos->top + TITLE_HEIGHT, pos->right, pos->bottom, 
		      libsel_window.colors.bg);
	disp_rectangle(pos->left, pos->top + TITLE_HEIGHT, pos->right, pos->bottom, 
		       libsel_window.colors.line);

	for (i = 0; word_libs[i].wordlist_load != NULL; i++) {
		int x, y;
		x = pos->left + 2;
		y = pos->top + 20*i + 2 + TITLE_HEIGHT;
		if (active_item == i) {			
			disp_fillrect(x,y, 
				       pos->right-2,y+20,GRAY);
			disp_putstring(x, y, libsel_window.colors.hi_text, 
				       (const byte *)word_libs[i].desc);

		} else {
			disp_putstring(x, y, libsel_window.colors.text, 
				       (const byte *)word_libs[i].desc);
		}
	}


}

void show_libsel_win() 
{
	libsel_draw_title();
	libsel_draw_content();
}

void libsel_event_handle(dword key)
{
	if ( key == PSP_CTRL_CIRCLE) {
		set_active_lib(active_item);
		wordlist_reset();
		wordlist_load(word_libs[active_item].desc);
		close_libsel_window();
	} else if (key == PSP_CTRL_UP) {
		active_item--;
		if (active_item < 0) {
			active_item = get_lib_count() - 1;
		}
		libsel_draw_content();
	} else if (key == PSP_CTRL_DOWN) {
		active_item ++;
		active_item %= get_lib_count();
		libsel_draw_content();
	} else if (key == PSP_CTRL_CROSS) {
		close_libsel_window();
	}

}

void init_libsel_window()
{
	PSP_WINDOW_P w;
	w = &libsel_window;

	w->pos.left = 0;
	w->pos.top = 0;
	w->pos.right = SCR_WIDTH -1;
	w->pos.bottom = SCR_HEIGHT - 1;

	active_item = get_active_lib();
	w->handle_event = libsel_event_handle;
	w->show = show_libsel_win;

	w->colors.bg = BLACK;
	w->colors.text = WHITE;
	w->colors.title = BLACK;
	w->colors.line = LIGHT_GRAY;
	w->colors.hi_text = RED;

} 


