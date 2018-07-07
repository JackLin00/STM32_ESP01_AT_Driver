#include "./usart2/bsp_usart2.h"
#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>


//配置NVIC
static void NVIC_Config()
{
	 NVIC_InitTypeDef NVIC_InitStructure;
  
  /* ???????????? */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  /* ??USART???? */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  /* ??????1 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* ?????1 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* ???? */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  /* ?????NVIC */
  NVIC_Init(&NVIC_InitStructure);
}

//初始化USART2的GPIO属性
static void GPIO_Config()
{
	GPIO_InitTypeDef GPIO_InitTypeStruct;
	
	RCC_AHB1PeriphClockCmd(USART2_CLK,ENABLE);
	
	GPIO_InitTypeStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitTypeStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitTypeStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitTypeStruct.GPIO_Speed = GPIO_Fast_Speed;
	
	GPIO_InitTypeStruct.GPIO_Pin = USART2_TX_PIN;
	GPIO_Init(USART2_TX_PORT,&GPIO_InitTypeStruct);
	
	GPIO_InitTypeStruct.GPIO_Pin = USART2_RX_PIN;
	GPIO_Init(USART2_RX_PORT,&GPIO_InitTypeStruct);

	
	GPIO_PinAFConfig (GPIOA,GPIO_PinSource2,GPIO_AF_USART2);
	GPIO_PinAFConfig (GPIOA,GPIO_PinSource3,GPIO_AF_USART2);
	
}

//配置USART2
void USART2_Config()
{
	USART_InitTypeDef USART_InitTypeStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	
	GPIO_Config();
	
	USART_InitTypeStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitTypeStruct.USART_BaudRate = 115200;
	USART_InitTypeStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitTypeStruct.USART_Parity = USART_Parity_No;
	USART_InitTypeStruct.USART_StopBits = USART_StopBits_1 ;
	USART_InitTypeStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2,&USART_InitTypeStruct);
	NVIC_Config();
	
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

	USART_Cmd(USART2,ENABLE);
	
}
//USART2的发送字节数据函数
void USART2_SendByte(uint8_t data)
{
	USART_SendData(USART2,data);
	
	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
}
//USART2的发送不定长数据函数
void USART2_SendString(char *buffer)
{
	while(*(buffer) != '\0')
	{
		USART2_SendByte(*(buffer));
		buffer++;
	}
	
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}

void USART2_SendData(char *buffer,int len)
{
	while(len--)
	{
		USART2_SendByte(*(buffer));
		buffer++;
	}
	while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}







