#include "mylib.h"

#define IS_BIT_SET(REG, BIT)         (((REG) & (BIT)) != 0U)
#define IS_BIT_CLR(REG, BIT)         (((REG) & (BIT)) == 0U)

int32_t not_int32(int32_t n)
{                 
	return (~n + 1); 
}

float map_number(float num, float minInput, float maxInput, float minOutput, float maxOutput)
{
//		 = 0.0;    	// 输入范围的最小值
//		 = 4096.0; 	// 输入范围的最大值
//		 = 0.0;   	// 输出范围的最小值
//		 = 32.0;  	// 输出范围的最大值
    // 按比例映射
    float mappedNum = ((num - minInput) / (maxInput - minInput)) * (maxOutput - minOutput) + minOutput;
    return mappedNum;
}

void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	LL_USART_TransmitData8(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (LL_USART_IsActiveFlag_TXE(pUSARTx) == RESET);		//单缓冲区使用TXE标志检测
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(LL_USART_IsActiveFlag_TC(pUSARTx)==RESET);		//多缓冲区使用TC标志检测
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(LL_USART_IsActiveFlag_TC(pUSARTx)==RESET);
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	LL_USART_TransmitData8(pUSARTx,temp_h);	
	while (LL_USART_IsActiveFlag_TXE(pUSARTx) == RESET);
	
	/* 发送低八位 */
	LL_USART_TransmitData8(pUSARTx,temp_l);	
	while (LL_USART_IsActiveFlag_TXE(pUSARTx) == RESET);
}

int fputc(int c,FILE *stream)
{	   
  LL_USART_TransmitData8(USART3,c);
  while(!LL_USART_IsActiveFlag_TXE(USART3))__NOP();
  LL_USART_ClearFlag_TC(USART3); 
  return c;
}

int fgetc(FILE *stream)
{
  while(!LL_USART_IsActiveFlag_RXNE(USART3)) __NOP();
  return ((char)LL_USART_ReceiveData8(USART3));
}


//软件复位
void SoftReset(void)
{
	__set_FAULTMASK(1); // 关闭所有中断
	NVIC_SystemReset(); // 复位
}

//设置栈顶地址
//addr:栈顶地址
//__asm void MSR_MSP(uint32_t addr)
//{
////	MSR MSP, r0 			//set Main Stack value
////	BX r14
//}

