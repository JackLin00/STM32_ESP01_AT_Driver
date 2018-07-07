#include "./systick/bsp_systick.h"
#include "./debug/debug.h"
#include "stm32f4xx.h"

static __IO uint32_t count;

void MySysTick_Config()
{
	if(SysTick_Config(SystemCoreClock / 1000))
	{
		Debug_Info("Systick Init is error......\r\n");
	}
	
}


void Delay_s(int number)
{
	count = number;
	while(count != 0);
}

void tida()
{
	count--;
}

