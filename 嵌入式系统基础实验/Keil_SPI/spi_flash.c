#include "spi_flash.h"  // 包含SPI Flash操作相关的头文件
#include "stdio.h"      // 包含标准输入输出库，用于调试信息输出

// SPI Flash指令定义
// 对应文件的 44 页 45 页
#define SPIMEM_PageSize 256          // Flash页大小
#define SPIMEM_PerWritePageSize 256  // 每次写入的页大小
#define SPIMEM_WriteEnable 0x06      // 写使能指令
#define SPIMEM_WriteDisable 0x04     // 写禁止指令
#define SPIMEM_ReadStatusReg 0x05    // 读状态寄存器指令
#define SPIMEM_WriteStatusReg 0x01   // 写状态寄存器指令
#define SPIMEM_ReadData 0x03         // 读数据指令
#define SPIMEM_PageProgram 0x02      // 页编程指令
#define SPIMEM_SectorErase 0x20      // 扇区擦除指令
#define SPIMEM_32KBBlockErase 0x52   // 32KB块擦除指令
#define SPIMEM_64KBBlockErase 0xD8   // 64KB块擦除指令
#define SPIMEM_ChipErase 0xC7        // 全片擦除指令
#define SPIMEM_PowerDown 0xB9        // 进入Power-Down模式指令
#define SPIMEM_ReleasePowerDown 0xAB // 退出Power-Down模式指令
#define SPIMEM_ManufactureID 0x90    // 读取制造商和设备ID指令
#define Dummy_Byte 0xFF              // 空字节，用于读取操作
#define WIP_Flag 0x01                // 状态寄存器中的写操作进行中标志位

// SPI Flash初始化函数
void SPI_FLASH_Init(void)
{
    // 使能SPI1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 将Flash的片选信号置高（未选中）
    SPI_FLASH_CS_HIGH();

    // 定义SPI初始化结构体
    SPI_InitTypeDef SPI_InitStructure;

    // 配置SPI为全双工模式
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    // 配置SPI为主模式
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    // 配置SPI数据帧为8位
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    // 配置SPI时钟极性为低电平空闲
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    // 配置SPI时钟相位为第一个边沿采样
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    // 配置SPI的NSS信号为软件控制
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    // 配置SPI波特率预分频为16
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    // 配置SPI数据传输从最高位开始
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    // 配置SPI CRC多项式为0（不使用CRC）
    SPI_InitStructure.SPI_CRCPolynomial = 0;

    // 初始化SPI1
    SPI_Init(SPI1, &SPI_InitStructure);
    // 使能SPI1
    SPI_Cmd(SPI1, ENABLE);
}

// SPI Flash扇区擦除函数
void SPI_FLASH_SectorErase(uint32_t SectorAddr)
{
    // 发送写使能指令
    SPI_FLASH_WriteEnable();
    // 等待写操作完成
    SPI_FLASH_WaitForWriteEnd();

    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送扇区擦除指令
    SPI_FLASH_SendByte(SPIMEM_SectorErase);
    // 发送扇区地址的高字节
    SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
    // 发送扇区地址的中字节
    SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
    // 发送扇区地址的低字节
    SPI_FLASH_SendByte(SectorAddr & 0xFF);
    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
    // 等待擦除操作完成
    SPI_FLASH_WaitForWriteEnd();
}

// SPI Flash页写入函数
void SPI_FLASH_PageWrite(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
    // 发送写使能指令
    SPI_FLASH_WriteEnable();
    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送页编程指令
    SPI_FLASH_SendByte(SPIMEM_PageProgram);
    // 发送写入地址的高字节
    SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
    // 发送写入地址的中字节
    SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
    // 发送写入地址的低字节
    SPI_FLASH_SendByte(WriteAddr & 0xFF);

    // 逐个字节写入数据
    while (NumByteToWrite--)
    {
        SPI_FLASH_SendByte(*pBuffer);
        pBuffer++;
    }

    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
    // 等待写操作完成
    SPI_FLASH_WaitForWriteEnd();
}

// SPI Flash缓冲区读取函数
void SPI_FLASH_BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead)
{
    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送读数据指令
    SPI_FLASH_SendByte(SPIMEM_ReadData);
    // 发送读取地址的高字节
    SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
    // 发送读取地址的中字节
    SPI_FLASH_SendByte((ReadAddr & 0xFF00) >> 8);
    // 发送读取地址的低字节
    SPI_FLASH_SendByte(ReadAddr & 0xFF);

    // 逐个字节读取数据
    while (NumByteToRead--)
    {
        *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
        pBuffer++;
    }

    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
}

// SPI Flash发送单字节函数
uint8_t SPI_FLASH_SendByte(uint8_t byte)
{
    // 等待发送缓冲区为空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    // 发送一个字节
    SPI_I2S_SendData(SPI1, byte);
    // 等待接收缓冲区非空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    // 返回接收到的字节
    return SPI_I2S_ReceiveData(SPI1);
}

// SPI Flash写使能函数
void SPI_FLASH_WriteEnable(void)
{
    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送写使能指令
    SPI_FLASH_SendByte(SPIMEM_WriteEnable);
    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
}

// SPI Flash等待写操作完成函数
void SPI_FLASH_WaitForWriteEnd(void)
{
    uint8_t FLASH_Status = 0;

    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送读状态寄存器指令
    SPI_FLASH_SendByte(SPIMEM_ReadStatusReg);

    // 循环检查状态寄存器中的写操作进行中标志位
    do
    {
        FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
    } while ((FLASH_Status & WIP_Flag) == SET);

    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
}

// SPI Flash进入Power-Down模式函数
void SPI_FLASH_PowerDown(void)
{
    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送进入Power-Down模式指令
    SPI_FLASH_SendByte(SPIMEM_PowerDown);
    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
}

// SPI Flash退出Power-Down模式函数
void SPI_FLASH_ReleasePowerDown(void)
{
    // 选中Flash
    // 其实就是 GPIO_ResetBits(GPIOA, GPIO_Pin_15)
    SPI_FLASH_CS_LOW();
    // 发送退出Power-Down模式指令
    SPI_FLASH_SendByte(SPIMEM_ReleasePowerDown);
    // 取消选中Flash
    // 其实就是 GPIO_SetBits(GPIOA, GPIO_Pin_15)
    SPI_FLASH_CS_HIGH();
}

// SPI Flash读取制造商和设备ID函数
uint16_t SPI_FLASH_ReadManufactureID(void)
{
    uint8_t manufacturerid;
    uint8_t deviceid;

    // 选中Flash
    SPI_FLASH_CS_LOW();
    // 发送读取制造商和设备ID指令
    SPI_FLASH_SendByte(SPIMEM_ManufactureID);
    // 发送3个空字节
    // 这是 SPI flash 的格式要求
    // 一共 6 个字节，第一个字节是指令码
    // 第 2，3，4个字节是地址，表示从哪里开始读取
    // 第5、6个字节：返回的制造商ID和设备ID
    SPI_FLASH_SendByte(0x00);
    SPI_FLASH_SendByte(0x00);
    SPI_FLASH_SendByte(0x00);
    // 读取制造商ID
    manufacturerid = SPI_FLASH_SendByte(Dummy_Byte);
    // 读取设备ID
    deviceid = SPI_FLASH_SendByte(Dummy_Byte);
    // 取消选中Flash
    SPI_FLASH_CS_HIGH();
    // 返回制造商和设备ID的组合值
    return ((uint16_t)manufacturerid << 8) | deviceid;
}