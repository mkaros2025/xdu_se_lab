#ifndef __SPIFLASH_H
#define __SPIFLASH_H

#include "stm32f10x.h"

#define SPI_FLASH_CS_LOW()		GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define SPI_FLASH_CS_HIGH()		GPIO_SetBits(GPIOA, GPIO_Pin_15)

uint8_t SPI_FLASH_SendByte(uint8_t byte);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);
void SPI_FLASH_Init(void);
void SPI_FLASH_PowerDown(void);
void SPI_FLASH_ReleasePowerDown(void);
void SPI_FLASH_SectorErase(uint32_t SectorAddr);
void SPI_FLASH_PageWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferWrite(uint8_t *pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite);
void SPI_FLASH_BufferRead(uint8_t *pBuffer, uint32_t ReadAddr, uint16_t NumByteToWrite);
uint16_t SPI_FLASH_ReadManufactureID(void);

#endif
