/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"

TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

void vTask1_handler(void* arg);
void vTask2_handler(void* arg);
static void prvSetupUart(void);
static void prvSetupHardware(void);

char msg[100] = "asdasdasdasdasd";
void printmsg(char *msg);

char usr_msg[250];
#define TRUE 1
#define FALSE 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE
uint8_t UART_ACCESS_KEY = AVAILABLE;

#ifdef USESEMIHOSTING
	extern void initialise_monitor_handles();
#endif

int main(void)
{

#ifdef USESEMIHOSTING
	initialise_monitor_handles();
	printf("Hello World Program!!\n");
#endif

	DWT->CTRL |= (1 << 0);

	//HSI ON
	//Reset clock to 16 MHz
	RCC_DeInit();

	//update clock
	SystemCoreClockUpdate();

	prvSetupHardware();

	sprintf(usr_msg, "Hello World Starting\r\n");
	printmsg(usr_msg);

	//create task
	xTaskCreate(vTask1_handler,
			"TASK-1",
			configMINIMAL_STACK_SIZE,
			NULL,
			2,
			&xTaskHandle1
			);

	xTaskCreate(vTask2_handler,
				"TASK-2",
				configMINIMAL_STACK_SIZE,
				NULL,
				2,
				&xTaskHandle2
				);

	vTaskStartScheduler();

	for(;;);
}

void vTask1_handler(void* arg)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("********* Entering Task-1 *********\r\n");
			vTaskDelay(2000);
			printmsg("********* Leaving Task-1 *********\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}
}

void vTask2_handler(void* arg)
{
	while(1)
	{
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE;
			printmsg("********* Entering Task-2 *********\r\n");
			vTaskDelay(2000);
			printmsg("********* Leaving Task-2 *********\r\n");
			UART_ACCESS_KEY = AVAILABLE;
			taskYIELD();
		}
	}
}

static void prvSetupUart(void)
{
	//enable uart
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);

	//PA2 TX, PA3 RX
	//alternate function configuration
	GPIO_InitTypeDef GPIO_InitStruct;
	memset(&GPIO_InitStruct, 0, sizeof(GPIO_InitStruct));
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//AF mode
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	//UART parameter init
	USART_InitTypeDef USART_InitStruct;
	memset(&USART_InitStruct, 0, sizeof(USART_InitStruct));
	USART_InitStruct.USART_BaudRate = 115200;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &USART_InitStruct);

	USART_Cmd(USART2, ENABLE);
}

static void prvSetupHardware(void)
{
	prvSetupUart();
}

void printmsg(char *msg)
{
	for(uint32_t i=0; i<strlen(msg);i++)
	{
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
		USART_SendData(USART2, msg[i]);
	}
}
