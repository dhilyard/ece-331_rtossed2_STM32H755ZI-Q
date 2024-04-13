/*
This is the core graphics library for all our displays, providing a common
set of graphics primitives (points, lines, circles, etc.).  It needs to be
paired with a hardware-specific library for each display device we carry
(to handle the lower-level functions).

Adafruit invests time and resources providing this open source code, please
support Adafruit & open-source hardware by purchasing products from Adafruit!

Copyright (c) 2013 Adafruit Industries.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

- Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
- Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "graphics.h"
#include "frame_buffer.h"
#include "stm32h7xx_hal.h"


static struct graphics gfx_prop = {
	.width = 0,
	.height = 0,
	.rotation = 0,
	.rotation = 0,
	.cursor_x = 0,
	.cursor_y = 0,
	.textsize = 1,
	.textcolor = 0,
	.textbgcolor = 0,
	.wrap = false,
	.font = NULL,
	.cp437 = 0,
};

void graphics_init(uint16_t w, uint16_t h)
{
	gfx_prop.width = w;
	gfx_prop.height = h;
}


void graphics_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	// Update in subclasses if desired!
	if (x0 == x1) {
		if (y0 > y1)
			_swap_int16_t(y0, y1);
		frame_buffer_drawFastVLine(x0, y0, y1 - y0 + 1, color);
	} else if (y0 == y1) {
		if (x0 > x1)
			_swap_int16_t(x0, x1);
		frame_buffer_drawFastHLine(x0, y0, x1 - x0 + 1, color);
	} else {
		graphics_writeLine(x0, y0, x1, y1, color);
	}
}

// Bresenham's algorithm - thx wikpedia
void graphics_writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color)
{
	int16_t steep = abs(y1 - y0) > abs(x1 - x0);
	if (steep) {
		_swap_int16_t(x0, y0);
		_swap_int16_t(x1, y1);
	}

	if (x0 > x1) {
		_swap_int16_t(x0, x1);
		_swap_int16_t(y0, y1);
	}

	int16_t dx, dy;
	dx = x1 - x0;
	dy = abs(y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++) {
		if (steep) {
			frame_buffer_drawPixel(y0, x0, color);
		} else {
			frame_buffer_drawPixel(x0, y0, color);
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

// Draw a circle outline
void graphics_drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	frame_buffer_drawPixel(x0, y0 + r, color);
	frame_buffer_drawPixel(x0, y0 - r, color);
	frame_buffer_drawPixel(x0 + r, y0, color);
	frame_buffer_drawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		frame_buffer_drawPixel(x0 + x, y0 + y, color);
		frame_buffer_drawPixel(x0 - x, y0 + y, color);
		frame_buffer_drawPixel(x0 + x, y0 - y, color);
		frame_buffer_drawPixel(x0 - x, y0 - y, color);
		frame_buffer_drawPixel(x0 + y, y0 + x, color);
		frame_buffer_drawPixel(x0 - y, y0 + x, color);
		frame_buffer_drawPixel(x0 + y, y0 - x, color);
		frame_buffer_drawPixel(x0 - y, y0 - x, color);
	}
}

void grahics_drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		if (cornername & 0x4) {
			frame_buffer_drawPixel(x0 + x, y0 + y, color);
			frame_buffer_drawPixel(x0 + y, y0 + x, color);
		}
		if (cornername & 0x2) {
			frame_buffer_drawPixel(x0 + x, y0 - y, color);
			frame_buffer_drawPixel(x0 + y, y0 - x, color);
		}
		if (cornername & 0x8) {
			frame_buffer_drawPixel(x0 - y, y0 + x, color);
			frame_buffer_drawPixel(x0 - x, y0 + y, color);
		}
		if (cornername & 0x1) {
			frame_buffer_drawPixel(x0 - y, y0 - x, color);
			frame_buffer_drawPixel(x0 - x, y0 - y, color);
		}
	}
}

void grahics_fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	frame_buffer_drawFastVLine(x0, y0 - r, 2 * r + 1, color);
	graphics_fillCircleHelper(x0, y0, r, 3, 0, color);
}

// Used to do circles and roundrects
void graphics_fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color)
{

	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		if (cornername & 0x1) {
			frame_buffer_drawFastVLine(x0 + x, y0 - y, 2 * y + 1 + delta, color);
			frame_buffer_drawFastVLine(x0 + y, y0 - x, 2 * x + 1 + delta, color);
		}
		if (cornername & 0x2) {
			frame_buffer_drawFastVLine(x0 - x, y0 - y, 2 * y + 1 + delta, color);
			frame_buffer_drawFastVLine(x0 - y, y0 - x, 2 * x + 1 + delta, color);
		}
	}
}

// Draw a rectangle
void graphics_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	frame_buffer_drawFastHLine(x, y, w, color);
	frame_buffer_drawFastHLine(x, y + h - 1, w, color);
	frame_buffer_drawFastVLine(x, y, h, color);
	frame_buffer_drawFastVLine(x + w - 1, y, h, color);
}

//
void graphics_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// Update in subclasses if desired!
	for (int16_t i = x; i < x + w; i++) {
		frame_buffer_drawFastVLine(i, y, h, color);
	}
}

// Return the size of the display (per current rotation)
int16_t graphics_width(void)
{
	return gfx_prop.width;
}

int16_t graphics_height(void)
{
	return gfx_prop.height;
}


// TEXT- AND CHARACTER-HANDLING FUNCTIONS ----------------------------------

// Draw a character
void graphics_drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size)
{
	if (!gfx_prop.font) {	// 'Classic' built-in font

		if ((x >= gfx_prop.width) ||	// Clip right
		    (y >= gfx_prop.height) ||	// Clip bottom
		    ((x + 6 * size - 1) < 0) ||	// Clip left
		    ((y + 8 * size - 1) < 0))	// Clip top
			{
			//printf("Clipped\r\n");
			return;
		}

		if (!gfx_prop.cp437 && (c >= 176))
			c++;	// Handle 'classic' charset behavior

		for (int8_t i = 0; i < 5; i++) {	// Char bitmap = 5 columns
			uint8_t line = font[c * 5 + i];
			for (int8_t j = 0; j < 8; j++, line >>= 1) {
				if (line & 1) {
					if (size == 1)
						frame_buffer_drawPixel(x + i, y + j, color);
					else
						graphics_fillRect(x + i * size, y + j * size, size, size, color);
				} else if (bg != color) {
					if (size == 1)
						frame_buffer_drawPixel(x + i, y + j, bg);
					else
						graphics_fillRect(x + i * size, y + j * size, size, size, bg);
				}
			}
		}
		if (bg != color) {	// If opaque, draw vertical line for last column
			if (size == 1)
				frame_buffer_drawFastVLine(x + 5, y, 8, bg);
			else
				graphics_fillRect(x + 5 * size, y, size, 8 * size, bg);
		}

	} else {		// Custom font

		// Character is assumed previously filtered by write() to eliminate
		// newlines, returns, non-printable characters, etc.  Calling
		// drawChar() directly with 'bad' characters of font may cause mayhem!

		c -= (uint8_t) gfx_prop.font->first;
		GFXglyph *glyph = &(((GFXglyph *) (&gfx_prop.font->glyph))[c]);
		uint8_t *bitmap = (uint8_t *) (&gfx_prop.font->bitmap);

		uint16_t bo = glyph->bitmapOffset;
		uint8_t w = glyph->width;
		uint8_t h = glyph->height;
		int8_t xo = glyph->xOffset;
		int8_t yo = glyph->yOffset;
		uint8_t xx, yy, bits = 0, bit = 0;
		int16_t xo16 = 0, yo16 = 0;

		if (size > 1) {
			xo16 = xo;
			yo16 = yo;
		}
		// Todo: Add character clipping here

		// NOTE: THERE IS NO 'BACKGROUND' COLOR OPTION ON CUSTOM FONTS.
		// THIS IS ON PURPOSE AND BY DESIGN.  The background color feature
		// has typically been used with the 'classic' font to overwrite old
		// screen contents with new data.  This ONLY works because the
		// characters are a uniform size; it's not a sensible thing to do with
		// proportionally-spaced fonts with glyphs of varying sizes (and that
		// may overlap).  To replace previously-drawn text when using a custom
		// font, use the getTextBounds() function to determine the smallest
		// rectangle encompassing a string, erase the area with fillRect(),
		// then draw new text.  This WILL infortunately 'blink' the text, but
		// is unavoidable.  Drawing 'background' pixels will NOT fix this,
		// only creates a new set of problems.  Have an idea to work around
		// this (a canvas object type for MCUs that can afford the RAM and
		// displays supporting setAddrWindow() and pushColors()), but haven't
		// implemented this yet.

		for (yy = 0; yy < h; yy++) {
			for (xx = 0; xx < w; xx++) {
				if (!(bit++ & 7)) {
					bits = bitmap[bo++];
				}
				if (bits & 0x80) {
					if (size == 1) {
						frame_buffer_drawPixel(x + xo + xx, y + yo + yy, color);
					} else {
						graphics_fillRect(x + (xo16 + xx) * size, y + (yo16 + yy) * size, size, size, color);
					}
				}
				bits <<= 1;
			}
		}

	}			// End classic vs custom font
}

size_t graphics_drawText(char *s, uint16_t x, uint16_t y)
{
	size_t i;

	graphics_setCursor(x, y);
	for (i=0;i<strlen(s);i++) {
		graphics_write(s[i]);
	}
	return i;
}

size_t graphics_write(uint8_t c)
{
	if (!gfx_prop.font) {	// 'Classic' built-in font

		if (c == '\n') {	// Newline?
			gfx_prop.cursor_x = 0;	// Reset x to zero,
			gfx_prop.cursor_y += gfx_prop.textsize * 8;	// advance y one line
		} else if (c != '\r') {	// Ignore carriage returns
			if (gfx_prop.wrap && ((gfx_prop.cursor_x + gfx_prop.textsize * 6) > gfx_prop.width)) {	// Off right?
				gfx_prop.cursor_x = 0;	// Reset x to zero,
				gfx_prop.cursor_y += gfx_prop.textsize * 8;	// advance y one line
			}
			graphics_drawChar(gfx_prop.cursor_x, gfx_prop.cursor_y, c, gfx_prop.textcolor, gfx_prop.textbgcolor, gfx_prop.textsize);
			gfx_prop.cursor_x += gfx_prop.textsize * 6;	// Advance x one char
		}
	} else {		// Custom font
		if (c == '\n') {
			gfx_prop.cursor_x = 0;
			gfx_prop.cursor_y += (int16_t) gfx_prop.textsize *(uint8_t) gfx_prop.font->yAdvance;
		} else if (c != '\r') {
			uint8_t first = gfx_prop.font->first;
			if ((c >= first) && (c <= (uint8_t) gfx_prop.font->last)) {
				GFXglyph *glyph = &(((GFXglyph *) gfx_prop.font->glyph)[c - first]);
				uint8_t w = glyph->width, h = glyph->height;
				if ((w > 0) && (h > 0)) {	// Is there an associated bitmap?
					int16_t xo = (int8_t) glyph->xOffset;	// sic
					if (gfx_prop.wrap && ((gfx_prop.cursor_x + gfx_prop.textsize * (xo + w)) > gfx_prop.width)) {
						gfx_prop.cursor_x = 0;
						gfx_prop.cursor_y += (int16_t) gfx_prop.textsize *(uint8_t) gfx_prop.font->yAdvance;
					}
					graphics_drawChar(gfx_prop.cursor_x, gfx_prop.cursor_y, c, gfx_prop.textcolor, gfx_prop.textbgcolor, gfx_prop.textsize);
				}
				gfx_prop.cursor_x += (uint8_t) glyph->xAdvance * (int16_t) gfx_prop.textsize;
			}
		}

	}
	return 1;
}

void graphics_setCursor(int16_t x, int16_t y)
{
	gfx_prop.cursor_x = x;
	gfx_prop.cursor_y = y;
}

int16_t graphics_getCursorX(void)
{
	return gfx_prop.cursor_x;
}

int16_t graphics_getCursorY(void)
{
	return gfx_prop.cursor_y;
}

void graphics_setTextSize(uint8_t s)
{
	gfx_prop.textsize = (s > 0) ? s : 1;
}

void graphics_setTextColor(uint16_t c)
{
	// For 'transparent' background, we'll set the bg
	// to the same as fg instead of using a flag
	gfx_prop.textcolor = gfx_prop.textbgcolor = c;
}

void graphics_setTextColor2(uint16_t c, uint16_t b)
{
	gfx_prop.textcolor = c;
	gfx_prop.textbgcolor = b;
}

void graphics_setTextWrap(bool w)
{
	gfx_prop.wrap = w;
}

uint8_t graphics_getRotation(void)
{
	return gfx_prop.rotation;
}

void graphics_setRotation(uint8_t x)
{
	gfx_prop.rotation = (x & 3);
	switch (gfx_prop.rotation) {
	case 0:
	case 2:
		gfx_prop.width = WIDTH;
		gfx_prop.height = HEIGHT;
		break;
	case 1:
	case 3:
		gfx_prop.width = HEIGHT;
		gfx_prop.height = WIDTH;
		break;
	}
}

// Enable (or disable) Code Page 437-compatible charset.
// There was an error in glcdfont.c for the longest time -- one character
// (#176, the 'light shade' block) was missing -- this threw off the index
// of every character that followed it.  But a TON of code has been written
// with the erroneous character indices.  By default, the library uses the
// original 'wrong' behavior and old sketches will still work.  Pass 'true'
// to this function to use correct CP437 character values in your code.
void graphics_cp437(bool x)
{
	gfx_prop.cp437 = x;
}

void graphics_setFont(const GFXfont *f)
{
	if (f) {		// Font struct pointer passed in?
		if (!gfx_prop.font) {	// And no current font struct?
			// Switching from classic to new font behavior.
			// Move cursor pos down 6 pixels so it's on baseline.
			gfx_prop.cursor_y += 6;
		}
	} else if (gfx_prop.font) {	// NULL passed.  Current font struct defined?
		// Switching from new to classic font behavior.
		// Move cursor pos up 6 pixels so it's at top-left of char.
		gfx_prop.cursor_y -= 6;
	}
	gfx_prop.font = (GFXfont *) f;
}

// Broke this out as it's used by both the PROGMEM- and RAM-resident
// getTextBounds() functions.
void graphics_charBounds(char c, int16_t *x, int16_t *y, int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy)
{

	if (gfx_prop.font) {

		if (c == '\n') {	// Newline?
			*x = 0;	// Reset x to zero, advance y by one line
			*y += gfx_prop.textsize * (uint8_t) gfx_prop.font->yAdvance;
		} else if (c != '\r') {	// Not a carriage return; is normal char
			uint8_t first = gfx_prop.font->first;
			uint8_t last = gfx_prop.font->last;
			if ((c >= first) && (c <= last)) {	// Char present in this font?
				GFXglyph *glyph = &(((GFXglyph *) (&gfx_prop.font->glyph))[c - first]);
				uint8_t gw = glyph->width;
				uint8_t gh = glyph->height;
				uint8_t xa = glyph->xAdvance;
				int8_t xo = glyph->xOffset;
				int8_t yo = glyph->yOffset;
				if (gfx_prop.wrap && ((*x + (((int16_t) xo + gw) * gfx_prop.textsize)) > gfx_prop.width)) {
					*x = 0;	// Reset x to zero, advance y by one line
					*y += gfx_prop.textsize * (uint8_t) gfx_prop.font->yAdvance;
				}
				int16_t ts = (int16_t) gfx_prop.textsize, x1 = *x + xo * ts, y1 = *y + yo * ts, x2 = x1 + gw * ts - 1, y2 = y1 + gh * ts - 1;
				if (x1 < *minx)
					*minx = x1;
				if (y1 < *miny)
					*miny = y1;
				if (x2 > *maxx)
					*maxx = x2;
				if (y2 > *maxy)
					*maxy = y2;
				*x += xa * ts;
			}
		}

	} else {		// Default font

		if (c == '\n') {	// Newline?
			*x = 0;	// Reset x to zero,
			*y += gfx_prop.textsize * 8;	// advance y one line
			// min/max x/y unchaged -- that waits for next 'normal' character
		} else if (c != '\r') {	// Normal char; ignore carriage returns
			if (gfx_prop.wrap && ((*x + gfx_prop.textsize * 6) > gfx_prop.width)) {	// Off right?
				*x = 0;	// Reset x to zero,
				*y += gfx_prop.textsize * 8;	// advance y one line
			}
			int x2 = *x + gfx_prop.textsize * 6 - 1,	// Lower-right pixel of char
			    y2 = *y + gfx_prop.textsize * 8 - 1;
			if (x2 > *maxx)
				*maxx = x2;	// Track max x, y
			if (y2 > *maxy)
				*maxy = y2;
			if (*x < *minx)
				*minx = *x;	// Track min x, y
			if (*y < *miny)
				*miny = *y;
			*x += gfx_prop.textsize * 6;	// Advance x one char
		}
	}
}

// Pass string and a cursor position, returns UL corner and W,H.
void graphics_getTextBounds(char *str, int16_t x, int16_t y, int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h)
{
	uint8_t c;		// Current character

	*x1 = x;
	*y1 = y;
	*w = *h = 0;

	int16_t minx = gfx_prop.width, miny = gfx_prop.height, maxx = -1, maxy = -1;

	while ((c = *str++))
		graphics_charBounds(c, &x, &y, &minx, &miny, &maxx, &maxy);

	if (maxx >= minx) {
		*x1 = minx;
		*w = maxx - minx + 1;
	}
	if (maxy >= miny) {
		*y1 = miny;
		*h = maxy - miny + 1;
	}
}
