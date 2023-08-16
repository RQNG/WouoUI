#ifndef __OLED_H
#define __OLED_H

#include "main.h"
#include "stdlib.h"

#include "u8g2.h"
#include "u8x8.h"

//#define MAX_LEN    		128  //
#define OLED_ADDRESS  0x78 // oled模块从机地址
//#define OLED_CMD   		0x00  // 写命令
//#define OLED_DATA  		0x40  // 写数据

#define OLED_CS_Clr()  LL_GPIO_ResetOutputPin(PWM1_GPIO_Port, PWM1_Pin)
#define OLED_CS_Set()  LL_GPIO_SetOutputPin(PWM1_GPIO_Port, PWM1_Pin)

#define OLED_RST_Clr() LL_GPIO_ResetOutputPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin)
#define OLED_RST_Set() LL_GPIO_SetOutputPin(SPI1_MISO_GPIO_Port, SPI1_MISO_Pin)

#define OLED_DC_Clr() LL_GPIO_ResetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin)
#define OLED_DC_Set() LL_GPIO_SetOutputPin(SPI1_CS_GPIO_Port, SPI1_CS_Pin)

#define OLED_SCLK_Clr() LL_GPIO_ResetOutputPin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin)
#define OLED_SCLK_Set() LL_GPIO_SetOutputPin(SPI1_SCK_GPIO_Port, SPI1_SCK_Pin)

#define OLED_SDIN_Clr() LL_GPIO_ResetOutputPin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin)
#define OLED_SDIN_Set() LL_GPIO_SetOutputPin(SPI1_MOSI_GPIO_Port, SPI1_MOSI_Pin)  

uint8_t stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr);
							   		    
void oled_init(void);



#endif  
	 



