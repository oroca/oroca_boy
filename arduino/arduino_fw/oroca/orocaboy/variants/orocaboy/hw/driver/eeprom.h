/*
 *  eeprom.h
 *
 *  Created on: 2017. 3.17.
 *      Author: Baram
 */

#ifndef EEPROM_H_
#define EEPROM_H_


#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"
#include "bsp.h"








int eepromInit();

uint8_t  eepromReadByte(int addr);
void     eepromWriteByte(int index, uint8_t data_in);
uint16_t eepromGetLength(void);


#ifdef __cplusplus
}
#endif


#endif

