#include "iic.h"
//*****************************************************************/
//起始信号
void IIC_Start(void)
{
	SDA_OUT();
	IIC_SDA(HIGH);
	IIC_SCL(HIGH);
	IIC_Delay();
	IIC_SDA(LOW);
	IIC_Delay();
	IIC_SCL(LOW);
}

/*****************************************************************/
//停止信号
void IIC_Stop(void)
{
	SDA_OUT();
	IIC_SCL(LOW);
	IIC_SDA(LOW);
	IIC_Delay();
	IIC_SCL(HIGH);
	IIC_SDA(HIGH);
	IIC_Delay();
}

/*****************************************************************/
//等待答应信号
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t	IIC_Wait_Ack(void)
{
	uint8_t Time = 0;

	SDA_IN();
	IIC_SDA(HIGH);
	IIC_Delay();
	IIC_SCL(HIGH);
	IIC_Delay();
	while(READ_SDA)
	{
		Time++;
		IIC_Delay();
		if(Time>250)
		{
			SDA_OUT();
			IIC_Stop();
			return 1;
		}
	}
	SDA_OUT();
	IIC_SCL(LOW);
	return 0;
}

/***************************************************************/
//产生答应信号
void IIC_Ack(void)
{
	IIC_SCL(LOW);
	SDA_OUT();
	IIC_SDA(LOW);
	IIC_Delay();
	IIC_SCL(HIGH);
	IIC_Delay();
	IIC_SCL(LOW);
}

/**************************************************************/
//不产生答应信号
void IIC_NAck(void)
{
	IIC_SCL(LOW);
	SDA_OUT();
	IIC_SDA(HIGH);
	IIC_Delay();
	IIC_SCL(HIGH);
	IIC_Delay();
	IIC_SCL(LOW);
}

/*************************************************************/
//发送一个字节
void IIC_Send_Byte(uint8_t Txd)
{
	uint8_t t;
	SDA_OUT();
	IIC_SCL(LOW);
	for(t = 0;t<8;t++)
	{
		IIC_SDA((Txd&0x80)>>7);
		Txd<<=1;
		IIC_SCL(HIGH);
		IIC_Delay();
		IIC_SCL(LOW);
		IIC_Delay();
	}
}

/**************************************************************/
//读取一个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t IIC_Read_Byte(uint8_t Ack)
{
	uint8_t i,receive = 0;
	SDA_IN();
	for(i = 0;i<8;i++)
	{
		IIC_SCL(LOW);
		IIC_Delay();
		IIC_SCL(HIGH);
		IIC_Delay();
		IIC_Delay();
		receive<<=1;
		if(READ_SDA)
		{
			receive++;
		}
		IIC_Delay();
	}
	SDA_OUT();
	if(!Ack)
		IIC_NAck();
	else
		IIC_Ack();
	return receive;
}

/**************************************************************/
//IIC延时
void IIC_Delay(void)
{
	//单周期指令 1/72M=0.01389us
//	co_delay_10us(1);
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
//	__NOP();__NOP();__NOP();__NOP();__NOP();
	LL_mDelay(1);
}


void SDA_OUT(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(LCD_SCL_GPIO_Port, &GPIO_InitStruct);
}

void SDA_IN(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(LCD_SCL_GPIO_Port, &GPIO_InitStruct);
}

//初始化IIC
void IIC_Init(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_GPIOB);
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LCD_SCL_Pin|LCD_SDA_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  LL_GPIO_Init(LCD_SCL_GPIO_Port, &GPIO_InitStruct);
}



