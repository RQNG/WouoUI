#include "oled.h"
#include "i2c.h"

extern u8g2_t u8g2;

uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)//硬件I2C
{
	/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
	static uint8_t buffer[128];
	static uint8_t buf_idx;
	uint8_t *data;

	switch (msg)
	{
		case U8X8_MSG_BYTE_INIT:
		{
			/* add your custom code to init i2c subsystem */
			MX_I2C1_Init(); //I2C初始化
		}
		break;
		case U8X8_MSG_BYTE_START_TRANSFER:
		{
			buf_idx = 0;
		}
		break;
		case U8X8_MSG_BYTE_SEND:
		{
			data = (uint8_t *)arg_ptr;
			while (arg_int > 0)
			{
				buffer[buf_idx++] = *data;
				data++;
				arg_int--;
			}
		}
		break;
		case U8X8_MSG_BYTE_END_TRANSFER:
		{
			int ret = HAL_I2C_Master_Transmit(&hi2c1, (OLED_ADDRESS), buffer, buf_idx, 1000);
			if (ret != HAL_OK)
			{
				return 0;
			}
		}
		break;
		case U8X8_MSG_BYTE_SET_DC:
				break;
		default:
				return 0;
	}
	return 1;
}

uint8_t stm32_gpio_and_delay(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
#if 0
  switch (msg)
  {
//  case U8X8_MSG_GPIO_AND_DELAY_INIT:
//       oled_init();                    
//  break;
		case U8X8_MSG_GPIO_SPI_DATA:
				if(arg_int)OLED_SDIN_Set();
				else OLED_SDIN_Clr();
		break;
		case U8X8_MSG_GPIO_SPI_CLOCK:
				if(arg_int)OLED_SCLK_Set();
				else OLED_SCLK_Clr();
		break;        
		case U8X8_MSG_GPIO_CS:
				//CS????
		case U8X8_MSG_GPIO_DC:
				if(arg_int)OLED_DC_Set();
				else OLED_DC_Clr();
		break;
		case U8X8_MSG_GPIO_RESET:
				if(arg_int)OLED_RST_Set();
				else OLED_RST_Clr();
		break;
		//Function which delays 100ns  
		case U8X8_MSG_DELAY_100NANO:  
				__NOP();  
		break;  
		case U8X8_MSG_DELAY_MILLI:
				LL_mDelay(arg_int);
		break;
		default:
				return 0;//A message was received which is not implemented, return 0 to indicate an error
  }
  return 1;
#elif 0
	// printf("%s:msg = %d,arg_int = %d\r\n",__FUNCTION__,msg,arg_int);
	switch (msg)
	{
		//		case U8X8_MSG_GPIO_AND_DELAY_INIT:
		//				oled_init();
		//		break;
	case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
		__NOP();
		break;
	case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
		for (uint16_t n = 0; n < 320; n++)
		{
			__NOP();
		}
		break;
	case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
		LL_mDelay(1);
		break;
	case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
		// delay 5us
		delay_us(5); // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us

	case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
		if (arg_int == 1)
		{
			LL_GPIO_SetOutputPin(LCD_SCL_GPIO_Port, LCD_SCL_Pin);
			//        HAL_GPIO_WritePin(GPIOB, SCL2_Pin, GPIO_PIN_SET);
		}
		else if (arg_int == 0)
		{
			LL_GPIO_ResetOutputPin(LCD_SCL_GPIO_Port, LCD_SCL_Pin);
			//        HAL_GPIO_WritePin(GPIOB, SCL2_Pin, GPIO_PIN_RESET);
		}
		break;					 // arg_int=1: Input dir with pullup high for I2C clock pin
	case U8X8_MSG_GPIO_I2C_DATA: // arg_int=0: Output low at I2C data pin
		//  printf("U8X8_MSG_GPIO_I2C_DATA:%d\r\n",arg_int);
		if (arg_int == 1)
		{
			LL_GPIO_SetOutputPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
			//        HAL_GPIO_WritePin(GPIOB, SDA2_Pin, GPIO_PIN_SET);
		}
		else if (arg_int == 0)
		{
			LL_GPIO_ResetOutputPin(LCD_SDA_GPIO_Port, LCD_SDA_Pin);
			//        HAL_GPIO_WritePin(GPIOB, SDA2_Pin, GPIO_PIN_RESET);
		}
		break; // arg_int=1: Input dir with pullup high for I2C data pin

	default:
		u8x8_SetGPIOResult(u8x8, 1); // default return value
		break;
	}
	return 1;
#elif 0
	switch (msg)
	{
	case U8X8_MSG_GPIO_AND_DELAY_INIT:
		break;
	case U8X8_MSG_DELAY_MILLI:
		LL_mDelay(arg_int);
		break;
	case U8X8_MSG_GPIO_I2C_CLOCK:
		break;
	case U8X8_MSG_GPIO_I2C_DATA:
		break;
	default:
		return 0;
	}
	return 1; // command processed successfully.
#elif 1	
	switch (msg)
	{
		case U8X8_MSG_DELAY_100NANO: // delay arg_int * 100 nano seconds
				__NOP();
				break;
		case U8X8_MSG_DELAY_10MICRO: // delay arg_int * 10 micro seconds
				for (uint16_t n = 0; n < 320; n++)
				{
					__NOP();
				}
				break;
		case U8X8_MSG_DELAY_MILLI: // delay arg_int * 1 milli second
				HAL_Delay(1);
				break;
		case U8X8_MSG_DELAY_I2C: // arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
//				delay_us(5);
				for (uint16_t n = 0; n < 160; n++)
				{
					__NOP();
				}
				break;                    // arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
		case U8X8_MSG_GPIO_I2C_CLOCK: // arg_int=0: Output low at I2C clock pin
				break;                    // arg_int=1: Input dir with pullup high for I2C clock pin
		case U8X8_MSG_GPIO_I2C_DATA:  // arg_int=0: Output low at I2C data pin
				break;                    // arg_int=1: Input dir with pullup high for I2C data pin
		case U8X8_MSG_GPIO_MENU_SELECT:
				u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
				break;
		case U8X8_MSG_GPIO_MENU_NEXT:
				u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
				break;
		case U8X8_MSG_GPIO_MENU_PREV:
				u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
				break;
		case U8X8_MSG_GPIO_MENU_HOME:
				u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
				break;
		default:
				u8x8_SetGPIOResult(u8x8, 1); // default return value
				break;
	}
	return 1;
}
#endif

void oled_init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = SPI1_CS_Pin | SPI1_SCK_Pin | SPI1_MISO_Pin | SPI1_MOSI_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = PWM1_Pin;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

//		OLED_RST_Set();
//		LL_mDelay(100);
//		OLED_RST_Clr();
//		LL_mDelay(100);
//		OLED_RST_Set();
	
//		GPIO_InitStruct.Pin = LCD_SCL_Pin | LCD_SDA_Pin;
//		GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
//		GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//		GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
//		LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//		LL_GPIO_SetOutputPin(GPIOB, LCD_SCL_Pin|LCD_SDA_Pin);
//	I2C1->CR1	= I2C_CR1_SWRST;  //复位I2C控制器
//	I2C1->CR1	= 0;              //解除复位（不会自动清除）

	//	u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_4wire_sw_spi, stm32_gpio_and_delay);
	//	u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, stm32_gpio_and_delay);
	u8g2_Setup_sh1106_i2c_128x64_noname_f(&u8g2, U8G2_R0, u8x8_byte_hw_i2c, stm32_gpio_and_delay);
	u8g2_InitDisplay(&u8g2);
	u8g2_SetPowerSave(&u8g2, 0);
}
