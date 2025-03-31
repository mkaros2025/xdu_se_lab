#include "stm32f10x.h"  // 包含STM32F10x系列微控制器的头文件
#include <stdio.h>      // 包含标准输入输出库，用于printf函数

// GPIO配置函数
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体

    // 使能GPIOC、GPIOB、GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);  // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  // 使能GPIOB时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  // 使能GPIOA时钟

    // 配置PC13为输出模式（LED）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  // 选择PC13引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置为推挽输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 设置输出速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);  // 初始化GPIOC的PC13引脚

    // 配置PB2为输出模式（LED控制）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;  // 选择PB2引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 设置为推挽输出模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);  // 初始化GPIOB的PB2引脚

    // 配置PA8（KEY0）为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;  // 选择PA8引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 设置为上拉输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);  // 初始化GPIOA的PA8引脚

    // 配置PB10（KEY1）为上拉输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  // 选择PB10引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 设置为上拉输入模式
    GPIO_Init(GPIOB, &GPIO_InitStructure);  // 初始化GPIOB的PB10引脚
}

// EXTI外部中断配置函数
void EXTI_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;  // 定义EXTI初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;  // 定义NVIC初始化结构体

    // 配置PA8（KEY0）为外部中断线8
    // （《第8讲 中断.pdf》第47页）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);  // 将PA8映射到EXTI线8
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;  // 选择EXTI线8
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  // 设置为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 设置为下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;  // 使能EXTI线8
    EXTI_Init(&EXTI_InitStructure);  // 初始化EXTI线8

    // 配置EXTI9_5中断通道
    // （《第8讲 中断.pdf》第35页）
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;  // 选择EXTI9_5中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 设置抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // 设置子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能EXTI9_5中断通道
    NVIC_Init(&NVIC_InitStructure);  // 初始化NVIC中断

    // 配置PB10（KEY1）为外部中断线10
    // （《第8讲 中断.pdf》第47页）
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);  // 将PB10映射到EXTI线10
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;  // 选择EXTI线10
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;  // 设置为中断模式
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  // 设置为下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;  // 使能EXTI线10
    EXTI_Init(&EXTI_InitStructure);  // 初始化EXTI线10

    // 配置EXTI15_10中断通道
    // （《第8讲 中断.pdf》第35页）
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;  // 选择EXTI15_10中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  // 设置抢占优先级为1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // 设置子优先级为0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能EXTI15_10中断通道
    NVIC_Init(&NVIC_InitStructure);  // 初始化NVIC中断
}

// TIM2定时器配置函数
void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定义TIM2初始化结构体
    NVIC_InitTypeDef NVIC_InitStructure;  // 定义NVIC初始化结构体

    // （《第7讲 定时器.pdf》第43页）
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  // 使能TIM2时钟

    // 配置TIM2定时器
    // （《第7讲 定时器.pdf》第43页）
    TIM_TimeBaseStructure.TIM_Period = 9999;  // 设置自动重装载寄存器值
    TIM_TimeBaseStructure.TIM_Prescaler = 7199;  // 设置预分频器值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 设置时钟分频
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 设置为向上计数模式
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);  // 初始化TIM2定时器

    // （《第7讲 定时器.pdf》第43页）
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  // 使能TIM2更新中断

    // 配置TIM2中断通道
    // （《第8讲 中断.pdf》第35页）
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  // 选择TIM2中断通道
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 设置抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // 设置子优先级为1
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能TIM2中断通道
    NVIC_Init(&NVIC_InitStructure);  // 初始化NVIC中断

    // （《第7讲 定时器.pdf》第43页）
    TIM_Cmd(TIM2, ENABLE);  // 使能TIM2定时器
}

// TIM2中断服务函数
void TIM2_IRQHandler(void)
{
    // （《第7讲 定时器.pdf》第43页）
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  // 检查TIM2更新中断标志位
    {
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  // 清除TIM2更新中断标志位
        // 输出数据寄存器（Output Data Register）
        GPIOC->ODR ^= GPIO_Pin_13;  // 翻转PC13引脚状态（LED闪烁）
    }
}

// EXTI9_5中断服务函数
// EXTI5~EXTI6共享中断服务程序EXTI9_5_IRQHandler
void EXTI9_5_IRQHandler(void)
{
    // （《第8讲 中断.pdf》第55页）
    if (EXTI_GetITStatus(EXTI_Line8) != RESET)  // 检查EXTI线8中断标志位
    {
        EXTI_ClearITPendingBit(EXTI_Line8);  // 清除EXTI线8中断标志位
        GPIOB->ODR &= ~GPIO_Pin_2;  // 将PB2引脚置低（关闭LED）
    }
}

// EXTI15_10中断服务函数
// EXTI10~EXTI15共享中断服务程序EXTI15_10_IRQHandler
void EXTI15_10_IRQHandler(void)
{
    // （《第8讲 中断.pdf》第55页）
    if (EXTI_GetITStatus(EXTI_Line10) != RESET)  // 检查EXTI线10中断标志位
    {
        EXTI_ClearITPendingBit(EXTI_Line10);  // 清除EXTI线10中断标志位
        GPIOB->ODR |= GPIO_Pin_2;  // 将PB2引脚置高（打开LED）
    }
}

// 重定向printf函数到USART1
int fputc(int ch, FILE *f)
{
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等待USART1发送完成
    USART_SendData(USART1, (uint8_t)ch);  // 发送数据
    return ch;
}

// 主函数
int main(void)
{
    USART_InitTypeDef USART_InitStructure;  // 定义USART初始化结构体

    // 使能USART1和AFIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);  // 使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  // 使能AFIO时钟

    // 配置PA9为USART1的TX引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;  // 选择PA9引脚
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;  // 设置输出速度为10MHz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 设置为复用推挽输出模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);  // 初始化GPIOA的PA9引脚

    // 配置USART1
    USART_InitStructure.USART_BaudRate = 115200;  // 设置波特率为115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 设置数据位为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 设置停止位为1位
    USART_InitStructure.USART_Parity = USART_Parity_No;  // 设置无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 设置无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx;  // 设置为发送模式
    USART_Init(USART1, &USART_InitStructure);  // 初始化USART1

    USART_Cmd(USART1, ENABLE);  // 使能USART1

    printf("USART1 Init Completely\r\n");  // 通过USART1发送初始化完成信息

    // 初始化GPIO、EXTI和TIM2
    GPIO_Config();
    EXTI_Config();
    TIM2_Config();

    while (1)  // 主循环
    {
    }
}