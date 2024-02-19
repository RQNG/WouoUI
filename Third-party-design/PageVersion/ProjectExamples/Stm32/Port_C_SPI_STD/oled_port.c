#include "oled_port.h"
#include "string.h"
#include "stm32f10x.h"
#include "main.h"

#define  SCL_PORT   GPIOA
#define  SCL_PIN	GPIO_Pin_5
#define  SDA_PORT   GPIOA
#define	 SDA_PIN	GPIO_Pin_7 
#define  RES_PORT   GPIOB
#define  RES_PIN    GPIO_Pin_0
#define  DC_PORT    GPIOB
#define  DC_PIN     GPIO_Pin_1

#define OLED_CMD 	0
#define OLED_DATA 	1

void delay(uint8_t i)
{
    i = i*5;
    while(i--){}
}

static void OLED_RES_Set(void)
{
  GPIO_WriteBit(RES_PORT, RES_PIN, Bit_SET);
}
static void OLED_DC_Set(void)
{
  GPIO_WriteBit(DC_PORT , DC_PIN, Bit_SET);
}
static void OLED_RES_Clr(void)
{
  GPIO_WriteBit(RES_PORT, RES_PIN, Bit_RESET);
}
static void OLED_DC_Clr(void)
{
  GPIO_WriteBit(DC_PORT, DC_PIN, Bit_RESET);
}


static void OLED_SPIRstDCPinInit(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//GPIOB
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin= DC_PIN|RES_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(DC_PORT, &GPIO_InitStructure);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//GPIOA
  GPIO_InitStructure.GPIO_Pin = SCL_PIN | SDA_PIN ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(SCL_PORT, &GPIO_InitStructure);
}


static void OLED_WriteByte(uint8_t dat, uint8_t cORd)
{
    if(cORd) OLED_DC_Set();
    else OLED_DC_Clr();
    /* Loop while DR register in not emplty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send byte through the SPI1 peripheral */
    SPI_I2S_SendData(SPI1, dat);
    delay(5);
    OLED_DC_Set();
}

static void OLED_WriteByteArrayData(uint8_t * data_array,uint16_t len)
{
    OLED_DC_Set();
    for(uint8_t i = 0; i < len; i++)
    {
        /* Loop while DR register in not emplty */
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
        /* Send byte through the SPI1 peripheral */
        SPI_I2S_SendData(SPI1, data_array[i]);
        delay(5);
    }
}


void OLED_Init(void)
{   
    OLED_SPIRstDCPinInit(); 
    
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
    SPI_InitTypeDef  SPI_InitStructure;
    SPI_SSOutputCmd(SPI1, ENABLE);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE);
    
    delay_ms(300);
    OLED_RES_Clr();
    delay_ms(300);
    OLED_RES_Set();

    OLED_WriteByte(0xAE,OLED_CMD);//--turn off oled panel
    OLED_WriteByte(0x00,OLED_CMD);//---set low column address
    OLED_WriteByte(0x10,OLED_CMD);//---set high column address
    OLED_WriteByte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_WriteByte(0x81,OLED_CMD);//--set contrast control register
    OLED_WriteByte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
    OLED_WriteByte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_WriteByte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_WriteByte(0xA6,OLED_CMD);//--set normal display
    OLED_WriteByte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
    OLED_WriteByte(0x3f,OLED_CMD);//--1/64 duty
    OLED_WriteByte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_WriteByte(0x00,OLED_CMD);//-not offset
    OLED_WriteByte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
    OLED_WriteByte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_WriteByte(0xD9,OLED_CMD);//--set pre-charge period
    OLED_WriteByte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_WriteByte(0xDA,OLED_CMD);//--set com pins hardware configuration
    OLED_WriteByte(0x12,OLED_CMD);
    OLED_WriteByte(0xDB,OLED_CMD);//--set vcomh
    OLED_WriteByte(0x40,OLED_CMD);//Set VCOM Deselect Level
    OLED_WriteByte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_WriteByte(0x02,OLED_CMD);//
    OLED_WriteByte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
    OLED_WriteByte(0x14,OLED_CMD);//--set(0x10) disable
    OLED_WriteByte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
    OLED_WriteByte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
    //OLED_SPIFill(0x00);
    OLED_WriteByte(0xAF,OLED_CMD);
}


void OLED_SendBuff(uint8_t buff[8][128])
{		    
	for(uint8_t i=0;i<8;i++)  
	{  
		OLED_WriteByte (0xb0+i,OLED_CMD);    //设置页地址（0~7）(b0-b7)
		OLED_WriteByte (0x00,OLED_CMD);      //设置显示位置—列低地址
		OLED_WriteByte (0x10,OLED_CMD);      //设置显示位置—列高地址   
		OLED_WriteByteArrayData(buff[i],128); //写一页128个字符
	} //更新显示
}
