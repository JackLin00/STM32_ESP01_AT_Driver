#ifndef __BEBUG_H__
#define __BEBUG_H__


#include <stdio.h>
#include "stm32f4xx.h"


#define __DEBUG__

#ifdef __DEBUG__
#define Debug_Info(fmt,arg...)         printf(fmt,##arg)
#else
#define Debug_Info(fmt,arg...)       
#endif




#define DEBUG_UART1   USART1
#define DEBUG_TX_PORT GPIOA
#define DEBUG_TX_PIN  GPIO_Pin_9
#define DEBUG_TX_CLK  RCC_AHB1Periph_GPIOA

#define DEBUG_RX_PORT GPIOA
#define DEBUG_RX_PIN  GPIO_Pin_10
#define DEBUG_RX_CLK  RCC_AHB1Periph_GPIOA




void USART_SendString(USART_TypeDef* USARTx, char *str);
void Debug_Config(void);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);






#endif
