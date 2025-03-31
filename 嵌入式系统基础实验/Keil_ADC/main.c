#include "stm32f10x.h"
#include <stdio.h>

#define ADC_BUFFER_SIZE 20  // 定义ADC缓冲区的大小为20
uint16_t ADC_Buffer[ADC_BUFFER_SIZE];  // 定义一个数组用于存储ADC转换结果

// 重定义fputc函数，用于通过USART1发送字符（用于printf调试）
int fputc(int ch, FILE *f)
{
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  // 等待USART1发送完成
    USART_SendData(USART1, (uint8_t)ch);  // 发送字符
    return ch;
}

// ADC1和ADC2的中断处理函数
void ADC1_2_IRQHandler(void) {
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC)) {  // 检查ADC1转换结束中断标志
        ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);  // 清除中断标志
    }
}

// NVIC配置函数
void NVIC_Config(void) {
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  // 设置NVIC优先级分组为1
    printf("NVIC Init Complete\r\n");  // 打印初始化完成信息
}

// TIM1 PWM初始化函数
void TIM1_PWM_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;  // 定义TIM1时基结构体
    TIM_OCInitTypeDef TIM_OCInitStructure;  // 定义TIM1输出比较结构体

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);  // 使能TIM1时钟

    // 配置TIM1时基
    TIM_TimeBaseStructure.TIM_Period = 5000-1;  // 设置自动重装载值
    TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;  // 设置预分频器
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;  // 重复计数器设置为0
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数模式
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  // 时钟分频为1
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);  // 初始化TIM1时基

    // 配置TIM1输出比较通道1
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;  // PWM模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;  // 使能输出
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;  // 禁用互补输出
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;  // 输出极性为低
    // TIM_Pulse：设置捕获/比较寄存器1（TIM1_CCR1）的值，用于控制PWM的占空比。
    TIM_OCInitStructure.TIM_Pulse = 2500;  // 设置占空比
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);  // 初始化TIM1输出比较通道1
    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  // 使能预装载寄存器

    TIM_Cmd(TIM1, ENABLE);  // 使能TIM1
    TIM_CtrlPWMOutputs(TIM1, ENABLE);  // 使能TIM1的PWM输出
}

// DMA配置函数
void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;  // 定义DMA初始化结构体

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  // 使能DMA1时钟

    // 配置DMA1通道1
    DMA_DeInit(DMA1_Channel1);  // 复位DMA1通道1
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);  // 外设地址为ADC1数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_Buffer;  // 内存地址为ADC缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  // 数据传输方向为外设到内存
    DMA_InitStructure.DMA_BufferSize = ADC_BUFFER_SIZE;  // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // 外设数据大小为半字
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;  // 内存数据大小为半字
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  // 循环模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;  // 高优先级
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  // 禁用内存到内存传输
    DMA_Init(DMA1_Channel1, &DMA_InitStructure);  // 初始化DMA1通道1

    DMA_Cmd(DMA1_Channel1, ENABLE);  // 使能DMA1通道1
}

// ADC配置函数
void ADC_Config(void)
{
    ADC_InitTypeDef ADC_InitStructure;  // 定义ADC初始化结构体
    ADC_DeInit(ADC1);  // 复位ADC1

    NVIC_InitTypeDef NVIC_InitStructure;  // 定义NVIC初始化结构体
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;  // 设置中断通道为ADC1和ADC2
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // 使能中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  // 抢占优先级为0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  // 子优先级为1
    NVIC_Init(&NVIC_InitStructure);  // 初始化NVIC

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);  // 使能ADC1时钟
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);  // 配置ADC时钟为PCLK2的8分频

    // 配置ADC1
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;  // 禁用扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  // 禁用连续转换模式
    // 当 PWM 正好达到哪个 pulse 值的时候会发生这个TIM1的CC1事件
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;  // 外部触发为TIM1的CC1事件
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;  // 转换通道数为1
    ADC_TempSensorVrefintCmd(ENABLE);  // 使能温度传感器和内部参考电压
    ADC_Init(ADC1, &ADC_InitStructure);  // 初始化ADC1
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_71Cycles5);  // 配置ADC1的规则通道16

    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);  // 使能ADC1转换结束中断
    ADC_DMACmd(ADC1, ENABLE);  // 使能ADC1的DMA请求
    ADC_Cmd(ADC1, ENABLE);  // 使能ADC1

    ADC_ResetCalibration(ADC1);  // 复位ADC1校准寄存器
    while (ADC_GetResetCalibrationStatus(ADC1));  // 等待复位校准完成
    ADC_StartCalibration(ADC1);  // 开始ADC1校准
    while (ADC_GetCalibrationStatus(ADC1));  // 等待校准完成
    ADC_ExternalTrigConvCmd(ADC1, ENABLE);  // 使能ADC1外部触发转换
}

// 读取温度函数
float Read_Temperature(void)
{
    uint32_t adc_sum = 0;  // 定义变量用于存储ADC转换结果的总和
    float adc_average = 0.0;  // 定义变量用于存储ADC转换结果的平均值
    uint16_t valid_samples = ADC_BUFFER_SIZE;  // 有效样本数为ADC缓冲区大小

    // 计算ADC缓冲区的总和
    for (uint16_t i = 0; i < ADC_BUFFER_SIZE; i++) {
        adc_sum += ADC_Buffer[i];
    }
    adc_average = adc_sum / (float)valid_samples;  // 计算平均值

    // 根据公式计算温度
    float temperature = ((1.43 - (adc_average * 3.3 / 4096.0)) / 0.0043) + 25.0;

    return temperature;  // 返回计算得到的温度
}

// 主函数
int main(void)
{
    // 初始化USART1用于调试
    GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体
    USART_InitTypeDef USART_InitStructure;  // 定义USART初始化结构体

    // 使能GPIOA、USART1、TIM1和ADC1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_TIM1 | RCC_APB2Periph_ADC1, ENABLE);

    // 配置GPIOA的Pin9为复用推挽输出（USART1的TX引脚）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1
    USART_InitStructure.USART_BaudRate = 115200;  // 波特率为115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;  // 数据位长度为8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1;  // 停止位为1位
    USART_InitStructure.USART_Parity = USART_Parity_No;  // 无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无硬件流控制
    USART_InitStructure.USART_Mode = USART_Mode_Tx;  // 仅使能发送模式
    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);  // 使能USART1

    printf("USART1 Initialized.\r\n");  // 打印USART1初始化完成信息

    NVIC_Config();  // 配置NVIC

    TIM1_PWM_Init();  // 初始化TIM1 PWM
    printf("TIM1 PWM Initialized.\r\n");  // 打印TIM1 PWM初始化完成信息

    DMA_Config();  // 配置DMA
    printf("DMA Configured.\r\n");  // 打印DMA配置完成信息

    ADC_Config();  // 配置ADC
    printf("ADC Configured with External Trigger.\r\n");  // 打印ADC配置完成信息

    // 主循环
    while (1)
    {
        // 打印ADC缓冲区中的值用于调试
        for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
            printf("ADC Buffer[%d]: %d\r\n", i, ADC_Buffer[i]);
        }

        // 读取温度并打印
        float temperature = Read_Temperature();
        printf("Average Temperature: %.2f \r\n", temperature);

        // 简单的延时
        for (uint32_t i = 0; i < 8000000; i++) {
            __NOP();
        }
    }
}