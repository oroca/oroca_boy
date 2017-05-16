/*
  Copyright (c) 2011 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hw.h"
#include "UARTClass.h"


// Constructors ////////////////////////////////////////////////////////////////
UARTClass::UARTClass(void){

}

UARTClass::UARTClass( uint8_t uart_ch )
{
  uart_number = uart_ch;
}

void UARTClass::begin(const uint32_t baud_rate)
{
  begin(baud_rate, SERIAL_8N1);
}

void UARTClass::begin(const uint32_t baud_rate, uint8_t config)
{
  uartOpen(uart_number, baud_rate, config);
} 

void UARTClass::end( void )
{
  flush();
}

int UARTClass::available( void )
{
  return uartAvailable(uart_number);
}

int UARTClass::availableForWrite(void)
{
  return 0;
}

int UARTClass::peek( void )
{
}

int UARTClass::read( void )
{
  return uartRead(uart_number);
}

void UARTClass::flush( void )
{
}

size_t UARTClass::write( const uint8_t uc_data )
{
  uartWrite(uart_number, (uint8_t *)&uc_data, 1);
  return 1;
}


