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
#include "quadspi.h"
#include "Adafruit_ST7735.h"
#include "frame_buffer.h"
#include "main.h"

static uint8_t rotation;
static uint8_t colstart, rowstart, xstart, ystart;	// some displays need this changed
static uint16_t _width, _height;

const struct st7735_cmd st7735_swreset = {
	.cmd = ST7735_SWRESET,
	.delay = 150
};

const struct st7735_cmd st7735_slpout = {
	.cmd = ST7735_SLPOUT,
	.delay = 500
};

const struct st7735_cmd st7735_frmctr1 = {
	.cmd = ST7735_FRMCTR1,
	.delay = 0,
	.cnt = 3,
	.data = {0x01, 0x2C, 0x2D}
};

const struct st7735_cmd st7735_frmctr2 = {
	.cmd = ST7735_FRMCTR2,
	.delay = 0,
	.cnt = 3,
	.data = {0x01, 0x2C, 0x2D}
};

const struct st7735_cmd st7735_frmctr3 = {
	.cmd = ST7735_FRMCTR3,
	.delay = 0,
	.cnt = 6,
	.data = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D}
};

const struct st7735_cmd st7735_invctr = {
	.cmd = ST7735_INVCTR,
	.delay = 0,
	.cnt = 1,
	.data = {0x07}
};

const struct st7735_cmd st7735_pwctr1 = {
	.cmd = ST7735_PWCTR1,
	.delay = 0,
	.cnt = 3,
	.data = {0xA2, 0x02, 0x84}
};

const struct st7735_cmd st7735_pwctr2 = {
	.cmd = ST7735_PWCTR2,
	.delay = 0,
	.cnt = 1,
	.data = {0xC5}
};

const struct st7735_cmd st7735_pwctr3 = {
	.cmd = ST7735_PWCTR3,
	.delay = 0,
	.cnt = 2,
	.data = {0x0A, 0x00}
};

const struct st7735_cmd st7735_pwctr4 = {
	.cmd = ST7735_PWCTR4,
	.delay = 0,
	.cnt = 2,
	.data = {0x8A, 0x2A}
};

const struct st7735_cmd st7735_pwctr5 = {
	.cmd = ST7735_PWCTR5,
	.delay = 0,
	.cnt = 2,
	.data = {0x8A, 0xEE}
};

const struct st7735_cmd st7735_vmctr1 = {
	.cmd = ST7735_VMCTR1,
	.delay = 0,
	.cnt = 1,
	.data = {0x0E}
};

const struct st7735_cmd st7735_invoff = {
	.cmd = ST7735_INVOFF,
	.delay = 0,
	.cnt = 0,
};

const struct st7735_cmd st7735_madctl = {
	.cmd = ST7735_MADCTL,
	.delay = 0,
	.cnt = 1,
	.data = {0xC8}
};

const struct st7735_cmd st7735_colmod = {
	.cmd = ST7735_COLMOD,
	.delay = 0,
	.cnt = 1,
	.data = {0x05}
};

const struct st7735_cmd st7735_caset = {
	.cmd = ST7735_CASET,
	.delay = 0,
	.cnt = 4,
	.data = {0x00, 0x00, 0x00, 0x7F}
};

const struct st7735_cmd st7735_raset = {
	.cmd = ST7735_RASET,
	.delay = 0,
	.cnt = 4,
	.data = {0x00, 0x00, 0x00, 0x9F}
};

const struct st7735_cmd st7735_gmctrp1 = {
	.cmd = ST7735_GMCTRP1,
	.delay = 0,
	.cnt = 16,
	.data =
	    {0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d, 0x29, 0x25, 0x2B,
	     0x39, 0x00, 0x01, 0x03, 0x10}
};

const struct st7735_cmd st7735_gmctrn1 = {
	.cmd = ST7735_GMCTRN1,
	.delay = 0,
	.cnt = 16,
	.data =
	    {0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37,
	     0x3F, 0x00, 0x00, 0x02, 0x10}
};

const struct st7735_cmd st7735_noron = {
	.cmd = ST7735_NORON,
	.delay = 10,
	.cnt = 0
};

const struct st7735_cmd st7735_dispon = {
	.cmd = ST7735_DISPON,
	.delay = 100,
	.cnt = 0
};

const struct st7735_cmd st7735_ramwr = {
	.cmd = ST7735_RAMWR,
	.delay = 0,
	.cnt = 0,
};

/**
  * @brief  Wait for a flag state until timeout.
  * @param  hqspi QSPI handle
  * @param  Flag Flag checked
  * @param  State Value of the flag expected
  * @param  Tickstart Tick start value
  * @param  Timeout Duration of the timeout
  * @retval HAL status
  */
HAL_StatusTypeDef QSPI_WaitFlagStateUntilTimeout(QSPI_HandleTypeDef * hqspi,
						 uint32_t Flag,
						 FlagStatus State,
						 uint32_t Tickstart,
						 uint32_t Timeout)
{
	/* Wait until flag is in expected state */
	while ((__HAL_QSPI_GET_FLAG(hqspi, Flag)) != State) {
		/* Check for the Timeout */
		if (Timeout != HAL_MAX_DELAY) {
			if (((HAL_GetTick() - Tickstart) > Timeout)
			    || (Timeout == 0U)) {
				hqspi->State = HAL_QSPI_STATE_ERROR;
				hqspi->ErrorCode |= HAL_QSPI_ERROR_TIMEOUT;

				return HAL_ERROR;
			}
		}
	}
	return HAL_OK;
}

// Write a entire frame to the TFT
HAL_StatusTypeDef qspi_write_frame(uint16_t * frame)
{
	HAL_StatusTypeDef status;
	uint32_t tickstart = HAL_GetTick();
//      __IO uint32_t *data_reg=&(hqspi.Instance->DR);;

	// Check our busy
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_BUSY, RESET,
					   tickstart, 10000);
	if (status != HAL_OK) {
		return HAL_BUSY;
	}
	// Now we need to set up the QSPI write command
	WRITE_REG(hqspi.Instance->DLR, 160 * 128 * 2 - 1);
	WRITE_REG(hqspi.Instance->CCR,
		  QSPI_DDR_MODE_DISABLE | QSPI_SIOO_INST_ONLY_FIRST_CMD |
		  QSPI_ADDRESS_NONE | QSPI_INSTRUCTION_NONE |
		  QSPI_ALTERNATE_BYTES_NONE | QSPI_DATA_1_LINE);
	HAL_QSPI_Transmit_DMA(&hqspi, (uint8_t *) frame);
#if 0
	// Write out frame data
	// CS is managed in hardware
	for (i = 0; i < 160 * 128; i++) {
		*((__IO uint16_t *) data_reg) = frame[i];
	}
	// Wait until finished
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_TC, SET, tickstart,
					   500);
	if (status == HAL_OK) {
		__HAL_QSPI_CLEAR_FLAG(&hqspi, QSPI_FLAG_TC);
	}
#endif

	return HAL_OK;
}

// Send the command part
HAL_StatusTypeDef qspi_write_cmd(const uint8_t cmd)
{
	HAL_StatusTypeDef status;
	uint32_t tickstart = HAL_GetTick();

	// Check our busy
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_BUSY, RESET,
					   tickstart, 10000);
	if (status != HAL_OK) {
		return HAL_BUSY;
	}
	// DC Command
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET);
	// This is set up as an instruction
	WRITE_REG(hqspi.Instance->CCR,
		  QSPI_SIOO_INST_ONLY_FIRST_CMD | QSPI_ADDRESS_NONE |
		  QSPI_INSTRUCTION_1_LINE | QSPI_ALTERNATE_BYTES_NONE |
		  QSPI_DATA_NONE | cmd);
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_TC, SET, tickstart,
					   500);
	if (status == HAL_OK) {
		__HAL_QSPI_CLEAR_FLAG(&hqspi, QSPI_FLAG_TC);
	}

	return HAL_OK;
}

// Send the data part
HAL_StatusTypeDef qspi_write_data(const uint8_t * data, const uint8_t cnt)
{
	HAL_StatusTypeDef status;
	uint32_t tickstart = HAL_GetTick();
	__IO uint32_t *data_reg = &(hqspi.Instance->DR);
	uint32_t i;

	// Check our busy
	if (cnt == 0)
		return HAL_OK;
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_BUSY, RESET,
					   tickstart, 10000);
	if (status != HAL_OK) {
		return HAL_BUSY;
	}
	// DC - data
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
	WRITE_REG(hqspi.Instance->DLR, cnt - 1);
	WRITE_REG(hqspi.Instance->CCR,
		  QSPI_DDR_MODE_DISABLE | QSPI_ADDRESS_NONE |
		  QSPI_INSTRUCTION_NONE | QSPI_ALTERNATE_BYTES_NONE |
		  QSPI_DATA_1_LINE);
	// Write out data
	// CS is managed in hardware
	for (i = 0; i < cnt; i++) {
		*((__IO uint8_t *) data_reg) = data[i];
	}
	status =
	    QSPI_WaitFlagStateUntilTimeout(&hqspi, QSPI_FLAG_TC, SET, tickstart,
					   500);
	if (status == HAL_OK) {
		__HAL_QSPI_CLEAR_FLAG(&hqspi, QSPI_FLAG_TC);
	}

	return HAL_OK;
}

// Send a command structure to the TFT
HAL_StatusTypeDef qspi_write_cmd_data(const struct st7735_cmd *cmd)
{
	// Split the write into a command and then data
	// This is needed because the DC pin needs to change after the first byte
	qspi_write_cmd(cmd->cmd);
	qspi_write_data(cmd->data, cmd->cnt);
	HAL_Delay(cmd->delay);

	return HAL_OK;
}

// Initialize the TFT
void Adafruit_ST7735_init(void)
{
	// Reset
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, 1);
	HAL_Delay(1);
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, 0);
	HAL_Delay(1);
	HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, 1);
	HAL_Delay(500);
	// Initialize
	qspi_write_cmd_data(&st7735_swreset);
	qspi_write_cmd_data(&st7735_slpout);
	qspi_write_cmd_data(&st7735_frmctr1);
	qspi_write_cmd_data(&st7735_frmctr2);
	qspi_write_cmd_data(&st7735_frmctr3);
	qspi_write_cmd_data(&st7735_invctr);
	qspi_write_cmd_data(&st7735_pwctr1);
	qspi_write_cmd_data(&st7735_pwctr2);
	qspi_write_cmd_data(&st7735_pwctr3);
	qspi_write_cmd_data(&st7735_pwctr4);
	qspi_write_cmd_data(&st7735_pwctr5);
	qspi_write_cmd_data(&st7735_vmctr1);
	qspi_write_cmd_data(&st7735_invoff);
	qspi_write_cmd_data(&st7735_madctl);
	qspi_write_cmd_data(&st7735_colmod);
	qspi_write_cmd_data(&st7735_caset);
	qspi_write_cmd_data(&st7735_raset);
	qspi_write_cmd_data(&st7735_gmctrp1);
	qspi_write_cmd_data(&st7735_gmctrn1);
	qspi_write_cmd_data(&st7735_noron);
	qspi_write_cmd_data(&st7735_dispon);
	Adafruit_ST7735_setRotation(1);
	qspi_write_cmd_data(&st7735_ramwr);
	HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET);
}

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void Adafruit_ST7735_setRotation(uint8_t m)
{

	struct st7735_cmd cmd = {
		.cmd = ST7735_MADCTL,
		.cnt = 1,
		.delay = 0,
//      n       .data={                                 }
//      n       .data={                        MADCTL_MV}
//  n   .data={            MADCTL_MY            }
//              .data={            MADCTL_MY | MADCTL_MV}
		.data = {MADCTL_MX}
//              .data={MADCTL_MX             | MADCTL_MV}
//              .data={MADCTL_MX | MADCTL_MY            }
//              .data={MADCTL_MX | MADCTL_MY | MADCTL_MV}
	};
	rotation = 1;

	qspi_write_cmd_data(&cmd);

	_width = ST7735_TFTHEIGHT_160;
	_height = ST7735_TFTWIDTH_128;

	ystart = colstart;
	xstart = rowstart;
}
