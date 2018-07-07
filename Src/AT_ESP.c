#include "AT_ESP.h"
#include <string.h>
#include <stdio.h>




char ESPBuffer[128];
__IO uint16_t ESPRxIndex = 0x00;
const char ResponseOK[] = "OK\r\n";
const char SendResponse[] = "OK\r\n> ";


#define ESP_UART_CONFIG        //你实现的和ESP01对接的串口初始化函数
#define SendMessageToESP       //你实现的基于字符串的串口发送函数
#define SendMessageToESP2      //你实现的基于发送长度的串口发送函数
#define Delay_ms               //你实现的毫秒级的延时函数 

/*
因为我测试是用USART2作为和ESP01对接的串口，所以以下都是用USART2，请根据自己的情况自行修改。
*/

//用中断接收ESP的反馈信息
void USART2_IRQHandler(void)
{
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) == SET)
	{
		USART_ReceiveData( USART2 );
		USART_ClearITPendingBit(USART2,USART_IT_ORE);
	}
	if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
	{
		if(ESPRxIndex < 128)
		{
			ESPBuffer[ESPRxIndex++] = USART_ReceiveData(USART2);
		}
		else
		{
			USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
		}
	}
}       
/*
-------------------------------------------------------------------------------
下面的是一些基础函数
-------------------------------------------------------------------------------
*/

static void delay()
{
	int i,j;
	i = 1000;
	while(i--)
	{
		j = 350;
		while(j--);
	}
}

static void ClearESPBuffer()
{
	ESPRxIndex = 0x00;
	memset(ESPBuffer, '\0', ESP8266_RX_BUFFER_LEN);
}


static void SendEspCommand(char *order)
{
	ClearESPBuffer();
	SendMessageToESP(order);
}

static void SendEspCommand2(char *data,int len)
{
	ClearESPBuffer();
	SendMessageToESP2(data,len);
}


static char CheckResponse()
{
	if(strstr(ESPBuffer,ResponseOK) == NULL)
	{
		return 0;
	}
	return 1;
}

/*
-------------------------------------------------------------------------------
基础函数结束
-------------------------------------------------------------------------------
*/

/*
-------------------------------------------------------------------------------
接下来是ESP01的API
-------------------------------------------------------------------------------
*/


/*
函数说明：检测单片机和ESP-01是否正确通信
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 CheckEsp(void)
{
	SendEspCommand("AT\r\n");
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}

/*
函数说明：设置ESP-01的当前的WIFI模式
参数说明：mode是想要设置的模式，具体模式详见AT指令手册
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 SetEspMode(char mode)
{
	char tmp[3];
	tmp[0] = mode;
	tmp[1] = '\r';
	tmp[2] = '\n';
	SendEspCommand("AT+CWMODE_CUR=");
	SendEspCommand(tmp);
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}

/*
函数说明：让ESP-01连接指定WIFI
参数说明：SSID:欲要连接的WIFI名，Passwd:对应的WIFI密码
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 ConnectWiFi(char *SSID,char *Passwd)
{
	SendEspCommand("AT+CWJAP=\"");
	SendEspCommand(SSID);
	SendEspCommand("\",\"");
	SendEspCommand(Passwd);
	SendEspCommand("\"\r\n");
	Delay_ms(6500);
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}
/*
函数说明：获取ESP-01的当前的IPaddress，注意必须让ESP-01连接WIFI后使用
参数说明：RecvBuffer:接收返回的IPaddress的Buffer
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 GetIpAddress(char *RecvBuffer)
{
	char i = 0;
	char *p;
	SendEspCommand("AT+CIFSR\r\n");
	delay();
	if(CheckResponse())
	{
		p = strtok(ESPBuffer,"\"");
		p = strtok(NULL,"\"");
		while(*p)
		{
			RecvBuffer[i] = *p;
			p++;
			i++;
		}
		return 1;
	}
	return 0;
}

/*
函数说明：获取ESP-01的MACaddress
参数说明：RecvBuffer:接收返回的MACaddress的Buffer
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 GetMACaddress(char *Recvbuffer)
{
	char i = 0;
	char *p;
	SendEspCommand("AT+CIFSR\r\n");
	delay();
	if(CheckResponse())
	{
		p = strtok(ESPBuffer,"\"");
		p = strtok(NULL,"\"");
		p = strtok(NULL,"\"");
		p = strtok(NULL,"\"");
		while(*p)
		{
			Recvbuffer[i] = *p;
			p++;
			i++;
		}
		return 1;
	}
	return 0;
}

/*
函数说明：断开ESP-01的连接的WIFI
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 DisconnectWiFi()
{
	SendEspCommand("AT+CWQAP\r\n");
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}
/*
函数说明：连接指定的TCPServer或者UDPServer
参数说明：IPAddress：TCPServer或者UDPServer的IPAddress，port：对应服务器的端口
					mode:连接Server的类型，0为TCP,1为UDP
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 ConnectServer(char *IPAddress,char *port,char mode)
{
	if(mode == 0)
	{
		SendEspCommand("AT+CIPSTART=\"TCP\",\"");
		SendEspCommand(IPAddress);
		SendEspCommand("\",");
		SendEspCommand(port);
		SendEspCommand("\r\n");
		Delay_ms(1000);
		if(CheckResponse())
		{
			return 1;
		}
		return 0;
	}
	else if(mode == 1)
	{
		SendEspCommand("AT+CIPSTART=\"UDP\",\"");
		SendEspCommand(IPAddress);
		SendEspCommand("\",");
		SendEspCommand(port);
		SendEspCommand("\r\n");
		Delay_ms(1000);
		if(CheckResponse())
		{
			return 1;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

/*
函数说明：关闭已连接的TCPServer or UDPServer
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
u8 CloseTCPOrUDPConnect(void)
{
	SendEspCommand("AT+CIPCLOSE\r\n");
	Delay_ms(1000);
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}

/*
函数说明：向已经连接的Sever发送字符串数据
参数说明：Data:要发送的字符串信息指针
返回值说明:函数返回值为1是success,0为fail
注意事项：这里的Server可以是TCPServer或者UDPServer，视乎于你之前连接的Server
					即如果你在使用这个函数之前使用的是TCPServer，那么该函数就向你所连接的
					TCPServer发送字符串数据；当是UDPServer，也是如此。
已测试
*/
u8 SendStringDataToServer(char *Data)
{
	char params[8];
	int len;
	len = strlen(Data);
	sprintf(params,"%d",len);
	SendEspCommand("AT+CIPSEND=");
	SendEspCommand(params);
	SendEspCommand("\r\n");
	delay();
	if(CheckResponse())
	{
		SendEspCommand(Data);
		delay();
		delay();
		return 1;
	}
	return 0;
}

/*
函数说明：向已经连接的Sever发送非字符串信息(发送例如结构体等内存对象信息)
参数说明：Data:要发送数据的头指针，len：发送数据长度
返回值说明:函数返回值为1是success,0为fail
注意事项：这里的Server可以是TCPServer或者UDPServer，视乎于你之前连接的Server
					即如果你在使用这个函数之前使用的是TCPServer，那么该函数就向你所连接的
					TCPServer发送字符串数据；当是UDPServer，也是如此。
已测试
*/
u8 SendDataToServer(char *Data,int len)
{
	char params[8];
	sprintf(params,"%d",len);
	SendEspCommand("AT+CIPSEND=");
	SendEspCommand(params);
	SendEspCommand("\r\n");
	delay();
	if(CheckResponse())
	{
		SendEspCommand2(Data,len);
		delay();
		delay();
		return 1;
	}
	return 0;
}

/*
函数说明：让ESP01进入modem_sleep状态
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：void
待测试
*/
u8 ModemSleep_Getinto(void)
{
	SendEspCommand("AT+SLEEP=2\r\n");
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}

/*
函数说明：让ESP01退出modem_sleep状态
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：void
待测试
*/
u8 ModemSleep_Quit(void)
{
	SendEspCommand("AT+SLEEP=0\r\n");
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}

/*
函数说明：让ESP01进入Deep_Sleep状态
参数说明：void
返回值说明:函数返回值为1是success,0为fail
注意事项：该模式下只能通过硬件唤醒，详细说明查看官方AT指令
待测试
*/
u8 DeepSleep_Getinto(void)
{
	SendEspCommand("AT+GSLP=0\r\n");   //等于0的意义是ESP除了硬件唤醒，其他不会唤醒.
	delay();
	if(CheckResponse())
	{
		return 1;
	}
	return 0;
}



/*
函数说明：esp8266结构体的初始化和对应UART的初始化
参数说明：IPAddress：TCPServer的IPAddress，port：对应服务器的端口
返回值说明:函数返回值为1是success,0为fail
注意事项：void
已测试
*/
void esp8266Init(esp8266 *handle)
{
	ESP_UART_CONFIG();                  //初始化ESP-01对应的串口
	handle->CheckESP = CheckEsp;        //这些是让函数指针对应相应的函数
	handle->SetEspMode = SetEspMode;
	handle->ConnectWiFi = ConnectWiFi;
	handle->DisconnectWiFi = DisconnectWiFi;
	handle->GetIpAddress = GetIpAddress;
	handle->GetMACaddress = GetMACaddress;
	handle->ConnectServer = ConnectServer;
	handle->CloseTCPOrUDPConnect = CloseTCPOrUDPConnect;
	handle->SendStringDataToServer = SendStringDataToServer;
	handle->SendDataToServer = SendDataToServer;
	handle->ModemSleep_Getinto = ModemSleep_Getinto;
	handle->ModemSleep_Quit = ModemSleep_Quit;
	handle->DeepSleep_Getinto = DeepSleep_Getinto;
}
