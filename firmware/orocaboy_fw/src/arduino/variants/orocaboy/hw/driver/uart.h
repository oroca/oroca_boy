/*
 * uart.h
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */

#ifndef SRC_ARDUINO_VARIANTS_OROCABOY_HW_DRIVER_UART_H_
#define SRC_ARDUINO_VARIANTS_OROCABOY_HW_DRIVER_UART_H_

#include "hw_def.h"
#include "bsp.h"


#ifdef __cplusplus
 extern "C" {
#endif





#define UART_CH_MAX   1




typedef struct
{
  bool               is_open;
  UART_HandleTypeDef h_uart;
  DMA_HandleTypeDef  hdma_rx;
  ring_buf_t         rx_buf;
} uart_t;





bool uartInit(void);

bool     uartOpen(uint8_t uart_ch, uint32_t baud_rate, uint8_t config);
int32_t  uartWrite(uint8_t uart_ch, uint8_t *p_data, uint32_t length);
uint32_t uartAvailable(uint8_t uart_ch);
uint8_t  uartRead(uint8_t uart_ch);





#ifdef __cplusplus
 }
#endif


#endif /* SRC_ARDUINO_VARIANTS_OROCABOY_HW_DRIVER_UART_H_ */
