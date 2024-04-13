/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "stm32h7xx_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "frame_buffer.h"

#include <stdint.h>

uint16_t frame[160*128] __attribute__ (( section(".axi") ));

void frame_buffer_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	return;
}

// Flood fill the entire screen
void frame_buffer_fillScreen(uint16_t color)
{
	uint32_t i;

	for (i=0;i<FRAME_BUFFER_WIDTH*FRAME_BUFFER_HEIGHT;i+=4) {
		frame[i]=color;
		frame[i+1]=color;
		frame[i+2]=color;
		frame[i+3]=color;
	}
}

// Draw a color at [x,y]
void frame_buffer_drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if (x<0) return;
	if (x>=FRAME_BUFFER_WIDTH) return;
	if (y<0) return;
	if (y>=FRAME_BUFFER_HEIGHT) return;

	frame[frame_buffer_idx(x,y)]=color;
}

void frame_buffer_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	uint32_t i=0;

	if (x<0) return;
	if (x>=FRAME_BUFFER_WIDTH) return;
	if (y<0) return;
	if (y>=FRAME_BUFFER_HEIGHT) return;

	if (y+h>=FRAME_BUFFER_HEIGHT) h=FRAME_BUFFER_HEIGHT-y;

	for (i=frame_buffer_idx(x,y);i<frame_buffer_idx(x,y+h);i++) {
		frame[i]=color;
	}
}

void frame_buffer_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	uint32_t i=0;

	if (x<0) return;
	if (x>=FRAME_BUFFER_WIDTH) return;
	if (y<0) return;
	if (y>=FRAME_BUFFER_HEIGHT) return;

	if (x+w>=FRAME_BUFFER_WIDTH) w=FRAME_BUFFER_WIDTH-w;

	for (i=frame_buffer_idx(x,y);i<frame_buffer_idx(x+w,y);i++) {
		frame[i]=color;
	}
}

void frame_buffer_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	uint32_t i=0;
	uint32_t j=0;

	if (x<0) return;
	if (x>=FRAME_BUFFER_WIDTH) return;
	if (y<0) return;
	if (y>=FRAME_BUFFER_HEIGHT) return;

	if (x+w>=FRAME_BUFFER_WIDTH) w=FRAME_BUFFER_WIDTH-x;
	if (y+h>=FRAME_BUFFER_HEIGHT) h=FRAME_BUFFER_HEIGHT-y;

	for (i=x;i<x+w;i++) {
		for (j=y;j<y+h;j++) {
			frame[frame_buffer_idx(i,j)]=color;
		}
	}


}

// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t frame_buffer_Color565(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


inline uint16_t swapcolor(uint16_t x)
{
	return (x << 11) | (x & 0x07E0) | (x >> 11);
}
