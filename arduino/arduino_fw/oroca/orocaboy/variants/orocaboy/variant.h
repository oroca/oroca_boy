 /*
 *  Modified on: 2016. 7.12.
 *       Author: Baram
 */
#ifndef _VARIANT_OROCABOY_
#define _VARIANT_OROCABOY_

#include <chip.h>



#define NO_ADC 		0xffff
#define NO_PWM		0xffff
#define NO_EXTI   0xffff


/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "Arduino.h"
#ifdef __cplusplus
#include "UARTClass.h"
#endif


#ifdef __cplusplus
extern "C"{
#endif // __cplusplus






#define millis(a1)  HAL_GetTick(a1)
#define delay(a2)   HAL_Delay(a2)



#define USE_SPI1
#define USE_SPI2


#define BOARD_NR_I2C  2
#define HAL_I2C1      I2C1
#define HAL_I2C2      I2C2
#define HAL_I2C3      I2C3


#define BOARD_NR_ADC_PINS       5
#define BOARD_NR_PWM_PINS       12

#define LED_BUILTIN             13

#define BDPIN_LED_USER_1        22
#define BDPIN_LED_USER_2        23
#define BDPIN_LED_USER_3        24
#define BDPIN_LED_USER_4        25
#define BDPIN_DIP_SW_1          26
#define BDPIN_DIP_SW_2          27
#define BDPIN_SPI_CS_IMU        28
#define BDPIN_BAT_PWR_ADC       29
#define BDPIN_CHG_PWR_ADC       30
#define BDPIN_BUZZER            31
#define BDPIN_DXL_PWR_EN        32
#define BDPIN_LED_BATT_CHARGE   33
#define BDPIN_PUSH_SW_1         34
#define BDPIN_PUSH_SW_2         35
#define BDPIN_LED_STATUS        36


/*
 * Analog pins
 */
static const uint8_t A0  = 14;
static const uint8_t A1  = 15;
static const uint8_t A2  = 16;
static const uint8_t A3  = 17;
static const uint8_t A4  = 18;
static const uint8_t A5  = 19;
static const uint8_t A6  = 20;
static const uint8_t A7  = 21;
static const uint8_t BAT = 29;




typedef struct _Pin2PortMapArray
{
  	GPIO_TypeDef *GPIOx_Port;

  	uint32_t 	Pin_abstraction;

  	ADC_HandleTypeDef *ADCx;
    uint32_t  adc_channel;

    TIM_HandleTypeDef *TIMx;
    uint32_t  timerChannel;
    uint32_t  extiChannel;
} Pin2PortMapArray ;


extern const Pin2PortMapArray g_Pin2PortMapArray[] ;



#ifdef __cplusplus
}
#endif


/*----------------------------------------------------------------------------
 *        Arduino objects - C++ only
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus



extern UARTClass Serial;
extern UARTClass Serial1;




#endif


#define digitalPinToInterrupt(P)   ( g_Pin2PortMapArray[P].extiChannel )


#define WIRE_INTERFACES_COUNT       1
#define SPI_INTERFACES_COUNT        2
#define EXTI_COUNT                  7
#define PINS_COUNT                  68

#endif
