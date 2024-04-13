/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    eth.c
  * @brief   This file provides code for the configuration
  *          of the ETH instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "eth.h"

#if defined ( __ICCARM__ )	/*!< IAR Compiler */

#pragma location=0x30000000
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT];	/* Ethernet Rx DMA Descriptors */
#pragma location=0x30000200
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT];	/* Ethernet Tx DMA Descriptors */

#elif defined ( __CC_ARM )	/* MDK ARM Compiler */

__attribute__((at(0x30000000)))
ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT];	/* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30000200)))
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT];	/* Ethernet Tx DMA Descriptors */

#elif defined ( __GNUC__ )	/* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection")));	/* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));	/* Ethernet Tx DMA Descriptors */

#endif
ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT * 2U];
ETH_TxPacketConfig TxConfig;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ETH_HandleTypeDef heth;

/* ETH init function */
void MX_ETH_Init(void)
{

	/* USER CODE BEGIN ETH_Init 0 */

	/* USER CODE END ETH_Init 0 */

	static uint8_t MACAddr[6];

	/* USER CODE BEGIN ETH_Init 1 */

	/* USER CODE END ETH_Init 1 */
	heth.Instance = ETH;
	MACAddr[0] = 0x00;
	MACAddr[1] = 0x80;
	MACAddr[2] = 0xE1;
	MACAddr[3] = 0x00;
	MACAddr[4] = 0x00;
	MACAddr[5] = 0x00;
	heth.Init.MACAddr = &MACAddr[0];
	heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
	heth.Init.TxDesc = DMATxDscrTab;
	heth.Init.RxDesc = DMARxDscrTab;
	heth.Init.RxBuffLen = 1524;

	/* USER CODE BEGIN MACADDRESS */

	/* USER CODE END MACADDRESS */

	if (HAL_ETH_Init(&heth) != HAL_OK) {
		Error_Handler();
	}

	memset(&TxConfig, 0, sizeof(ETH_TxPacketConfig));
	TxConfig.Attributes =
	    ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
	TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
	/* USER CODE BEGIN ETH_Init 2 */

	/* USER CODE END ETH_Init 2 */

}

void HAL_ETH_MspInit(ETH_HandleTypeDef * ethHandle)
{

	if (ethHandle->Instance == ETH) {
		/* USER CODE BEGIN ETH_MspInit 0 */

		/* USER CODE END ETH_MspInit 0 */
		/* ETH clock enable */
		__HAL_RCC_ETH1MAC_CLK_ENABLE();
		__HAL_RCC_ETH1TX_CLK_ENABLE();
		__HAL_RCC_ETH1RX_CLK_ENABLE();
		/* USER CODE BEGIN ETH_MspInit 1 */

		/* USER CODE END ETH_MspInit 1 */
	}
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef * ethHandle)
{

	if (ethHandle->Instance == ETH) {
		/* USER CODE BEGIN ETH_MspDeInit 0 */

		/* USER CODE END ETH_MspDeInit 0 */
		/* Peripheral clock disable */
		__HAL_RCC_ETH1MAC_CLK_DISABLE();
		__HAL_RCC_ETH1TX_CLK_DISABLE();
		__HAL_RCC_ETH1RX_CLK_DISABLE();
		/* USER CODE BEGIN ETH_MspDeInit 1 */

		/* USER CODE END ETH_MspDeInit 1 */
	}
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
