#include "oled_port.h"
#include "string.h"
#include "air001xx.h"
#include "air001xx_ll_spi.h"
#include "air001xx_ll_bus.h"
#include "air001xx_ll_gpio.h"
#include "air001xx_ll_utils.h"

#define OLED_CMD 	0
#define OLED_DATA 	1

#define RES_PORT GPIOA
#define RES_PIN  LL_GPIO_PIN_4
#define DC_PORT  GPIOA
#define DC_PIN   LL_GPIO_PIN_6
#define SCK_PORT GPIOA
#define SCK_PIN  LL_GPIO_PIN_5
#define SDI_PORT GPIOA
#define SDI_PIN  LL_GPIO_PIN_7

__STATIC_INLINE void OLED_RES_Set(void)
{
  WRITE_REG(RES_PORT->BSRR, READ_REG(RES_PORT->BSRR) | RES_PIN);
}
__STATIC_INLINE void OLED_DC_Set(void)
{
  WRITE_REG(DC_PORT->BSRR, READ_REG(DC_PORT->BSRR) | DC_PIN);
}
__STATIC_INLINE void OLED_RES_Clr(void)
{
  WRITE_REG(RES_PORT->BRR, READ_REG(RES_PORT->BRR) | RES_PIN);
}
__STATIC_INLINE void OLED_DC_Clr(void)
{
  WRITE_REG(DC_PORT->BRR, READ_REG(DC_PORT->BRR) | DC_PIN);
}


static void OLED_SPIRstDCPinInit(void)
{
    LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA); //使能GPIOA的时钟
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = RES_PIN|DC_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    // GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    // GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;  //输出推挽就可以
    LL_GPIO_Init(RES_PORT,&GPIO_InitStruct);

    GPIO_InitStruct.Pin = SCK_PIN|SDI_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    // GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;  //默认初始化为0，就是NO
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init(SCK_PORT,&GPIO_InitStruct);
}


static void OLED_WriteByte(uint8_t dat, uint8_t cORd)
{
    if(cORd) OLED_DC_Set();
    else OLED_DC_Clr();
    LL_SPI_TransmitData8(SPI1,dat);
    while(!LL_SPI_IsActiveFlag_TXE(SPI1))OLED_LOG("hw spi send fail!");
    OLED_DC_Set();
}

static void OLED_WriteByteArrayData(uint8_t * data_array,uint16_t len)
{
    OLED_DC_Set();
    for(uint8_t i = 0; i < len; i++)
    {
        LL_SPI_TransmitData8(SPI1,data_array[i]);
        while(!LL_SPI_IsActiveFlag_TXE(SPI1))OLED_LOG("HW SPI SEND FAIL");
    }
}


void OLED_Init(void)
{
		LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SPI1);
    LL_SPI_InitTypeDef SPI_InitStruct = {0};
    // LL_SPI_StructInit(&SPI_InitStruct); //默认初始化
    // //全双工、从机、8bit、时钟极性低、第一个时钟边沿采数据、硬件NSS、波特率2分频、MSB、normal speed
    // SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX; //全双工
    // SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT; //8bit
    // SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST; //MSB     //这三个参数在初始化是就是0，设置也是0，因此不需要重复设置
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER; //主机
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH; //极性高
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE; //第2个时钟延采样
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT; //软件NSS
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4; //4分频
    LL_SPI_Init(SPI1,&SPI_InitStruct);
    LL_SPI_Enable(SPI1);

    OLED_SPIRstDCPinInit(); 
    LL_mDelay(300);
    OLED_RES_Clr();
    LL_mDelay(300);
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
