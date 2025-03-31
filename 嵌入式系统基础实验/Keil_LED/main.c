/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>

int fputc(int ch, FILE *f)
{    
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET);
	USART_SendData(USART1, (uint8_t)ch);
	      
	return ch;
}

/* Private functions ---------------------------------------------------------*/

void GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable clocks for GPIOC, GPIOB, GPIOA
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Configure PC13 as push-pull output (LED0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // Configure PB2 as push-pull output (LED1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // Configure PA8 as input (KEY0)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input with pull-up
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Configure PB10 as input (KEY1)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // Input with pull-up
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// Initialize GPIOs for LEDs and keys
    GPIO_Config();
 	
	//Enable GPIOA's Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		
	//Enable USART1's Clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	//USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);//GPIOA.9
	
	//Set USART1's Config
	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;	
    USART_Init(USART1, &USART_InitStructure); 
	
	//Enable USART1
    USART_Cmd(USART1, ENABLE); 
	
	printf("USART1 Init Completely\r\n");
	
  /* Infinite loop */
  while (1)
  {
		// Toggle LED0 (PC13) and LED1 (PB2) based on the key presses
        if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == 1) // KEY0 pressed
        {
            GPIO_ResetBits(GPIOC, GPIO_Pin_13); // Turn on LED0
        }
        else
        {
            GPIO_SetBits(GPIOC, GPIO_Pin_13);  // Turn off LED0
        }

        if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10) == 1) // KEY1 pressed
        {
            GPIO_ResetBits(GPIOB, GPIO_Pin_2); // Turn on LED1
        }
        else
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_2);  // Turn off LED1
        }
  }
}
