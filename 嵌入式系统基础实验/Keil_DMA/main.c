#include "stm32f10x.h"
#include <stdio.h>

/* 定义Flash和SRAM中的数组 */
const int N = 10000;  // 定义数组大小为10000
const uint8_t FlashArray[N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10}; // 定义Flash中的数组
uint8_t SRAMArray[N] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11}; // 定义SRAM中的数组
static uint32_t dma_start_time, dma_end_time; // 用于记录DMA传输的开始和结束时间
static uint32_t cpu_start_time, cpu_end_time; // 用于记录CPU传输的开始和结束时间

/* 重定向printf函数到USART1 */
int fputc(int ch, FILE *f)
{    
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET); // 等待USART1发送完成
    USART_SendData(USART1, (uint8_t)ch); // 发送字符
    return ch;
}

/* GPIO配置函数 */
void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // 使能GPIOC, GPIOB, GPIOA的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // 配置PC13为推挽输出（LED0）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // 配置PB2为推挽输出（LED1）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置PA8为上拉输入（KEY0，用于中断）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置PB10为上拉输入（KEY1，用于中断）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* DMA配置函数 */
void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能DMA1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 复位DMA1通道6到默认值
    DMA_DeInit(DMA1_Channel6);

    // 配置DMA1通道6
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)FlashArray; // 源地址（Flash）
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)SRAMArray;      // 目标地址（SRAM）
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;               // 数据传输方向：从外设（Flash）到内存（SRAM）
    DMA_InitStructure.DMA_BufferSize = N;                           // 传输大小（FlashArray中的元素数量）
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;  // 使能外设地址递增（FlashArray）
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;          // 使能内存地址递增（SRAMArray）
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 设置外设数据大小为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;        // 设置内存数据大小为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                    // 正常模式（非循环模式）
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;            // 设置中等优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;                      // 使能内存到内存传输

    DMA_Init(DMA1_Channel6, &DMA_InitStructure);

    // 使能DMA1通道6传输完成中断
    DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);

    // 配置NVIC以处理DMA中断
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* TIM2配置函数，用于计时 */
void TIM2_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // 使能TIM2时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    
    // TIM2配置
    TIM_TimeBaseStructure.TIM_Period = 0xFFFFFFFF; // 最大周期，用于计时长间隔
    TIM_TimeBaseStructure.TIM_Prescaler = 71;      // 预分频器，设置为1 MHz（假设系统时钟为72 MHz）
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_Cmd(TIM2, ENABLE); // 启动TIM2
}

/* LED控制函数 */
void LED_Control(uint8_t led, uint8_t status)
{
    if (led == 0) { // 控制LED0（PC13）
        if (!status) {
            GPIO_SetBits(GPIOC, GPIO_Pin_13); // 熄灭LED0
        } else {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 点亮LED0
        }
    } else if (led == 1) { // 控制LED1（PB2）
        if (!status) {
            GPIO_SetBits(GPIOB, GPIO_Pin_2); // 熄灭LED1
        } else {
            GPIO_ResetBits(GPIOB, GPIO_Pin_2); // 点亮LED1
        }
    }
}

/* 外部中断配置函数 */
void EXTI_Config(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 配置PA8为外部中断线8
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource8);
    EXTI_InitStructure.EXTI_Line = EXTI_Line8;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置NVIC以处理外部中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 配置PB10为外部中断线10
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource10);
    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    // 配置NVIC以处理外部中断
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/* 外部中断9_5中断服务函数 */
void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) // 检查是否发生了PA8的中断
    {
        LED_Control(0, 0); // 点亮LED0
			
        // 延时
        for (uint32_t i = 0; i < 800000; i++) {
            __NOP(); 
        }

        EXTI_ClearITPendingBit(EXTI_Line8); // 清除中断标志位
        DMA_Cmd(DMA1_Channel6, DISABLE); // 禁用DMA1通道6
        DMA_Config(); // 重新配置DMA
        printf("already reset DMA\n"); // 打印信息
        DMA_Cmd(DMA1_Channel6, ENABLE); // 使能DMA1通道6
        dma_start_time = TIM_GetCounter(TIM2); // 记录DMA传输开始时间
    }
}

/* 外部中断15_10中断服务函数 */
void EXTI15_10_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line10) != RESET) // 检查是否发生了PB10的中断
    {
        cpu_start_time = TIM_GetCounter(TIM2); // 记录CPU传输开始时间

        // 使用CPU将FlashArray中的数据复制到SRAMArray
        for (int i = 0; i < N; i++) {
            SRAMArray[i] = FlashArray[i];
        }

        cpu_end_time = TIM_GetCounter(TIM2); // 记录CPU传输结束时间
        printf("already complete CPU copy\n"); // 打印信息

        printf("CPU transfer time: %u cycles\n", cpu_end_time - cpu_start_time); // 打印CPU传输时间

        EXTI_ClearITPendingBit(EXTI_Line10); // 清除中断标志位
    }
}

/* DMA1通道6中断服务函数 */
void DMA1_Channel6_IRQHandler(void)
{
    if (DMA_GetITStatus(DMA1_IT_TC6)) // 检查是否发生了DMA传输完成中断
    {
        dma_end_time = TIM_GetCounter(TIM2); // 记录DMA传输结束时间
        LED_Control(0, 1); // 熄灭LED0

        printf("DMA transfer time: %u cycles\n", dma_end_time - dma_start_time); // 打印DMA传输时间

        DMA_ClearITPendingBit(DMA1_IT_TC6); // 清除中断标志位
    }
}

/* 主函数 */
int main(void)
{
    USART_InitTypeDef USART_InitStructure;

    // 使能USART1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
    // 配置USART1的GPIO引脚
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置USART1
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;    
    USART_Init(USART1, &USART_InitStructure); 
    
    USART_Cmd(USART1, ENABLE); // 使能USART1
    
    printf("USART1 Init Completely\r\n"); // 打印信息
		
    GPIO_Config(); // 配置GPIO
    EXTI_Config(); // 配置外部中断
    TIM2_Config(); // 配置TIM2

    /* 无限循环 */
    while (1)
    {
        uint8_t data_match = 1; // 标志位，用于跟踪数据是否匹配

        // 比较FlashArray和SRAMArray中的数据
        for (int i = 0; i < 10; i++) {
            if (FlashArray[i] != SRAMArray[i]) {
                data_match = 0; // 数据不匹配
                break;
            }
        }

        // 根据比较结果控制LED1
        LED_Control(1, data_match);
				
        // 延时
        for (uint32_t i = 0; i < 800000; i++) {
            __NOP(); 
        }
    }
}