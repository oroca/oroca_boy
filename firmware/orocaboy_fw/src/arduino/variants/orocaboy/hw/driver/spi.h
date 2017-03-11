/*
 * uart.h
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */

#ifndef SPI_H_
#define SPI_H_

#include "hw_def.h"
#include "bsp.h"


#ifdef __cplusplus
 extern "C" {
#endif




#define SPI_CH_MAX   1







typedef struct
{
  bool               is_open;
  SPI_HandleTypeDef  h_spi;
} spi_t;





bool spiInit(void);

void     spiBegin(uint8_t spi_ch);
void     spiTransfer(uint8_t spi_ch, uint8_t *p_tx_data, uint8_t *p_rx_data, uint32_t length);
uint8_t  spiTransfer8(uint8_t spi_ch, uint8_t data);
uint16_t spiTransfer16(uint8_t spi_ch, uint16_t data);

void spiSetBitOrder(uint8_t spi_ch, uint8_t bitOrder);
void spiSetClockDivider(uint8_t spi_ch, uint8_t clockDiv);
void spiSetDataMode(uint8_t spi_ch, uint8_t dataMode);


#ifdef __cplusplus
 }
#endif


#endif /* SRC_ARDUINO_VARIANTS_OROCABOY_HW_DRIVER_UART_H_ */
