# STM32_ESP01_AT_Driver



这是一个STM32的ESP-01驱动库，由于本人在这段时间对OOP十分感兴趣，所以本驱动将以我的蹩脚OOP写法来写这个ESP-01驱动库。请不要吐槽我的C语言OOP写法，我知道很烂，很不符合规范，但这个是我的第一个尝试，一个按照我心中的OOP想法。

首先来介绍一下该仓库的结构：      
Examlpe:里面有Template文件夹.Template里面包含一个完整的移植实例(基于STM32F4)。     
Src:包含AT_ESP.c和AT_ESP.h，这是这个驱动库文件。   

硬件连接：      
       ESP01的3V3,GND连上单片机的3V3和GND。ESP01的TX,RX和单片机的TX,RX反接，ESP01的EN引脚接上单片机的3V3。

我采用的是STM32的STD库来写程序，可能会让大部分用HAL的同学感到不友好。但是当你花几分钟来了解这个驱动库的原理后，你会发现只要移植一下几个关键函数，你就可以用HAL库来运行这个ESP-01驱动。

该驱动的总体思路：
    利用STM32某一个UART和ESP01进行通信，该串口的的参数是波特率115200，8位数据位，1位停止位，无校验，无硬件控制。利用串口的中断来进行接收ESP01返回给单片机的信息判断我们想要执行的指令是否正确运行。其他都是基本的字符串常规操作。


这里以STD库函数的用户举例：
    对于STD库函数的用户来说：仅仅需要实现：      
    1.和ESP01对接的串口初始化函数，记得必须开USART_IT_RXNE中断，串口初始化参数上面有说。       
    2.基于字符串的串口发送函数，用于发送指令给ESP01。     
    3.基于发送长度的串口发送函数，用于发送特殊指令给ESP01.如果你是单单是发送字符串指令这个函数你可以不实现。      
    4.毫秒级的延时函数。一般用SYSTICK来实现。     
    
  
    打开AT_ESP.c，在以下宏定义填写上述所需要的函数：
    #define ESP_UART_CONFIG        //和ESP01对接的串口初始化函数
    #define SendMessageToESP       //基于字符串的串口发送函数
    #define SendMessageToESP2      //基于发送长度的串口发送函数
    #define Delay_ms               //毫秒级的延时函数
    
    可能有人会对要求2，3的所说的东西感到疑惑。
    在这里，我举一个例子：
    首先是一个串口字节发送函数
    void USART2_SendByte(uint8_t data)
    {
    	USART_SendData(USART2,data);

    	while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
    }
    基于字符串的串口发送函数和基于发送长度的串口发送函数都需要这个串口字节发送函数.
    因为我的ESP01模块对接我的UART2，所以填写UART2。当你们使用的时候，只需填回对应的串口即可。
    基于字符串的串口发送函数：
    void USART2_SendString(char *buffer)
    {
         while(*(buffer) != '\0')
	 {
	    USART2_SendByte(*(buffer));
	    buffer++;
         }
	
	 while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
    }
    基于发送长度的串口发送函数：
    void USART2_SendData(char *buffer,int len)
    {
        while(len--)
        {
            USART2_SendByte(*(buffer));
            buffer++;
        }
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
    }
    
   或许你会感到疑惑为什么需要这2个几乎一样的函数，这是因为当你用ESP01向服务器发送结构体等内存对象的信息时，这些内存对象往往会存在0X00数据，如果你采用字符串发送的形式就会无法发送。或许你也会问，为什么不直接用基于发送长度的串口发送函数?这是因为基于字符串的串口发送函数存在大多数应用场景，如果全部采用USART?_SendData会十分影响代码的美观性，当然这也是个人认为。各位客官可以根据自己的需要来更改。       
   
   接下来介绍用于接收ESP01反馈信息的中断接收函数，它仅仅是用常规中断的函数。
   
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
     
     
   有一个缓冲区用来接收ESP01的反馈信息，我们在每次发完指令给ESP01后都会根据AT反馈的信息来判断我们的指令是否正确执行。具体可以看看CheckResponse()这个函数。而我们每次发送指令给ESP01之前都会清空一下这个缓冲区。
   
   
用法（在实现该库所需要的函数后）：
    
    第一步：新建一个结构体esp8266，调用esp8266Init()函数初始化这个结构体,并检查单片机能否正确和ESP01通信
    '''
    你别的需要初始化函数....
    '''
    esp8266 test;
    esp8266Init(&test);
    if(!test.CheckESP())
    {
    	printf("无法和ESP01正确通信........\r\n");
    }
    
    第二步：Do what you want..._-_
    例如，设置ESP01的WIFI模式，连接WIFI，连接TCPServer,向TCPServer发送字符串数据
    test.SetEspMode(1);      //设置ESP01的WIFI模式为Station模式
    test.ConnectWiFi(SSID,PassWD);   //连接指定WIFI
    test.ConnectServer(IPaddress,Port,0);     //连接TCPServer
    test.SendStringDataToServer("I AM JACK");   //向TCPServer发送"I AM JACK"字符串
    
    更多用法，请查看源码注释
    
    
    
    
该驱动库要求使用者遵守MIT协议。
    
    
    
    
