#ifndef __BSP_USART2_H__
#define __BSP_USART2_H__

#include "stm32f4xx.h"
#include <stdio.h>


#define USART2_TX_PORT     GPIOA
#define USART2_TX_PIN      GPIO_Pin_2
#define USART2_CLK         RCC_AHB1Periph_GPIOA


#define USART2_RX_PORT     GPIOA
#define USART2_RX_PIN	     GPIO_Pin_3





////AT√¸¡Ó
//const char RESPONSE_OK[] = "OK\r\n";
//const char ESP8266_TEST[] = "";	// Test AT startup


void USART2_Config(void);
void USART2_SendString(char *buffer);
void USART2_SendData(char *buffer,int len);
void GetEspData(char *RecvBuffer);













#endif

