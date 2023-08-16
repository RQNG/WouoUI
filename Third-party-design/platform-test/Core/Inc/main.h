/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_adc.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mylib.h"
#include "flash.h"
#include "oled.h"
#include "ir_sw.h"
#include "buzzer.h"
#include "ranging.h"
#include "gui_main.h"
#include "multi_button.h"
#include "power_monitor.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void button_tick(void);
	
#define VCC_3V3_CRTL_Pin LL_GPIO_PIN_13
#define VCC_3V3_CRTL_GPIO_Port GPIOC
#define VCC_5V_CTRL_Pin LL_GPIO_PIN_14
#define VCC_5V_CTRL_GPIO_Port GPIOC
#define GHA_Pin LL_GPIO_PIN_15
#define GHA_GPIO_Port GPIOC
#define KEY3_Pin LL_GPIO_PIN_0
#define KEY3_GPIO_Port GPIOA
#define KEY1_Pin LL_GPIO_PIN_1
#define KEY1_GPIO_Port GPIOA
#define SPI1_CS_Pin LL_GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_SCK_Pin LL_GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin LL_GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin LL_GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define KEY2_Pin LL_GPIO_PIN_2
#define KEY2_GPIO_Port GPIOB
#define IR_IN_Pin LL_GPIO_PIN_8
#define IR_IN_GPIO_Port GPIOA
#define IR_IN_EXTI_IRQn EXTI9_5_IRQn
#define VCC_USB_CTRL_Pin LL_GPIO_PIN_15
#define VCC_USB_CTRL_GPIO_Port GPIOA
#define BUZZER_Pin LL_GPIO_PIN_3
#define BUZZER_GPIO_Port GPIOB
#define PWM2_Pin LL_GPIO_PIN_4
#define PWM2_GPIO_Port GPIOB
#define PWM1_Pin LL_GPIO_PIN_5
#define PWM1_GPIO_Port GPIOB
#define LCD_SCL_Pin LL_GPIO_PIN_6
#define LCD_SCL_GPIO_Port GPIOB
#define LCD_SDA_Pin LL_GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOB

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define VCC_3V3_CRTL_Pin LL_GPIO_PIN_13
#define VCC_3V3_CRTL_GPIO_Port GPIOC
#define VCC_5V_CTRL_Pin LL_GPIO_PIN_14
#define VCC_5V_CTRL_GPIO_Port GPIOC
#define GHA_Pin LL_GPIO_PIN_15
#define GHA_GPIO_Port GPIOC
#define KEY3_Pin LL_GPIO_PIN_0
#define KEY3_GPIO_Port GPIOA
#define KEY1_Pin LL_GPIO_PIN_1
#define KEY1_GPIO_Port GPIOA
#define SPI1_CS_Pin LL_GPIO_PIN_4
#define SPI1_CS_GPIO_Port GPIOA
#define SPI1_SCK_Pin LL_GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISO_Pin LL_GPIO_PIN_6
#define SPI1_MISO_GPIO_Port GPIOA
#define SPI1_MOSI_Pin LL_GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define EX_POWER_Pin LL_GPIO_PIN_0
#define EX_POWER_GPIO_Port GPIOB
#define EX_VREF_Pin LL_GPIO_PIN_1
#define EX_VREF_GPIO_Port GPIOB
#define KEY2_Pin LL_GPIO_PIN_2
#define KEY2_GPIO_Port GPIOB
#define IR_IN_Pin LL_GPIO_PIN_8
#define IR_IN_GPIO_Port GPIOA
#define IR_IN_EXTI_IRQn EXTI9_5_IRQn
#define VCC_IR_CTRL_Pin LL_GPIO_PIN_15
#define VCC_IR_CTRL_GPIO_Port GPIOA
#define BUZZER_Pin LL_GPIO_PIN_3
#define BUZZER_GPIO_Port GPIOB
#define PWM2_Pin LL_GPIO_PIN_4
#define PWM2_GPIO_Port GPIOB
#define PWM1_Pin LL_GPIO_PIN_5
#define PWM1_GPIO_Port GPIOB
#define LCD_SCL_Pin LL_GPIO_PIN_6
#define LCD_SCL_GPIO_Port GPIOB
#define LCD_SDA_Pin LL_GPIO_PIN_7
#define LCD_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
