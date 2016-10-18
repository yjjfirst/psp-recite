#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <./common/datatype.h>
#define TITLE_HEIGHT 24

typedef struct _POSITION {
	int left;
	int top;
	int right;
	int bottom;
}POSITION, *POSITION_P;

typedef struct _COLORS {
    dword bg;
    dword text;
    dword title;
    dword line;
    dword hi_text;
	
}COLORS,*COLORS_P;

typedef struct _WINDOW_STRUCTURE PSP_WINDOW;
typedef struct _WINDOW_STRUCTURE *PSP_WINDOW_P;

struct _WINDOW_STRUCTURE {
	POSITION pos;
	PSP_WINDOW_P  parent;
	PSP_WINDOW_P  self;
	char title[32];
	COLORS colors;
	
	void (*set_position)(int left, int top, int right, int buttom);
	void (*show)();
	void (*handle_event)(dword key);

	void *private;
};

extern PSP_WINDOW_P active_win;
extern void window_init(); 
extern void active_libsel_window();
extern void close_libsel_window();
extern void active_auto_window();
extern void close_auto_window();
#endif
