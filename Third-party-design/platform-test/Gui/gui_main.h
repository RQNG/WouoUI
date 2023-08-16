#ifndef	__GUI_MAIN_H
#define	__GUI_MAIN_H

#include "main.h"
#include "stdbool.h"
#include "string.h"
#include "math.h"

//总目录，缩进表示页面层级
enum 
{
  M_WINDOW,
	M_CONFIRM_WINDOW,
  M_SLEEP,
    M_MAIN, 
      M_EDITOR,
        M_KNOB,
          M_KRF,
          M_KPF,
      M_VOLT,
			M_COUNT,
			M_COUNT_SHOW,
			M_RANGING,
      M_SETTING,
        M_ABOUT,
};

//状态，初始化标签
enum
{
  S_FADE,       				//转场动画
  S_WINDOW,     				//弹窗初始化
	S_CONFIRM_WINDOW,     //弹窗初始化
  S_LAYER_IN,   				//层级初始化
  S_LAYER_OUT,  				//层级初始化
  S_NONE,       				//直接选择页面
};

// 按键ID
enum
{
  BTN_ID_CC = 0, // 逆时针旋转
	BTN_ID_CW = 1, // 顺时针旋转
	BTN_ID_SP = 2, // 短按
	BTN_ID_LP = 3, // 长按
};

// 按键变量
#define BTN_PARAM_TIMES 2 // 由于uint8_t最大值可能不够，但它存储起来方便，这里放大两倍使用

//按键信息
typedef struct
{
  uint8_t id;
  bool pressed;
}M_BTN_INFO;


//菜单结构体
typedef struct MENU
{
  char *m_select;
} M_SELECT;

/************************************* 页面变量 *************************************/

//OLED变量
#define   DISP_H              64    //屏幕高度
#define   DISP_W              128   //屏幕宽度

//UI变量
#define   UI_DEPTH            20    //最深层级数
#define   UI_MNUMB            50   //菜单数量

// 列表变量
// 默认参数

#define LIST_FONT u8g2_font_HelvetiPixel_tr // 列表字体
#define LIST_TEXT_H 8                       // 列表每行文字字体的高度
#define LIST_LINE_H 16                      // 列表单行高度
#define LIST_TEXT_S 4                       // 列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define LIST_BAR_W 5                        // 列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define LIST_BOX_R 0.5                      // 列表选择框圆角

// 超窄行高度测试
/*
#define LIST_FONT u8g2_font_4x6_tr // 列表字体
#define LIST_TEXT_H 5              // 列表每行文字字体的高度
#define LIST_LINE_H 7              // 列表单行高度
#define LIST_TEXT_S 1              // 列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define LIST_BAR_W 7               // 列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define LIST_BOX_R 0.5             // 列表选择框圆角
*/

enum 
{
  DISP_BRI,     //屏幕亮度
  TILE_ANI,     //磁贴动画速度
  LIST_ANI,     //列表动画速度
  WIN_ANI,      //弹窗动画速度
  SPOT_ANI,     //聚光动画速度
  TAG_ANI,      //标签动画速度
  FADE_ANI,     //消失动画速度
  BTN_SPT,      //按键短按时长
  BTN_LPT,      //按键长按时长
  TILE_UFD,     //磁贴图标从头展开开关
  LIST_UFD,     //菜单列表从头展开开关
  TILE_LOOP,    //磁贴图标循环模式开关
  LIST_LOOP,    //菜单列表循环模式开关
  WIN_BOK,      //弹窗背景虚化开关
  KNOB_DIR,     //旋钮方向切换开关
  DARK_MODE,    //黑暗模式开关
	UI_PARAM,			//16//参数数量
};

typedef struct
{
  bool init;
  uint8_t num[UI_MNUMB];
  uint8_t select[UI_DEPTH];
  uint8_t layer;
  uint8_t index; // = M_SLEEP;
  uint8_t state; // = S_NONE;
  bool sleep;    // = true;
  uint8_t fade;  // = 1;
  uint8_t param[UI_PARAM];
} M_UI;

typedef struct 
{
  float   title_y_calc; 		//= TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H * 2;
  float   title_y_trg_calc; //= TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H;
  int16_t temp;
  bool    select_flag;
  float   icon_x;
  float   icon_x_trg;
  float   icon_y;
  float   icon_y_trg;
  float   indi_x; 
  float   indi_x_trg;
  float   title_y;
  float   title_y_trg;
} M_TILE;

//磁贴变量
//所有磁贴页面都使用同一套参数
#define   TILE_B_FONT         u8g2_font_helvB18_tr        //磁贴大标题字体
#define   TILE_B_TITLE_H      18                          //磁贴大标题字体高度
#define   TILE_ICON_H         30                          //磁贴图标高度
#define   TILE_ICON_W         30                          //磁贴图标宽度
#define   TILE_ICON_S         36                          //磁贴图标间距
#define   TILE_INDI_H         27                          //磁贴大标题指示器高度
#define   TILE_INDI_W         7                           //磁贴大标题指示器宽度
#define   TILE_INDI_S         36                          //磁贴大标题指示器上边距


typedef void (*fun_callback_t)(int number);//声明回调函数指针

extern M_BTN_INFO btn;
extern M_UI ui;

char *itoa(uint32_t num);
char *ftoa(float num);
//通知需要存储参数
void eeprom_notify_change(void);
// 动画函数
void animation(float *a, float *a_trg, uint8_t n);
// 显示数值的初始化
void check_box_v_init(uint8_t *param);
// 列表类页面通用显示函数
void list_show(M_SELECT arr[], uint8_t ui_index);
// 列表类页面旋转时判断通用函数
void list_rotate_switch();
// 弹窗数值初始化
void window_value_init(char title[], uint8_t select, uint8_t *value, uint8_t max, uint8_t min, uint8_t step, M_SELECT *bg, uint8_t index);
// 确认弹窗数值初始化
void confirm_window_value_init(char title[], uint8_t _select_btn, fun_callback_t _cb, M_SELECT *bg, uint8_t index);
void gui_btn_send_signal(uint8_t btn_id,uint8_t event);
void gui_task(void);
void gui_init(void);



#endif		/******		__GUI_MAIN_H		******/
