/*
 * uart.c
 *
 *  Created on: 2017. 3. 11.
 *      Author: Baram
 */
/*

 UART2
  TX PA2  AF7 DMA1 Channel7 Request2
  RX PA15 AF3 DMA1 Channel6 Request2


 */

#include "uart.h"



#define UART_RX_BUF_LENGTH    128



uart_t uart_tbl[UART_CH_MAX];



bool uartInit(void)
{
  uint8_t i;


  for(i=0; i<UART_CH_MAX; i++)
  {
    uart_tbl[i].is_open = false;

    uart_tbl[i].rx_buf.ptr_in  = 0;
    uart_tbl[i].rx_buf.ptr_out = 0;
    uart_tbl[i].rx_buf.p_buf   = NULL;
  }

  return true;
}


bool uartOpen(uint8_t uart_ch, uint32_t baud_rate, uint8_t config)
{
  uart_t  *p_uart;



  switch(uart_ch)
  {
    case _HW_DEF_UART1:
      p_uart = &uart_tbl[uart_ch];

      p_uart->rx_buf.ptr_in  = 0;
      p_uart->rx_buf.ptr_out = 0;
      if (p_uart->rx_buf.p_buf != NULL)
      {
        free(p_uart->rx_buf.p_buf);
      }
      p_uart->rx_buf.p_buf  = (uint8_t *)malloc(UART_RX_BUF_LENGTH);
      p_uart->rx_buf.length = UART_RX_BUF_LENGTH;
      if (p_uart->rx_buf.p_buf == NULL)
      {
        return false;
      }

      p_uart->h_uart.Instance        = USART2;

      p_uart->h_uart.Init.BaudRate   = baud_rate;
      p_uart->h_uart.Init.WordLength = UART_WORDLENGTH_8B;
      p_uart->h_uart.Init.StopBits   = UART_STOPBITS_1;
      p_uart->h_uart.Init.Parity     = UART_PARITY_NONE;
      p_uart->h_uart.Init.HwFlowCtl  = UART_HWCONTROL_NONE;
      p_uart->h_uart.Init.Mode       = UART_MODE_TX_RX;
      HAL_UART_DeInit(&p_uart->h_uart);
      HAL_UART_Init(&p_uart->h_uart);

      HAL_UART_Receive_DMA(&p_uart->h_uart, (uint8_t *)p_uart->rx_buf.p_buf, p_uart->rx_buf.length );

      p_uart->is_open = true;
      break;
  }


  return true;
}


uint32_t  uartAvailable(uint8_t uart_ch)
{
  uint32_t length = 0;
  uart_t  *p_uart = &uart_tbl[uart_ch];


  if (p_uart->is_open == false) return 0;


  p_uart->rx_buf.ptr_in = p_uart->rx_buf.length - p_uart->hdma_rx.Instance->CNDTR;

  length = (   p_uart->rx_buf.length
             + p_uart->rx_buf.ptr_in
             - p_uart->rx_buf.ptr_out ) % p_uart->rx_buf.length;

  return length;
}


uint8_t uartRead(uint8_t uart_ch)
{
  uint32_t length = 0;
  uart_t  *p_uart = &uart_tbl[uart_ch];
  uint8_t ret = 0;


  if (p_uart->is_open == false) return 0;



  ret = p_uart->rx_buf.p_buf[p_uart->rx_buf.ptr_out];

  p_uart->rx_buf.ptr_out = (p_uart->rx_buf.ptr_out + 1) % p_uart->rx_buf.length;

  return ret;
}


int32_t uartWrite(uint8_t uart_ch, uint8_t *p_data, uint32_t length)
{
  uart_t  *p_uart = &uart_tbl[uart_ch];


  if (p_uart->is_open == false) return 0;


  if (HAL_UART_Transmit(&p_uart->h_uart, (uint8_t*)p_data, length, 100) == HAL_OK)
  {
    return length;
  }


  return 0;
}







void USART2_IRQHandler(void)
{
  HAL_UART_IRQHandler(&uart_tbl[_HW_DEF_UART1].h_uart);
}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  uart_t  *p_uart;
  GPIO_InitTypeDef  GPIO_InitStruct;




  if (huart->Instance == uart_tbl[_HW_DEF_UART1].h_uart.Instance)
  {
    p_uart = &uart_tbl[_HW_DEF_UART1];


    __HAL_RCC_USART2_CLK_ENABLE();


    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_2;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = GPIO_PIN_15;
    GPIO_InitStruct.Alternate = GPIO_AF3_USART2;

    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);


    /*##-3- Configure the DMA ##################################################*/
    /* Configure the DMA handler for Transmission process */

    /* Enable DMA clock */
    __HAL_RCC_DMA1_CLK_ENABLE();


    /* Configure the DMA handler for reception process */
    p_uart->hdma_rx.Instance                 = DMA1_Channel6;
    p_uart->hdma_rx.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    p_uart->hdma_rx.Init.PeriphInc           = DMA_PINC_DISABLE;
    p_uart->hdma_rx.Init.MemInc              = DMA_MINC_ENABLE;
    p_uart->hdma_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    p_uart->hdma_rx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
    p_uart->hdma_rx.Init.Mode                = DMA_CIRCULAR;
    p_uart->hdma_rx.Init.Priority            = DMA_PRIORITY_HIGH;
    p_uart->hdma_rx.Init.Request             = DMA_REQUEST_2;

    HAL_DMA_Init(&p_uart->hdma_rx);

    /* Associate the initialized DMA handle to the the UART handle */
    __HAL_LINKDMA(huart, hdmarx, p_uart->hdma_rx);

    /*##-4- Configure the NVIC for DMA #########################################*/
    /* NVIC configuration for DMA transfer complete interrupt (USART1_RX) */
    //HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 10, 0);
    //HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

    /* NVIC for USART, to catch the TX complete */
    HAL_NVIC_SetPriority(USART2_IRQn, 10, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO, DMA and NVIC configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{

  if (huart->Instance == uart_tbl[_HW_DEF_UART1].h_uart.Instance)
  {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_USART2_FORCE_RESET();
    __HAL_RCC_USART2_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks #################################*/
    /* Configure USARTx Tx as alternate function  */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2);
    /* Configure USARTx Rx as alternate function  */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

    /*##-3- Disable the DMA #####################################################*/
    /* De-Initialize the DMA channel associated to reception process */
    if(huart->hdmarx != 0)
    {
      HAL_DMA_DeInit(huart->hdmarx);
    }
    /* De-Initialize the DMA channel associated to transmission process */
    if(huart->hdmatx != 0)
    {
      HAL_DMA_DeInit(huart->hdmatx);
    }

    HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);
  }
}
