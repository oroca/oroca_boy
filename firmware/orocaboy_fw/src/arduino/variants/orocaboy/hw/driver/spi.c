/*
 * spi.c
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */

/*

 SPI3
  SCK  PB3  AF6
  MISO PB4  AF6 DMA2 Channel1 Request3
  MOSI PB5  AF6 DMA2 Channel2 Request3

 */


#include "spi.h"




#define SPI_MODE0           0
#define SPI_MODE1           1
#define SPI_MODE2           2
#define SPI_MODE3           3






spi_t spi_tbl[SPI_CH_MAX];



bool spiInit(void)
{
  uint8_t i;


  for(i=0; i<SPI_CH_MAX; i++)
  {
    spi_tbl[i].is_open = false;

  }

  return true;
}


void spiBegin(uint8_t spi_ch)
{
  spi_t  *p_spi;



  switch(spi_ch)
  {
    case _HW_DEF_SPI1:
      p_spi = &spi_tbl[spi_ch];

      p_spi->h_spi.Instance               = SPI3;
      p_spi->h_spi.Init.Mode              = SPI_MODE_MASTER;
      p_spi->h_spi.Init.Direction         = SPI_DIRECTION_2LINES;
      p_spi->h_spi.Init.DataSize          = SPI_DATASIZE_8BIT;
      p_spi->h_spi.Init.CLKPolarity       = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase          = SPI_PHASE_1EDGE;
      p_spi->h_spi.Init.NSS               = SPI_NSS_SOFT;
      p_spi->h_spi.Init.NSSPMode          = SPI_NSS_PULSE_DISABLE;
      p_spi->h_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; // 4.5 Mbit
      p_spi->h_spi.Init.FirstBit          = SPI_FIRSTBIT_MSB;
      p_spi->h_spi.Init.TIMode            = SPI_TIMODE_DISABLE;
      p_spi->h_spi.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
      p_spi->h_spi.Init.CRCPolynomial     = 10;
      p_spi->h_spi.Init.CRCLength         = SPI_CRC_LENGTH_8BIT;

      HAL_SPI_Init(&p_spi->h_spi);

      p_spi->is_open = true;
      break;
  }
}


void spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length)
{
  uint8_t ret = 0;



  return;
}


uint8_t spiTransfer8(uint8_t spi_ch, uint8_t data)
{
  uint8_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  //if (p_spi->is_open == false) return 0;


  HAL_SPI_TransmitReceive(&p_spi->h_spi, &data, &ret, 1, 0xffff);

  return ret;
}


uint16_t spiTransfer16(uint8_t spi_ch, uint16_t data)
{
  uint8_t tBuf[2];
  uint8_t rBuf[2];
  uint16_t ret;
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return 0;


  tBuf[1] = (uint8_t)data;
  tBuf[0] = (uint8_t)(data>>8);
  HAL_SPI_TransmitReceive(&p_spi->h_spi, (uint8_t *)&tBuf, (uint8_t *)&rBuf, 2, 0xffff);

  ret = rBuf[0];
  ret <<= 8;
  ret += rBuf[1];

  return ret;
}


void spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.FirstBit = bitOrder;
  HAL_SPI_Init(&p_spi->h_spi);
}

void spiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;

  p_spi->h_spi.Init.BaudRatePrescaler = clockDiv;
  HAL_SPI_Init(&p_spi->h_spi);
}

void spiSetDataMode(uint8_t spi_ch, uint8_t dataMode)
{
  spi_t  *p_spi = &spi_tbl[spi_ch];


  if (p_spi->is_open == false) return;


  switch( dataMode )
  {
    // CPOL=0, CPHA=0
    case SPI_MODE0:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=0, CPHA=1
    case SPI_MODE1:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=0
    case SPI_MODE2:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_1EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;

    // CPOL=1, CPHA=1
    case SPI_MODE3:
      p_spi->h_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
      p_spi->h_spi.Init.CLKPhase    = SPI_PHASE_2EDGE;
      HAL_SPI_Init(&p_spi->h_spi);
      break;
  }
}







void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi)
{
  spi_t  *p_spi;
  GPIO_InitTypeDef  GPIO_InitStruct;




  if (hspi->Instance == spi_tbl[_HW_DEF_SPI1].h_spi.Instance)
  {
    p_spi = &spi_tbl[_HW_DEF_SPI1];

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable SPI clock */
    __HAL_RCC_SPI3_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* SPI SCK GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_3;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MISO GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_4;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* SPI MOSI GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_5;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}


void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi)
{
  spi_t  *p_spi;


  if (hspi->Instance == spi_tbl[_HW_DEF_SPI1].h_spi.Instance)
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_SPI3_FORCE_RESET();
    __HAL_RCC_SPI3_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Configure SPI SCK as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_3);
    /* Configure SPI MISO as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_4);
    /* Configure SPI MOSI as alternate function  */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5);
  }
}
