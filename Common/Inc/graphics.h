// Adafruit graphics routines
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stdbool.h>
#include "gfxfont.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b) { int16_t t = a; a = b; b = t; }
#endif

#define WIDTH 160
#define HEIGHT 128

struct graphics {
	uint16_t width;
	uint16_t height;
	uint16_t rotation;
	uint16_t cursor_x;
	uint16_t cursor_y;
	uint16_t textcolor;
	uint16_t textsize;
	uint16_t textbgcolor;
	bool wrap;
	GFXfont *font;
	bool cp437;
};

// Box for text bounds
struct bounds {
	int16_t x;	// Start x
	int16_t y;	// Start y
	uint16_t w;	// Width
	uint16_t h;	// Height
};


inline uint16_t graphics_mk_color(uint16_t c)
{
	return __builtin_bswap16(c);
}

void graphics_init(uint16_t w, uint16_t h);
void graphics_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void graphics_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
void graphics_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void grahics_drawCircleHelper( int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
void grahics_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void graphics_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
void graphics_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
int16_t graphics_width(void);
int16_t graphics_height(void);
void graphics_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size);

size_t graphics_write(uint8_t c);
void graphics_setCursor(int16_t x, int16_t y);
int16_t graphics_getCursorX(void);
int16_t graphics_getCursorY(void);
void graphics_setTextSize(uint8_t s);
void graphics_setTextColor(uint16_t c);
void graphics_setTextColor2(uint16_t c, uint16_t b);
void graphics_setTextWrap(bool w);
void graphics_setRotation(uint8_t x);
void graphics_cp437(bool x);
void graphics_setFont(const GFXfont * f);
void graphics_charBounds(char c, int16_t * x, int16_t * y, int16_t * minx, int16_t * miny, int16_t * maxx, int16_t * maxy);
void graphics_getTextBounds(char *str, int16_t x, int16_t y, int16_t * x1, int16_t * y1, uint16_t * w, uint16_t * h);
size_t graphics_drawText(char *s, uint16_t x, uint16_t y);
void graphics_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);



#endif				// GRAPHICS_H
