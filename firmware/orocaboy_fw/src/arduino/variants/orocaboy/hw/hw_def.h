/*
 * hw.h
 *
 *  Created on: 2017. 3. 10.
 *      Author: Baram
 */

#ifndef HW_DEF_H_
#define HW_DEF_H_


#include <stdint.h>
#include <stdlib.h>


typedef struct
{
  uint32_t ptr_in;
  uint32_t ptr_out;
  uint32_t length;
  uint8_t  *p_buf;
} ring_buf_t;


#define _HW_DEF_UART1     0
#define _HW_DEF_UART2     1
#define _HW_DEF_UART3     2
#define _HW_DEF_UART4     3
#define _HW_DEF_UART5     4


#define _HW_DEF_SPI1      0
#define _HW_DEF_SPI2      1



#define _HW_DEF_INPUT           0
#define _HW_DEF_INPUT_PULLUP    1
#define _HW_DEF_INPUT_PULLDOWN  2
#define _HW_DEF_OUTPUT          3
#define _HW_DEF_INPUT_ANALOG    4



#endif /* SRC_HW_HW_H_ */
