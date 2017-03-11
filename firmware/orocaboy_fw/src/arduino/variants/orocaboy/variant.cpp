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
 /*
 *  Modified on: 2016. 7.12.
 *       Author: Baram, PBHP
 */
#include "variant.h"




#ifdef __cplusplus
extern "C" {
#endif




extern const Pin2PortMapArray g_Pin2PortMapArray[]=
{
    {GPIOA, GPIO_PIN_10,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 0  UART6_RX
    {GPIOA, GPIO_PIN_9,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 1  UART6_TX
    {GPIOA, GPIO_PIN_12,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 2                         EXTI_0
    {GPIOB, GPIO_PIN_0,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 3
    {GPIOB, GPIO_PIN_7,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 4
    {GPIOB, GPIO_PIN_6,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 5
    {GPIOB, GPIO_PIN_1,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 6
    {GPIOC, GPIO_PIN_14,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 7
    {GPIOC, GPIO_PIN_15,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 8
    {GPIOA, GPIO_PIN_8,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 9
    {GPIOA, GPIO_PIN_11,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 10
    {GPIOB, GPIO_PIN_5,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 11
    {GPIOB, GPIO_PIN_4,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 12
    {GPIOB, GPIO_PIN_3,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 13

    {GPIOA, GPIO_PIN_0,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 14 A0
    {GPIOA, GPIO_PIN_1,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 15 A1
    {GPIOA, GPIO_PIN_3,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 16 A2
    {GPIOA, GPIO_PIN_4,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 17 A3
    {GPIOA, GPIO_PIN_5,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 18 A4
    {GPIOA, GPIO_PIN_6,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 19 A5
    {GPIOA, GPIO_PIN_7,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 20 A6
    {GPIOA, GPIO_PIN_2,   NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI },  // 21 A7


    {NULL , 0          ,  NULL,     NO_ADC        , NULL   ,   NO_PWM       , NO_EXTI }
};


#ifdef __cplusplus
}
#endif




/* ----------------------------------------------------------------------------
 *     USART objects
 * ----------------------------------------------------------------------------*/
void serialEvent() __attribute__((weak));
void serialEvent() { }

void serialEvent1() __attribute__((weak));
void serialEvent1() { }

void serialEvent2() __attribute__((weak));
void serialEvent2() { }

void serialEvent3() __attribute__((weak));
void serialEvent3() { }

UARTClass Serial(_HW_DEF_UART1);
UARTClass Serial1(_HW_DEF_UART2);

//UARTClass Serial2(&huart2, USART2_IRQn, 1, USART2);
//UARTClass Serial3(&huart3, USART3_IRQn, 2, USART3);




void serialEventRun(void)
{
  if (Serial.available()) serialEvent();
  if (Serial1.available()) serialEvent1();
}



