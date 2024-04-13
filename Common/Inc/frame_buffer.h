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

#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define FRAME_BUFFER_WIDTH 160
#define FRAME_BUFFER_HEIGHT 128

// Index into frame buffer array for (x,y)
inline int16_t frame_buffer_idx(int16_t x, int16_t y)
{
	return x*FRAME_BUFFER_HEIGHT+y;
}

extern uint16_t frame[160*128];

void frame_buffer_setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void frame_buffer_pushColor(uint16_t color);
void frame_buffer_fillScreen(uint16_t color);
void frame_buffer_drawPixel(int16_t x, int16_t y, uint16_t color);
void frame_buffer_drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
void frame_buffer_drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
void frame_buffer_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void frame_buffer_setRotation(uint8_t r);
uint16_t frame_buffer_Color565(uint8_t r, uint8_t g, uint8_t b);

#endif	// FRAME_BUFFER_H
