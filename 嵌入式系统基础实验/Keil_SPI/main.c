#include "stm32f10x.h"          // STM32F10x系列微控制器的标准外设库头文件
#include "spi_flash.h"          // SPI Flash操作相关的头文件
#include "usart1.h"             // USART1串口通信相关的头文件
#include <string.h>             // 字符串操作函数库

typedef enum {FAILED=0, PASSED=1} TestStatus;  // 定义枚举类型TestStatus，用于表示测试结果状态

#define FLASH_WriteAddress	0x000000  // 定义Flash写入地址
#define FLASH_ReadAddress		FLASH_WriteAddress  // 定义Flash读取地址，与写入地址相同
#define FLASH_SectorToErase	FLASH_WriteAddress  // 定义要擦除的Flash扇区地址

// 定义要写入Flash的数据
uint8_t Tx_Buffer[] = "MK 22009200472";

// 定义从Flash读取数据的缓冲区
uint8_t Rx_Buffer[100];

// 定义测试状态变量，初始状态为FAILED
TestStatus Transferstatus = FAILED;

// 函数声明：用于比较两个缓冲区的数据是否相同
TestStatus Buffercmp(uint8_t* pBufferl, uint8_t* pBuffer2, uint16_t BufferLength);

// GPIO配置函数
void GPIO_Config() {
	GPIO_InitTypeDef GPIO_InitStructure;  // 定义GPIO初始化结构体

	// 使能GPIOA和GPIOB的时钟，并开启AFIO（复用功能IO）时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	// 使能SPI1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	// 配置JTAG引脚为禁用状态，释放PB3、PB4、PA15引脚用于SPI1
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);

	// 配置PB3为SPI1的SCK引脚，模式为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 配置PB4为SPI1的MISO引脚，模式为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 配置PB5为SPI1的MOSI引脚，模式为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// 配置PA15为SPI1的NSS引脚，模式为推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

// 主函数
int main(void)
{
	uint16_t ManufactureDeviceID;  // 定义变量用于存储Flash的制造商和设备ID
	uint32_t BufferSize = strlen((const char *)Tx_Buffer);  // 计算要写入Flash的数据长度
	
	// 配置USART1串口，波特率为115200
	USART1_Config(115200);

	// 打印启动信息
	printf("SPI FLASH START\r\n");
	
	// 配置GPIO引脚
	GPIO_Config();
	
	// 初始化SPI Flash
	SPI_FLASH_Init();

	// 唤醒SPI Flash（从Power-Down模式中恢复）
	SPI_FLASH_ReleasePowerDown();

	// 读取Flash的制造商和设备ID
	ManufactureDeviceID = SPI_FLASH_ReadManufactureID();
	
	// 判断读取的ID是否正确（制造商ID为0xEF）
	if ((ManufactureDeviceID & 0xFF00) == 0xEF00)
			printf("Read Flash ManufactureDeviceID 0x%x Successfully\r\n", ManufactureDeviceID);
	else
			printf("Read Flash ManufactureDeviceID 0x%x Failed\r\n", ManufactureDeviceID);

	// 擦除指定的Flash扇区
	SPI_FLASH_SectorErase(FLASH_SectorToErase);
	printf("Erase the sector 0x%x successfully\r\n", FLASH_SectorToErase);

	// 从Flash读取数据到Rx_Buffer
	SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);

	// 打印读取到的每个字节数据
	for (int i = 0; i < BufferSize; i++) {
		printf("Byte %d: 0x%02X\r\n", i, Rx_Buffer[i]);
	}

	// 将Tx_Buffer中的数据写入Flash
	SPI_FLASH_PageWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
	printf("The data written to the FLASH:\r\n %s\r\n", Tx_Buffer);

	// 从Flash读取数据到Rx_Buffer
	SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
	printf("The data read from the FLASH:\r\n %s\r\n", Rx_Buffer);

	// 比较写入和读取的数据是否一致
	Transferstatus = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);
	if (PASSED == Transferstatus)
			printf("Program the FLASH successfully! \r\n");
	else
			printf("Failed to program the FLASH! \r\n");

	// 将SPI Flash进入Power-Down模式
	SPI_FLASH_PowerDown();
	
	// 主循环
	while(1)
	{
		// 主循环中不执行任何操作
	}
}

// 缓冲区比较函数
TestStatus Buffercmp(uint8_t* pBufferl, uint8_t* pBuffer2, uint16_t BufferLength)
{
	// 逐个字节比较两个缓冲区的内容
	while(BufferLength--)
	{
		// 如果发现不相同的字节，返回FAILED
		if(*pBufferl != *pBuffer2)
			return FAILED;
		pBufferl++;
		pBuffer2++;
	}
	// 如果所有字节都相同，返回PASSED
	return PASSED;
}