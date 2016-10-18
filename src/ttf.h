#ifndef _TTF_H_
#define _TTF_H_

extern void ttf_init();
extern void ttf_draw_string(int x, int y, unsigned char *text);
extern int  ttf_draw_char(int x, int y, int ch);

#endif
