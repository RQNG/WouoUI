#include "page_ranging.h"

extern u8g2_t u8g2;
float ranging_show_y = 0;
float ranging_show_y_trg = 0;

char tip[10];
unsigned char ranging_state = RANG_WAIT;

// 页初始化
void ranging_param_init(void)
{
	ranging_init();
	ranging_show_y = 0;
	ranging_show_y_trg = 32;
	ranging_state = RANG_WAIT;
	sprintf(tip, "Wait..");
}

void ranging_show(void)
{
  animation(&ranging_show_y, &ranging_show_y_trg, WIN_ANI);
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_SetFont(&u8g2, TILE_B_FONT);				//使用大字体
	if(ranging_state == RANG_PASS)
		sprintf(tip, "Pass");
	else if(ranging_state == RANG_FAIL)
		sprintf(tip, "Fail");
	else if(ranging_state == RANG_ERROR)
		sprintf(tip, "Error");
	else if(ranging_state == RANG_NORMAL)
		sprintf(tip, "Warn");
	u8g2_DrawStr(&u8g2, DISP_W/2 - (u8g2_GetStrWidth(&u8g2, tip)/2), (int16_t)ranging_show_y, tip);
	if(ranging_state == RANG_PASS || ranging_state == RANG_ERROR || ranging_state == RANG_NORMAL)
	{
		u8g2_DrawStr(&u8g2, DISP_W/2 - (u8g2_GetStrWidth(&u8g2, ftoa(ranging_get_length()))/2), (int16_t)ranging_show_y+32, ftoa(ranging_get_length()));
	}
	
  // 反转屏幕内元素颜色，白天模式遮罩
  u8g2_SetDrawColor(&u8g2, 2);
  if (!ui.param[DARK_MODE])
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
}

void ranging_proc(void)
{
	ranging_show();
  if (btn.pressed)
	{ 
		btn.pressed = false;
		switch (btn.id) 
		{
			case BTN_ID_CW: 
			case BTN_ID_CC: ranging_state = 0;ranging_state = ranging_start();break; 
			case BTN_ID_LP: ui.select[ui.layer] = 0; 
			case BTN_ID_SP: {
				switch (ui.select[ui.layer]) 
				{
					case 0:ranging_deinit();ui.index = M_MAIN;ui.state = S_LAYER_OUT;break;
				}
			};break;
    }
  }
}