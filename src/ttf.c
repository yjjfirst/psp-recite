#include <pspkernel.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include <math.h>
#include "ttf.h"
#include "common/log.h"
#include "display.h"
#include "gbk2uni.h"

FT_Library    library;
FT_Face       face;
int char_width = 36;
int char_height = 40;

void ttf_init()
{
	FT_Matrix     matrix;
	FT_Vector     pen;      


	FT_Init_FreeType( &library );
	FT_New_Face( library, "fonts/simkai.ttf", 0, &face );
	FT_Set_Pixel_Sizes(face, char_width, char_height);
	//FT_Set_Char_Size(face, char_width*10, char_height*10, SCR_WIDTH, SCR_HEIGHT);
	float angle = 0;
	matrix.xx = (FT_Fixed) (cos (angle) * 0x10000L);
	matrix.xy = (FT_Fixed) (-sin (angle) * 0x10000L);
	matrix.yx = (FT_Fixed) (sin (angle) * 0x10000L);
	matrix.yy = (FT_Fixed) (cos (angle) * 0x10000L);

	FT_Set_Transform( face, &matrix, &pen );
}

int ttf_draw_char(int x, int y, int ch)
{
	FT_Glyph glyph;
	FT_BitmapGlyph bitmap_glyph;
	FT_Bitmap  *bitmap;
	int width, height;
	int i,j;

	FT_Load_Glyph(face, FT_Get_Char_Index (face, ch), FT_LOAD_DEFAULT);
	FT_Get_Glyph(face->glyph, &glyph);

	FT_Render_Glyph (face->glyph, FT_RENDER_MODE_NORMAL);
	FT_Glyph_To_Bitmap (&glyph, ft_render_mode_normal, 0, 1);
	bitmap_glyph = (FT_BitmapGlyph ) glyph;

	bitmap = &bitmap_glyph->bitmap;
	
	width = bitmap->width;
	height = bitmap->rows;
	
	dword *addr = disp_get_vaddr(x, y);
	
	for (j = 0; j < height; j ++) {
		for (i = 0; i < width; i ++) {
			addr[i + (j-bitmap_glyph->top) * 512] =
				bitmap->buffer[i + bitmap->width *j];
		}

	}

	return width;
}

void ttf_draw_string(int x, int y, unsigned char *text)
{
	int pos = 0;
	int zh_char;
	int width;
	int pos_x = x;
	int pos_y = y;

	pos_y += char_height;
	while ( text[pos] != '\0' ) {
		if (text[pos] > 0x80) { //chinese
			zh_char = gbk2unicode(text[pos], text[pos + 1]);
			width = ttf_draw_char(pos_x, pos_y, zh_char);			
			pos_x += width + 2;
			if (pos_x + char_width > SCR_WIDTH) {
				pos_x = x;
				pos_y += char_height;
			}
			pos += 2;
		} else if (text[pos] > 0x1F) { //ascii
			width = ttf_draw_char(pos_x, pos_y, text[pos]);
			pos_x += width + 2;
			if (pos_x + char_width > SCR_WIDTH) {
				pos_x = x;
				pos_y += char_height;
			}
			pos ++;
		} else {
			pos ++;
		}
	}
}
