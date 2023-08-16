#ifndef __MYLIB_H
#define __MYLIB_H
#include "main.h"
#include "stdio.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"

#define DEBUG     1
#ifdef DEBUG
//	#define LOG_INFO(format, ...) printf("FILE: "__FILE__",FUNC: %s, LINE: %d MSG: "format"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
	#define LOG_INFO(format, ...) printf(format,##__VA_ARGS__)
#else
  #define LOG_INFO(format,...)   NULL
#endif


int32_t not_int32(int32_t n);
float map_number(float num, float minInput, float maxInput, float minOutput, float maxOutput);
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch);
void Usart_SendString( USART_TypeDef * pUSARTx, char *str);
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num);
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch);

int fputc(int c,FILE *stream);
int fgetc(FILE *stream);

void SoftReset(void);
void MSR_MSP(uint32_t addr);

#endif /* __MYLIB_H*/
