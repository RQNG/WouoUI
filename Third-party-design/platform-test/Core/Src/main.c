/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

struct Button btn1;
struct Button btn2;
struct Button btn3;

uint8_t read_button_GPIO(uint8_t button_id)
{
	// you can share the GPIO read function with multiple Buttons
	switch(button_id)
	{
		case 1:
			return LL_GPIO_IsInputPinSet(KEY1_GPIO_Port, KEY1_Pin);
			break;
		case 2:
			return LL_GPIO_IsInputPinSet(KEY2_GPIO_Port, KEY2_Pin);
			break;
		case 3:
			return LL_GPIO_IsInputPinSet(KEY3_GPIO_Port, KEY3_Pin);
			break;
		default:
			return 0;
			break;
	}
}

void button_callback(void *button)
{
	struct Button *btn = button;
	gui_btn_send_signal(btn->button_id,btn->event);
#if 0
	switch(btn->event)
	{
	 case PRESS_DOWN:
//			 LOG_INFO("---> key%d press down! <---\r\n",btn->button_id); 
		break; 

	 case PRESS_UP: 
//			 LOG_INFO("---> key%d press up! <---\r\n",btn->button_id);
				gui_btn_send_signal(btn->button_id,btn->event);
		break; 

	 case PRESS_REPEAT: 
//			 LOG_INFO("---> key%d press repeat! <---\r\n",btn->button_id);
		break; 

	 case SINGLE_CLICK: 
//			 LOG_INFO("---> key%d single click! <---\r\n",btn->button_id);
		break; 

	 case DOUBLE_CLICK: 
//			 LOG_INFO("---> key%d double click! <***\r\n",btn->button_id);
		break; 

	 case LONG_PRESS_START: 
//			 LOG_INFO("---> key%d long press start! <---\r\n",btn->button_id);
		break; 

	 case LONG_PRESS_HOLD: 
//			 LOG_INFO("---> key%d long press hold! <***\r\n",btn->button_id);
		break; 
 }
#endif
}

void button_tick(void)
{
  static uint8_t tickstart = 0;
	tickstart++;
  if(tickstart < 5)
		return;
	tickstart = 0;
	button_ticks();
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  LL_mDelay(50);
	LOG_INFO("init\r\n");
	
	power_monitor_init();
	buzzer_init();
	buzzer_on(500);
	LL_GPIO_SetOutputPin(VCC_3V3_CRTL_GPIO_Port, VCC_3V3_CRTL_Pin);
	
	button_init(&btn1, read_button_GPIO, 1, 1);
	button_attach(&btn1, PRESS_DOWN,       button_callback);
	button_attach(&btn1, PRESS_UP,         button_callback);
//	button_attach(&btn1, PRESS_REPEAT,     button_callback);
//	button_attach(&btn1, SINGLE_CLICK,     button_callback);
//	button_attach(&btn1, DOUBLE_CLICK,     button_callback);
//	button_attach(&btn1, LONG_PRESS_START, button_callback);
	button_attach(&btn1, LONG_PRESS_HOLD, button_callback);
	button_start(&btn1);
	
	button_init(&btn2, read_button_GPIO, 1, 2);
	button_attach(&btn2, PRESS_DOWN,       button_callback);
	button_attach(&btn2, PRESS_UP,         button_callback);
//	button_attach(&btn2, PRESS_REPEAT,     button_callback);
//	button_attach(&btn2, SINGLE_CLICK,     button_callback);
//	button_attach(&btn2, DOUBLE_CLICK,     button_callback);
	button_attach(&btn2, LONG_PRESS_START, button_callback);
	button_start(&btn2);

	button_init(&btn3, read_button_GPIO, 1, 3);
	button_attach(&btn3, PRESS_DOWN,       button_callback);
	button_attach(&btn3, PRESS_UP,         button_callback);
	button_attach(&btn3, PRESS_REPEAT,     button_callback);
//	button_attach(&btn3, SINGLE_CLICK,     button_callback);
//	button_attach(&btn3, DOUBLE_CLICK,     button_callback);
//	button_attach(&btn3, LONG_PRESS_START, button_callback);
	button_attach(&btn3, LONG_PRESS_HOLD, button_callback);
	button_start(&btn3);
	
	gui_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		gui_task();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
  LL_RCC_SetADCClockSource(LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
