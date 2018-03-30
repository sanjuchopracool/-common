#include "serial.h"

#ifdef STM32F3DISCOVERY
#include <stm32f30x.h>
#include <stm32f30x_spi.h>
#define SERIAL_GPIO_CLOCK_ENABLE()         RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
#define SERIAL_CLK_ENABLE()                RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE)
#define SERIAL_AF                          GPIO_AF_7
#endif

#ifdef STM32F40_41xxx
#include <stm32f4xx.h>
#define SERIAL_GPIO_CLOCK_ENABLE()          RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE )
#define SERIAL_CLK_ENABLE()                RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE)
#define SERIAL_AF                          GPIO_AF_USART2
#endif

#ifdef STM32F429_439xx
#include <stm32f4xx.h>
#include <stm32f4xx_spi.h>
#define SERIAL_GPIO_CLOCK_ENABLE()          RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOA, ENABLE )
#define SERIAL_CLK_ENABLE()                 RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1, ENABLE )
#define SERIAL_AF                           GPIO_AF_USART1
#endif

void initSerial()
{
  USART_InitTypeDef USART_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  SERIAL_GPIO_CLOCK_ENABLE();
  SERIAL_CLK_ENABLE();

#ifdef STM32F3DISCOVERY
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, SERIAL_AF);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, SERIAL_AF);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9 | GPIO_Pin_10;
#endif

#ifdef NUCLEO401RE
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, SERIAL_AF);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, SERIAL_AF);
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2 | GPIO_Pin_3;
#endif

  /* Configure  pins:  Rx and Tx ----------------------------*/
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

#ifdef STM32F3DISCOVERY
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1,ENABLE);
#endif

#ifdef NUCLEO401RE
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2,ENABLE);
#endif

  init_printf(0, sendChar);
}


void sendChar(void *p, char ch)
{
#ifdef STM32F3DISCOVERY
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint16_t)ch);
#endif

#ifdef NUCLEO401RE
    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART_SendData(USART2, (uint16_t)ch);
#endif
}

char getChar()
{
#ifdef STM32F3DISCOVERY
    while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
    return (char)USART_ReceiveData(USART1);
#endif
}
