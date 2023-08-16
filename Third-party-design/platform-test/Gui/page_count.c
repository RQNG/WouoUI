#include "page_count.h"

extern u8g2_t u8g2;
uint8_t *count_time;

M_SELECT count_menu[4] =
{
	{"[ Count ]"},
	{"~ Set Time"},
	{"- Reset Count"},
	{"- Start Count"},
};
		
struct
{
  float y;
  float y_trg;
} show_count_data;

// 页初始化
void count_param_init(void)
{
	count_time = 	ir_sw_get_time_point();
  check_box_v_init(count_time);
}

// 页初始化
void count_show_init(void)
{
	show_count_data.y = 64;
	show_count_data.y_trg = 0;
	ir_sw_init();
}

void confirm_callback(int par)
{
	if(par)
	{
		ir_sw_set_count(0);
		eeprom_notify_change();
	}
}

void count_show(void)
{
  animation(&show_count_data.y, &show_count_data.y_trg, WIN_ANI);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawRFrame(&u8g2, 0, (int16_t)show_count_data.y, DISP_W, 18, 2);	// 绘制外框描边
	u8g2_DrawLine(&u8g2, DISP_W/2, (int16_t)show_count_data.y, DISP_W/2, (int16_t)show_count_data.y+17);	// 绘制外框描边
  u8g2_SetFont(&u8g2, LIST_FONT);
	if(ir_sw_get_out_state())
		u8g2_DrawStr(&u8g2, 0+2, (int16_t)show_count_data.y + 13, "STATE:		ON");
	else
		u8g2_DrawStr(&u8g2, 0+2, (int16_t)show_count_data.y + 13, "STATE:		OFF");
	u8g2_DrawStr(&u8g2, 66, (int16_t)show_count_data.y + 13, "TIME:"); 										// 绘制标题
	u8g2_DrawStr(&u8g2, 95, (int16_t)show_count_data.y + 13, itoa(ir_sw_get_now_time())); 										// 绘制标题

  u8g2_SetFont(&u8g2, u8g2_font_logisoso42_tr);				//使用大字体
	u8g2_DrawStr(&u8g2, DISP_W/2 - (u8g2_GetStrWidth(&u8g2, itoa(ir_sw_get_count()))/2), (int16_t)show_count_data.y + 64, itoa(ir_sw_get_count()));
	
  // 反转屏幕内元素颜色，白天模式遮罩
  u8g2_SetDrawColor(&u8g2, 2);
  if (!ui.param[DARK_MODE])
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
}

void count_show_proc(void)
{
  count_show();
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
			case BTN_ID_CW:
			case BTN_ID_CC:
				list_rotate_switch();
				break;
			case BTN_ID_SP:
			case BTN_ID_LP:
				ir_sw_deinit();
				ui.index = M_COUNT;
				ui.state = S_LAYER_OUT;
				break;
    }
  }
}

void count_proc(void)
{
  list_show(count_menu, M_COUNT);
  if (btn.pressed)
	{ 
		btn.pressed = false;
		switch (btn.id) 
		{
			case BTN_ID_CW: 
			case BTN_ID_CC: list_rotate_switch();break; 
			case BTN_ID_LP: ui.select[ui.layer] = 0; 
			case BTN_ID_SP: {
				switch (ui.select[ui.layer]) 
				{
					case 0:ui.index = M_MAIN;ui.state = S_LAYER_OUT;break;
					//弹出窗口，参数初始化：	标题，参数名，参数值，最大值，最小值，步长，背景列表名，背景列表标签
					case 1:	window_value_init("Time", 0, count_time, 255,  1,  5, count_menu, M_COUNT);break;
					//确认窗口，参数初始化：					标题，默认选择按钮，确认回调函数，背景列表名，背景列表标签
					case 2:	confirm_window_value_init("Count", 0, confirm_callback, count_menu, M_COUNT);break;
					case 3:	ui.index = M_COUNT_SHOW;ui.state = S_LAYER_IN;break;
				}
			};break;
    }
  }
}