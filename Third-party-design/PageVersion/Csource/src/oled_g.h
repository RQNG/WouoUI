#ifndef __OLED_G_H__
#define __OLED_G_H__


#include "oled_port.h"

#define OLED_WIDTH  128
#define OLED_HEIGHT 64

typedef struct 
{   
    int16_t start_x;
    int16_t start_y;
    int16_t w;
    int16_t h;
} window;


typedef enum
{
    RD_LEFT_UP = 0x00, //射线从左上角出发
    RD_LEFT_DOWN,      //射线从左下角出发
    RD_RIGHT_UP,       //射线从右上角出发
    RD_RIGHT_DOWN,	   //射线从右下角出发
    RD_RIGHT,		   //射线从水平向右
    RD_LEFT, 		   //射线从水平向左
} RaderDirection;


void OLED_ClearBuff(void);
void OLED_SetPointColor(uint8_t color);
void OLED_RefreshBuff(void);


int16_t OLED_WinDrawASCII(window *win,int16_t x, int16_t y ,uint8_t size, char c);
void OLED_WinDrawStr(window *win,int16_t x, int16_t y ,uint8_t size, uint8_t* str);
void OLED_WinDrawVLine(window *win,int16_t x, int16_t y_start, int16_t y_end);
void OLED_WinDrawHLine(window * win, int16_t x_start, int16_t x_end, int16_t y);
void OLED_WinDrawRBox(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r);
void OLED_WinDrawRBoxEmpty(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r);
void OLED_WinDrawBMP(window * win, int16_t x, int16_t y, int16_t width, int16_t height,const uint8_t * BMP, uint8_t color);
void OLED_WinDrawPoint(window * win, int16_t x, int16_t y);
void OLED_WinDrawLine(window* win,int16_t x1, int16_t y1, int16_t x2, int16_t y2);
uint8_t OLED_GetStrWidth(const char * str, uint8_t size);
void OLED_Animation(float *a, float *a_trg, float n);
void OLED_AllSrcFade(uint8_t Odd0OrEven1,uint8_t byte);
uint8_t OLED_WinDrawRaderPic(window * win,const uint8_t* pic,int16_t start_x, uint16_t start_y, uint8_t width, uint8_t height, RaderDirection direct, uint8_t enable_all_point_scan, uint8_t scan_rate);
#endif
