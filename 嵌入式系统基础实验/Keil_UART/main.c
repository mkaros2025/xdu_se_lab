#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>

#define RX_BUFFER_SIZE 128 // 接收缓冲区大小
#define TX_BUFFER_SIZE 128 // 发送缓冲区大小
#define ID_MAX_SIZE 64     // ID 最大长度

volatile char RX_Buffer[RX_BUFFER_SIZE]; // 接收缓冲区
volatile uint16_t RX_WriteIndex = 0;     // 接收缓冲区的写索引
volatile uint16_t RX_ReadIndex = 0;      // 接收缓冲区的读索引
volatile uint8_t RX_Complete = 0;        // 标志位：表示是否接收到完整的一行数据

uint8_t TX_Buffer[TX_BUFFER_SIZE]; // 发送缓冲区

char ID_Value[ID_MAX_SIZE] = "DefaultID"; // 存储ID的变量，默认值为"DefaultID"

// 重定向printf函数的输出到USART1
int fputc(int ch, FILE *f)
{    
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); // 等待发送数据寄存器为空
    USART_SendData(USART1, (uint8_t)ch); // 发送数据
    return ch;
}

// 配置USART1
void USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 使能GPIOA和USART1的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);

    // 配置USART1的TX引脚（PA9）为复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1的RX引脚（PA10）为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART1的参数：波特率115200，8位数据位，1位停止位，无校验位，无硬件流控制
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    // 使能USART1的接收中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    // 配置USART1的中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 使能USART1
    USART_Cmd(USART1, ENABLE);
}

// 配置DMA
void DMA_Config(void)
{
    DMA_InitTypeDef DMA_InitStructure;

    // 使能DMA1的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // 配置DMA1的通道4（用于USART1的发送）
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR; // 外设地址为USART1的数据寄存器
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)TX_Buffer; // 内存地址为发送缓冲区
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST; // 数据传输方向为内存到外设
    DMA_InitStructure.DMA_BufferSize = TX_BUFFER_SIZE; // 缓冲区大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设地址不递增
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 内存地址递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 内存数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // DMA模式为普通模式（非循环模式）
    DMA_InitStructure.DMA_Priority = DMA_Priority_High; // DMA通道优先级为高
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // 禁用内存到内存的传输

    DMA_Init(DMA1_Channel4, &DMA_InitStructure); // 初始化DMA1的通道4

    DMA_Cmd(DMA1_Channel4, DISABLE); // 先禁用DMA1的通道4
}

// 使用DMA发送数据
void USART1_DMA_SendData(uint8_t *data, uint16_t size)
{
    memcpy(TX_Buffer, data, size); // 将数据复制到发送缓冲区

    DMA_SetCurrDataCounter(DMA1_Channel4, size); // 设置DMA传输的数据量
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE); // 使能USART1的DMA发送请求
    DMA_Cmd(DMA1_Channel4, ENABLE); // 使能DMA1的通道4

    while (DMA_GetFlagStatus(DMA1_FLAG_TC4) == RESET); // 等待DMA传输完成

    DMA_ClearFlag(DMA1_FLAG_TC4); // 清除DMA传输完成标志

    DMA_Cmd(DMA1_Channel4, DISABLE); // 禁用DMA1的通道4
    USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE); // 禁用USART1的DMA发送请求
}

// USART1的中断处理函数
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 检查是否接收到数据
    {
        RX_Buffer[RX_WriteIndex] = USART_ReceiveData(USART1); // 将接收到的数据存入接收缓冲区

        if (RX_Buffer[RX_WriteIndex] == '\n') { // 如果接收到换行符，表示一行数据接收完成
            RX_Complete = 1; // 设置接收完成标志
        }

        RX_WriteIndex = (RX_WriteIndex + 1) % RX_BUFFER_SIZE; // 更新写索引，循环使用缓冲区

        USART_ClearITPendingBit(USART1, USART_IT_RXNE); // 清除接收中断标志
    }
}

// 解析AT指令
void Parse_AT_Command(void)
{
    if (RX_Complete) // 如果接收到完整的一行数据
    {
        char command[RX_BUFFER_SIZE]; // 用于存储解析出的命令
        uint16_t index = RX_ReadIndex; // 从读索引开始解析

        uint16_t i = 0;
        for (i = 0; i < RX_BUFFER_SIZE && RX_Buffer[index] != '\n'; i++) // 读取一行数据
        {
            command[i] = RX_Buffer[index];
            index = (index + 1) % RX_BUFFER_SIZE; // 更新索引，循环使用缓冲区
        }
        command[i] = '\0'; // 添加字符串结束符

        RX_ReadIndex = index + 1; // 更新读索引

        if (strncmp(command, "AT+SETID=", 9) == 0) // 如果命令是设置ID
        {
            strncpy(ID_Value, command + 9, ID_MAX_SIZE - 1); // 提取并保存ID
            ID_Value[ID_MAX_SIZE - 1] = '\0'; // 确保字符串以'\0'结尾
            const char *response = "Success\n"; // 返回成功响应
            USART1_DMA_SendData((uint8_t *)response, strlen(response)); // 发送响应
        }
        else if (strcmp(command, "AT+ID?") == 0) // 如果命令是查询ID
        {
            char response[TX_BUFFER_SIZE];
            snprintf(response, TX_BUFFER_SIZE, "%s\n", ID_Value); // 格式化ID到响应字符串
            USART1_DMA_SendData((uint8_t *)response, strlen(response)); // 发送响应
        }
        else // 如果命令无效
        {
            const char *response = "ERROR\n"; // 返回错误响应
            USART1_DMA_SendData((uint8_t *)response, strlen(response)); // 发送响应
        }

        RX_Complete = 0; // 清除接收完成标志
    }
}

// 主函数
int main(void)
{
    USART_Config(); // 配置USART1
    DMA_Config(); // 配置DMA

    printf("Config complete!\n"); // 打印配置完成信息

    while (1)
    {
        Parse_AT_Command(); // 解析AT指令
    }
}