#ifndef __OLED_CONF_H__
#define __OLED_CONF_H__

#define UI_CONWIN_ENABLE            1  //是否使能 以"$ " 为开头的选项使用确认弹窗
#define UI_MAX_PAGE_NUM             32 //页面的最大数量，这个数字需要大于所有页面的page_id
#define UI_INPUT_MSG_QUNEE_SIZE     4  //ui内部消息对列的大小(至少需要是2)

//页面类型使能宏，使用对应的页面类型，则需要开启该宏，将宏置为1，默认都开启
#define PAGE_WAVE_ENABLE        1
#define PAGE_RADIO_ENABLE       1
#define PAGE_RADERPIC_ENABLE    1
#define PAGE_DIGITAL_ENABLE     1

#endif
