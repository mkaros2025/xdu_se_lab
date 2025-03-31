/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"  // 包含STM32F10x系列微控制器的标准外设库头文件
#include <stdio.h>      // 包含标准输入输出库，用于printf函数

// 重定义fputc函数，用于通过USART1发送字符
int fputc(int ch, FILE *f)
{    
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等待USART1发送完成
	USART_SendData(USART1, (uint8_t)ch);  // 发送字符
	return ch;
}

// 配置TIM2定时器
void TIM2_Config(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定义定时器初始化结构体
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 使能TIM2时钟（第43页）

	// 配置TIM2定时器
	// 分频系数为 7199，于是计数频率 = 72MHz / (7199 + 1) = 10,000 Hz
	// 定时周期 = (9999 + 1) / 10,000 Hz = 1 s
	TIM_TimeBaseStructure.TIM_Period = 9999;  // 设置自动重装载值（第43页）
	TIM_TimeBaseStructure.TIM_Prescaler = 7199;  // 设置预分频器（第43页）
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 设置时钟分频（第43页）
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 设置计数模式为向上计数（第43页）
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  // 初始化TIM2（第43页）

	TIM_Cmd(TIM2, ENABLE);  // 使能TIM2（第43页）
}

int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体
	USART_InitTypeDef USART_InitStructure;  // 定义USART初始化结构体
 	
	// 使能GPIOA、GPIOB、GPIOC和USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // 使能GPIOC时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  // 使能USART1时钟

	// 配置PA9为复用推挽输出（USART1_TX）
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  // 选择PA9引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  // 设置GPIO速度为10MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 设置为复用推挽输出模式
	GPIO_Init(GPIOA, &GPIO_InitStructure);  // 初始化GPIOA
	
	// 配置USART1
	USART_InitStructure.USART_BaudRate = 115200;  // 设置波特率为115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 设置数据位为8位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 设置停止位为1位
	USART_InitStructure.USART_Parity = USART_Parity_No;  // 无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控制
	USART_InitStructure.USART_Mode = USART_Mode_Tx;  // 设置为发送模式
	USART_Init(USART1, &USART_InitStructure);  // 初始化USART1
	
	USART_Cmd(USART1, ENABLE);  // 使能USART1
	printf("USART1 Init Completely\r\n");  // 通过USART1发送初始化完成信息

	// 配置PC13和PB2为推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  // 选择PC13引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  // 设置GPIO速度为10MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置为推挽输出模式
	GPIO_Init(GPIOC, &GPIO_InitStructure);  // 初始化GPIOC

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // 选择PB2引脚
	GPIO_Init(GPIOB, &GPIO_InitStructure);  // 初始化GPIOB

	TIM2_Config();  // 配置TIM2定时器

	// 主循环
	while (1)
	{
		// 检查TIM2更新标志位
		if (TIM_GetFlagStatus(TIM2, TIM_FLAG_Update) == SET)  // 检查TIM2更新标志位（第43页）
		{
			TIM_ClearFlag(TIM2, TIM_FLAG_Update);  // 清除TIM2更新标志位（第43页）

			// 切换PC13和PB2的状态
			if (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET)  // 读取PC13状态
			{
				GPIO_SetBits(GPIOC, GPIO_Pin_13);  // 设置PC13为高电平，关灯
				GPIO_SetBits(GPIOB, GPIO_Pin_2);  // 设置PB2为高电平
			}
			else
			{
				GPIO_ResetBits(GPIOC, GPIO_Pin_13);  // 设置PC13为低电平，开灯
				GPIO_ResetBits(GPIOB, GPIO_Pin_2);  // 设置PB2为低电平
			}
		}
	}
}