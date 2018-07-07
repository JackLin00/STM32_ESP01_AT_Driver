#include "./debug/debug.h"
#include "stm32f4xx.h"



void Debug_Config(){
	
	
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	USART_InitTypeDef Debug_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_AHB1PeriphClockCmd(DEBUG_TX_CLK | DEBUG_RX_CLK,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitTypeStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Fast_Speed;
	
	GPIO_InitTypeStruct.GPIO_Pin = DEBUG_TX_PIN;
	GPIO_Init(DEBUG_TX_PORT,&GPIO_InitTypeStruct);
	
	
	GPIO_InitTypeStruct.GPIO_Pin = DEBUG_RX_PIN;
	GPIO_Init(DEBUG_TX_PORT,&GPIO_InitTypeStruct);
	
	GPIO_PinAFConfig(DEBUG_TX_PORT,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(DEBUG_RX_PORT,GPIO_PinSource10,GPIO_AF_USART1);
	
	
	Debug_InitStruct.USART_BaudRate = 115200;
	Debug_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	Debug_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	Debug_InitStruct.USART_Parity = USART_Parity_No;
	Debug_InitStruct.USART_StopBits = USART_StopBits_1;
	Debug_InitStruct.USART_WordLength = USART_WordLength_8b;
	
	USART_Init(DEBUG_UART1,&Debug_InitStruct);
	
	
	USART_Cmd(DEBUG_UART1,ENABLE);
}


int fputc(int ch, FILE *f)
{
		//发送数据到串口
		USART_SendData(DEBUG_UART1, (uint8_t) ch);
		
		//等待数据发送完毕
		while (USART_GetFlagStatus(DEBUG_UART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重定向后可使用scanf,getchar等函数
