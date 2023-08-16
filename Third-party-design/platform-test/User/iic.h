#ifndef	__IIC_H
#define	__IIC_H

#include "main.h"

#define HIGH	1
#define LOW		0

#define IIC_SDA(N)	(N? (LL_GPIO_SetOutputPin(LCD_SDA_GPIO_Port,LCD_SDA_Pin)) : (LL_GPIO_ResetOutputPin(LCD_SDA_GPIO_Port,LCD_SDA_Pin)))
#define IIC_SCL(N)	(N? (LL_GPIO_SetOutputPin(LCD_SCL_GPIO_Port,LCD_SCL_Pin)) : (LL_GPIO_ResetOutputPin(LCD_SCL_GPIO_Port,LCD_SCL_Pin)))

//#define OLED_SCLK_Clr() LL_GPIO_ResetOutputPin(LCD_SCL_GPIO_Port,LCD_SCL_Pin)//SCL IIC接口的时钟信号
//#define OLED_SCLK_Set() LL_GPIO_SetOutputPin(LCD_SCL_GPIO_Port,LCD_SCL_Pin)

//#define OLED_SDIN_Clr() LL_GPIO_ResetOutputPin(LCD_SDA_GPIO_Port,LCD_SDA_Pin)//SDA IIC接口的数据信号
//#define OLED_SDIN_Set() LL_GPIO_SetOutputPin(LCD_SDA_GPIO_Port,LCD_SDA_Pin)




#define READ_SDA		LL_GPIO_IsInputPinSet(LCD_SDA_GPIO_Port,LCD_SDA_Pin)

void IIC_Init(void);
void IIC_Start(void);
void IIC_Stop(void);
uint8_t	IIC_Wait_Ack(void);
void IIC_Ack(void);
void IIC_NAck(void);
uint8_t IIC_Read_Byte(uint8_t Ack);
void IIC_Send_Byte(uint8_t Txd);
void IIC_Delay(void);


void SDA_OUT(void);
void SDA_IN(void);



#endif		/******		__BSP_IIC_H		******/
