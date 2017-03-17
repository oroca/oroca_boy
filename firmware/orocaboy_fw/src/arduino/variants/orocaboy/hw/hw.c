/*
 * hw.c
 *
 *  Created on: 2017. 3. 10.
 *      Author: Baram
 */



#include "hw.h"



static TIM_HandleTypeDef    TimHandle;



void hwMicrosInit(void);




void hwInit(void)
{
  hwMicrosInit();

  uartInit();
  spiInit();
  eepromInit();
}


void hwMicrosInit(void)
{
  uint32_t uwPrescalerValue = 0;


  __HAL_RCC_TIM2_CLK_ENABLE();


  // Compute the prescaler value to have TIMx counter clock equal to 1Mh
  uwPrescalerValue = (uint32_t)((SystemCoreClock / 1) / 1000000) - 1;


  TimHandle.Instance = TIM2;
  TimHandle.Init.Period            = 0xFFFFFFFF;
  TimHandle.Init.Prescaler         = uwPrescalerValue;
  TimHandle.Init.ClockDivision     = 0;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0;


  HAL_TIM_Base_Init(&TimHandle);
  HAL_TIM_Base_Start(&TimHandle);
}


uint32_t hwMicros()
{
  return TimHandle.Instance->CNT;
}
