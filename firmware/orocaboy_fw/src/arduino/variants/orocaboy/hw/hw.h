/*
 * hw.h
 *
 *  Created on: 2017. 3. 10.
 *      Author: Baram
 */

#ifndef SRC_HW_HW_H_
#define SRC_HW_HW_H_


#include "hw_def.h"

#include "driver/uart.h"
#include "driver/spi.h"

#ifdef __cplusplus
 extern "C" {
#endif



void hwInit(void);


#ifdef __cplusplus
 }
#endif

#endif /* SRC_HW_HW_H_ */
