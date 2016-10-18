#include "window.h"
#include "mainwin.h"
#include "libsel-win.h"
#include "autowin.h"

PSP_WINDOW_P active_win;

void window_init() 
{
	init_main_window();
	init_libsel_window();
	init_auto_window();

	active_win = &main_window;
	active_win->show();
} 

void active_auto_window()
{
	active_win = &auto_window;
	active_win->show();
}

void close_auto_window()
{
	active_win = &main_window;
	active_win->show();

}

void active_libsel_window()
{
	active_win = &libsel_window;
	active_win->show();
}

void close_libsel_window()
{
	active_win = &main_window;
	active_win->show();
}
