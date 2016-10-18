#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <pspdisplay.h>
#include "common/datatype.h"

#define DISP_FONTSIZE 16

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define PIXEL_SIZE (4) /* change this if you change to another screenmode */
#define FRAME_SIZE (BUF_WIDTH * SCR_HEIGHT * PIXEL_SIZE)
#define ZBUF_SIZE (BUF_WIDTH SCR_HEIGHT * 2) /* zbuffer seems to be 16-bit? */

extern dword * vram_start;

// R,G,B color to word value color
#define RGB(r,g,b) ((((dword)(b))<<16)|(((dword)(g))<<8)|((dword)(r))|0xFF000000)
#define RGBA(r,g,b,a) ((((dword)(b))<<16)|(((dword)(g))<<8)|((dword)(r))|(((dword)(a))<<24))
#define RGB_R(c) ((c) & 0xFF)
#define RGB_G(c) (((c) >> 8) & 0xFF)
#define RGB_B(c) (((c) >> 16) & 0xFF)
#define RGB_16to32(c) RGBA((((c)&0x1F)*255/31),((((c)>>5)&0x1F)*255/31),((((c)>>10)&0x1F)*255/31),((c&0x8000)?0xFF:0))


#define LIGHT_GRAY  (RGB(214,211,206))
#define GRAY  (RGB(64,64,64))
#define WHITE (RGB(255,255,255))
#define RED   (RGB(255,0,0))
#define GREEN (RGB(0,255,0))
#define BLUE  (RGB(0,0,255))
#define BLACK (RGB(0,0,0))

#define BACKGROUND (RGB(128,132,141))

#define HIGH_LIGHT_MAX   48
#define HIGH_LIGHT_MARK  "<*=*>"

// sceDisplayWaitVblankStart function alias name, define is faster than function call (even at most time this is inline linked)
#define disp_waitv() sceDisplayWaitVblankStart()

#define disp_get_vaddr(x, y) (vram_start + (x) + ((y) << 9))

#define disp_putpixel(x,y,c) *(dword*)disp_get_vaddr((x),(y)) = (c)

extern void disp_init();

extern bool disp_load_font(const char * efont, const char * cfont);

extern void disp_free_font();

extern void disp_flip();

extern void disp_getimage(dword x, dword y, dword w, dword h, dword * buf);

extern void disp_putimage(dword x, dword y, dword w, dword h, dword startx, dword starty, dword * buf);

extern void disp_duptocache();

extern void disp_rectduptocache(dword x1, dword y1, dword x2, dword y2);

extern void disp_putnstring(word x, word y, dword color, const byte *str, int count);
#define disp_putstring(x,y,color,str) disp_putnstring((x),(y),(color),(str),0x7FFFFFFF)

extern void disp_putnstringvert(word x, word y, dword color, const byte *str, int count);
#define disp_putstringvert(x,y,color,str) disp_putnstringvert((x),(y),(color),(str),0x7FFFFFFF)

extern void disp_fillvram(dword color);

extern void disp_fillrect(dword x1, dword y1, dword x2, dword y2, dword color);

extern void disp_rectangle(dword x1, dword y1, dword x2, dword y2, dword color);

extern void disp_line(dword x1, dword y1, dword x2, dword y2, dword color);

extern void symbol_draw_string(int x, int y, dword color, char *str);
#endif
