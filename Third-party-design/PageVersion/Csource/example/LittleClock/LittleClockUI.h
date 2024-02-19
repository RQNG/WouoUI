#ifndef __LITTLECLOCKUI_H__
#define __LITTLECLOCKUI_H__

#include "oled_ui.h"
#include "oled_g.h"
#include "oled_port.h"

void LittleClockUI_Init(void);
void LittleClockUI_Proc(void);

#define DATE_UPDATE_MSK     0x01
#define TIME_UPDATE_MSK     0x02
#define ALARM1_UPDATE_MSK   0x04
#define ALARM2_UPDATE_MSK   0x08 //用于检测update flag的置位
#define ALARM1_ENABLE_MSK   0x10
#define ALARM2_ENABLE_MSK   0x20 //用于检测update flag的置位

#define ALARM1_RING_MSK     0x40
#define ALARM2_RING_MSK     0x80 //主函数中置位，在响铃页面检测


#define SET_FLAG(flag, msk)     (flag|=msk)
#define FLAG_IS_SET(flag, msk)  (flag&msk)
#define CLEAR_FLAG(flag, msk)   (flag&= (~msk))

extern uint16_t fps; // 外界需要提供一个变量，统计一秒内UIproc能刷新几次，粗略估计帧率'
extern uint8_t update_flag; //用于主函数中判断是否需要更新rtc芯片的数据
extern DigitalPage calendar_page;
extern DigitalPage clock_page; //用于主函数中，1秒更新一次时间
extern DigitalPage alarm1_page;
extern DigitalPage alarm2_page;
extern RaderPicPage ring_page; 
#endif
