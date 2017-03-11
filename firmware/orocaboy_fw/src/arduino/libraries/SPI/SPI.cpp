/****************************************************************************
 *
 * SPI Master library for Arduino STM32 + HAL + CubeMX (HALMX).
 *
 * Copyright (c) 2016 by Vassilis Serasidis <info@serasidis.gr>
 * Home: http://www.serasidis.gr
 * email: avrsite@yahoo.gr
 *
 * Arduino_STM32 forum: http://www.stm32duino.com
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 *
 ****************************************************************************/


#include <SPI.h>
#include <chip.h>
#include "hw.h"


/* Create an SPIClass instance */
SPIClass SPI(_HW_DEF_SPI1);



void SPISettings::init_AlwaysInline(uint32_t clock, uint8_t bitOrder, uint8_t dataMode)
{
  if (clock >= 50000000 / 2) {
    clockDiv = SPI_CLOCK_DIV2;
  } else if (clock >= 50000000 / 4) {
    clockDiv = SPI_CLOCK_DIV4;
  } else if (clock >= 50000000 / 8) {
    clockDiv = SPI_CLOCK_DIV8;
  } else if (clock >= 50000000 / 16) {
    clockDiv = SPI_CLOCK_DIV16;
  } else if (clock >= 50000000 / 32) {
    clockDiv = SPI_CLOCK_DIV32;
  } else if (clock >= 50000000 / 64) {
    clockDiv = SPI_CLOCK_DIV64;
  } else {
    clockDiv = SPI_CLOCK_DIV64;
  }

  _bitOrder = bitOrder;
  _dataMode = dataMode;
}


SPIClass::SPIClass(uint8_t spi_ch) {
  spi_port = spi_ch;
}


void SPIClass::begin(void)
{
  spiBegin(spi_port);
}

void SPIClass::beginFast(void) {
  //drv_spi_enable_dma(_hspi);
  //init();
}


uint8_t SPIClass::transfer(uint8_t data) const{
  uint8_t ret;

  ret = spiTransfer8(spi_port, data);

	return ret;
}

uint16_t SPIClass::transfer16(uint16_t data) {
  uint16_t ret;


  ret = spiTransfer16(spi_port, data);

  return ret;
}

void SPIClass::transfer(void *buf, size_t count) {
  //HAL_SPI_Transmit(_hspi, (uint8_t *)buf, count, 0xffff);
  //HAL_SPI_TransmitReceive(_hspi, (uint8_t *)buf, (uint8_t *)buf, count, 0xffff);
  int i;
  uint8_t *pbuf = (uint8_t *)buf;

  for( i=0; i<count; i++ )
  {
    pbuf[i] = transfer(0xFF);
  }
}


void SPIClass::transferFast(void *buf, size_t count) {
  uint32_t t_time;

#if 0
  drv_spi_start_dma_tx(_hspi, (uint8_t *)buf, count);

  t_time = millis();

  while(1)
  {
    if(drv_spi_is_dma_tx_done(_hspi))
    {
      break;
    }
    if((millis()-t_time) > 1000)
    {
      break;
    }
  }
#endif
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
  uint8_t spi_bit_order = 0;

  if (bitOrder == 1)
    spi_bit_order = SPI_FIRSTBIT_MSB;
  else
    spi_bit_order = SPI_FIRSTBIT_LSB;

  spiSetBitOrder(spi_port, spi_bit_order);
}

void SPIClass::setClockDivider(uint8_t clockDiv) {

  uint8_t spi_clock_div = 0;


  switch(clockDiv){
    case SPI_CLOCK_DIV2:
      spi_clock_div = SPI_BAUDRATEPRESCALER_2;
    break;
    case SPI_CLOCK_DIV4:
      spi_clock_div = SPI_BAUDRATEPRESCALER_4;
    break;
    case SPI_CLOCK_DIV8:
      spi_clock_div = SPI_BAUDRATEPRESCALER_8;
    break;
    case SPI_CLOCK_DIV16:
      spi_clock_div = SPI_BAUDRATEPRESCALER_16;
    break;
    case SPI_CLOCK_DIV32:
      spi_clock_div = SPI_BAUDRATEPRESCALER_32;
    break;
    case SPI_CLOCK_DIV64:
      spi_clock_div = SPI_BAUDRATEPRESCALER_64;
    break;
    case SPI_CLOCK_DIV128:
      spi_clock_div = SPI_BAUDRATEPRESCALER_128;
    break;
    case SPI_CLOCK_DIV256:
      spi_clock_div = SPI_BAUDRATEPRESCALER_256;
    break;
  }
  spiSetClockDivider(spi_port, spi_clock_div);
}

void SPIClass::setDataMode(uint8_t dataMode){

  spiSetDataMode(spi_port, dataMode);
}
