#include "gui_main.h"
#include "page_count.h"
#include "page_ranging.h"

u8g2_t u8g2;

/*
  此项目模仿自稚晖君暂未开源的 MonoUI，用于实现类似 UltraLink 的丝滑界面

  有四个版本：

    * 分辨率 128 * 128 ：主菜单，列表和关于本机页基本还原了 UltraLink 的界面和动态效果，电压测量页为原创设计
    * 分辨率 128 * 64 ：主菜单模仿 UltraLink 重新设计，去掉了小标题，电压测量页重新设计，关于本机页面改为列表，列表适配了该分辨率
    * 分辨率 128 * 32 ：在 128 * 64 分辨率的基础上，主界面只保留图标，电压测量页重新设计
    * 通用版本：仅保留列表，主菜单也改为列表，删除电压测量页和与列表无关的动画（保留弹窗效果），经过简单修改可以适配任何分辨率，任何行高度的情况

  WouoUI v2.2 更新内容：

    * 优化动画函数，动画结束后不会再进行无意义的浮点数计算

  WouoUI v2 功能：

    * 全部使用非线性的平滑缓动动画，包括列表，弹窗，甚至进度条
    * 优化平滑动画算法到只有两行，分类别定义平滑权重，并且每个权重值都分别可调
    * 可以打断的非线性动画，当前动画未结束但下一次动画已经被触发时，动画可以自然过渡
    * 非通用版本分别适配了类似 UltraLink 主菜单的磁贴界面（因为让我想起 WP7 的 Metron 风格，所以称之为磁贴）
    * 通用版本仅保留列表类界面，经过简单修改可以适配所有分辨率的屏幕，包括屏幕内行数不是整数的情况
    * 列表菜单，列表可以无限延长
    * 列表文字选择框，选择框可根据选择的字符串长度自动伸缩，进入菜单时从列表开头从长度 0 展开，转到上一级列表时，长度和纵坐标平滑移动到上一级选择的位置
    * 列表单选框，储存数据时也储存该值在列表中所在的位置，展开列表时根据每行开头的字符判断是否绘制外框，再根据位置数据判断是否绘制里面的点
    * 列表多选框，储存数据的数组跟多选框列表的行数对应，不要求连续排列，展开列表时根据每行开头的字符判断是否绘制外框，再根据行数对应的储存数据位置的数值是否为1判断是否绘制里面的点
    * 列表显示数值，与多选框原理相似，但不涉及修改操作
    * 列表展开动画，初始化列表时，可以选择列表从头开始展开，或者从上次选中的位置展开
    * 图标展开动画，初始化磁贴类界面时，可以选择图标从头开始展开，或者从上次选中的位置展开
    * 弹出窗口，实现了窗口弹出的动画效果，可以自定义最大值，最小值，步进值，需要修改的参数等，窗口独立运行，调用非常简单
    * 弹出窗口背景虚化可选项，背景虚化会产生卡顿感，但删掉代码有些可惜，因此做成可选项，默认关闭
    * 亮度调节，在弹出窗口中调节亮度值可以实时改变当前亮度值
    * 旋钮功能，使用EC11旋钮控制，旋钮方向可以软件调整，内置一个USB控制电脑的示例，在睡眠模式下旋转调整音量或者亮度，短按输入一个键值，长按进入主菜单，旋钮消抖时长等参数可以在弹出窗口中调整
    * 循环模式，选择项超过头尾时，选择项跳转到另一侧继续，列表和磁贴类可以分别选择
    * 黑暗模式，其实本来就是黑暗模式，是增加了白天模式，默认开启黑暗模式
    * 消失动画适配两种模式，一种是渐变成全黑，另一种渐变成全白
    * 断电存储，用简单直观的方式将每种功能参数写入EEPROM，只在修改过参数，进入睡眠模式时写入，避免重复擦写，初始化时检查11个标志位，允许一位误码

  项目参考：

    * B站：路徍要什么自行车：在线仿真：https://wokwi.com/projects/350306511434547796，https://www.bilibili.com/video/BV1HA411S7pv/
    * Github：createskyblue：OpenT12：https://github.com/createskyblue/OpenT12
    * Github：peng-zhihui：OpenHeat：https://github.com/peng-zhihui/OpenHeat

  注意事项：

    * 为防止使用者在上传程序后无法直接使用，认为是代码有问题，HID功能默认禁用，如需使用旋钮音量控制和点按输入功能，请在初始化函数中启用相关功能，上传成功后设置两个跳线帽都为0，断电再插上 USB 线

  本项目不设置开源协议，如需商用或借鉴，在醒目处标注本项目开源地址即可
  欢迎关注我的B站账号，用户名：音游玩的人，B站主页：https://space.bilibili.com/9182439?spm_id_from=..0.0
*/

///************************************* 页面变量 *************************************/
struct
{
  uint8_t line_n; // = DISP_H / LIST_LINE_H;
  int16_t temp;
  bool loop;
  float y;
  float y_trg;
  float box_x;
  float box_x_trg;
  float box_y;
  float box_y_trg[UI_DEPTH];
  float bar_y;
  float bar_y_trg;
} list;


//电压测量页面变量
//开发板模拟引脚
//曲线相关
#define   WAVE_SAMPLE         1                          //采集倍数
#define   WAVE_W              94                          //波形宽度
#define   WAVE_L              24                          //波形左边距
#define   WAVE_U              0                           //波形上边距
#define   WAVE_MAX            27                          //最大值
#define   WAVE_MIN            4                           //最小值
#define   WAVE_BOX_H          32                          //波形边框高度
#define   WAVE_BOX_W          94                          //波形边框宽度
#define   WAVE_BOX_L_S        24                          //波形边框左边距
//列表和文字背景框相关
#define   VOLT_FONT           u8g2_font_helvB18_tr        //电压数字字体
#define   VOLT_TEXT_BG_L_S    24                          //文字背景框左边距
#define   VOLT_TEXT_BG_W      94                          //文字背景框宽度
#define   VOLT_TEXT_BG_H      29                          //文字背景框高度

struct
{
  int     ch0_adc[WAVE_SAMPLE * WAVE_W];
  int     ch0_wave[WAVE_W];
  float   val;
  float   text_bg_r; 
  float   text_bg_r_trg; 
} volt;


// 选择框变量
// 默认参数
#define CHECK_BOX_L_S 95 // 选择框在每行的左边距
#define CHECK_BOX_U_S 2  // 选择框在每行的上边距
#define CHECK_BOX_F_W 12 // 选择框外框宽度
#define CHECK_BOX_F_H 12 // 选择框外框高度
#define CHECK_BOX_D_S 2  // 选择框里面的点距离外框的边距

// 超窄行高度测试
/*
#define CHECK_BOX_L_S 99 // 选择框在每行的左边距
#define CHECK_BOX_U_S 0  // 选择框在每行的上边距
#define CHECK_BOX_F_W 5  // 选择框外框宽度
#define CHECK_BOX_F_H 5  // 选择框外框高度
#define CHECK_BOX_D_S 1  // 选择框里面的点距离外框的边距
*/

struct
{
  uint8_t *v;
  uint8_t *m;
  uint8_t *s;
  uint8_t *s_p;
} check_box;

// 弹窗变量
#define WIN_FONT u8g2_font_HelvetiPixel_tr // 弹窗字体
#define WIN_H 32                           // 弹窗高度
#define WIN_W 102                          // 弹窗宽度
#define WIN_BAR_W 92                       // 弹窗进度条宽度
#define WIN_BAR_H 7                        // 弹窗进度条高度
#define WIN_Y -WIN_H - 2                   // 弹窗竖直方向出场起始位置
#define WIN_Y_TRG -WIN_H - 2               // 弹窗竖直方向退场终止位置

// 确认弹窗变量
#define CON_WIN_FONT u8g2_font_HelvetiPixel_tr // 弹窗字体
#define CON_WIN_H 35                           // 弹窗高度
#define CON_WIN_W 102                          // 弹窗宽度
#define CON_WIN_Y -CON_WIN_H - 2               // 弹窗竖直方向出场起始位置
#define CON_WIN_Y_TRG -CON_WIN_H - 2           // 弹窗竖直方向退场终止位置

struct
{
  uint8_t *value;
  uint8_t max;
  uint8_t min;
  uint8_t step;

  M_SELECT *bg;
  uint8_t index;
  char title[20];
  uint8_t select;
  uint8_t l; // = (DISP_W - WIN_W) / 2;
  uint8_t u; // = (DISP_H - WIN_H) / 2;
  float bar;
  float bar_trg;
  float y;
  float y_trg;
} win;

struct
{
  uint8_t select_btn;
  uint8_t reset_value;
	fun_callback_t cb;
  M_SELECT *bg;
  uint8_t index;
  char title[10];
  uint8_t l; // = (DISP_W - CONFIRM_WIN_W) / 2;
  uint8_t u; // = (DISP_H - CONFIRM_WIN_H) / 2;
  float y;
  float y_trg;
  float btn_x;
  float btn_x_trg;
} con_win;

// 旋钮功能变量
#define KNOB_PARAM 4
#define KNOB_DISABLE 0
#define KNOB_ROT_VOL 1
#define KNOB_ROT_BRI 2

enum
{
  KNOB_ROT,   // 睡眠下旋转旋钮的功能，0 禁用，1 音量，2 亮度
  KNOB_COD,   // 睡眠下短按旋钮输入的字符码，0 禁用
  KNOB_ROT_P, // 旋转旋钮功能在单选框中选择的位置
  KNOB_COD_P, // 字符码在单选框中选择的位置
};
struct
{
  uint8_t param[KNOB_PARAM]; // = {KNOB_DISABLE, KNOB_DISABLE, 2, 2}; // 禁用在列表的第2个选项，第0个是标题，第1个是分界线
} knob;

uint8_t *buf_ptr; // 指向屏幕缓冲的指针
uint16_t buf_len; // 缓冲长度

M_UI ui;
M_TILE tile;
M_BTN_INFO btn;
uint8_t eeprom_change = false;

/************************************* 定义内容 *************************************/

/************************************* 文字内容 *************************************/

M_SELECT main_menu[] = 
{
  {"Sleep"},
  {"Editor"},
  {"Volt"},
	{"Count"},
	{"Ranging"},
  {"Setting"},
};

M_SELECT editor_menu[] = 
{
  {"[ Editor ]"},
  {"- Function 0"},
  {"- Function 1"},
  {"- Function 2"},
  {"- Function 3"},
  {"- Function 4"},
  {"- Function 5"},
  {"- Function 6"},
  {"- Function 7"},
  {"- Function 8"},
  {"- Function 9"},
  {"- Knob"},
};

M_SELECT knob_menu[] = 
{
  {"[ Knob ]"},
  {"# Rotate Func"},
  {"$ Press Func"},
};

M_SELECT krf_menu[] = 
{
  {"[ Rotate Function ]"},
  {"--------------------------"},
  {"= Disable"},
  {"--------------------------"},
  {"= Volume"},
  {"= Brightness"},
  {"--------------------------"},
};

M_SELECT kpf_menu[] = 
{
  {"[ Press Function ]"},
  {"--------------------------"},
  {"= Disable"},
  {"--------------------------"},
  {"= A"},
  {"= B"},
  {"= C"},
  {"= D"},
  {"= E"},
  {"= F"},
  {"= G"},
  {"= H"},
  {"= I"},
  {"= J"},
  {"= K"},
  {"= L"},
  {"= M"},
  {"= N"},
  {"= O"},
  {"= P"},
  {"= Q"},
  {"= R"},
  {"= S"},
  {"= T"},
  {"= U"},
  {"= V"},
  {"= W"},
  {"= X"},
  {"= Y"},
  {"= Z"},
  {"--------------------------"},
  {"= 0"},
  {"= 1"},
  {"= 2"},
  {"= 3"},
  {"= 4"},
  {"= 5"},
  {"= 6"},
  {"= 7"},
  {"= 8"},
  {"= 9"},
  {"--------------------------"},
  {"= Esc"},
  {"= F1"},
  {"= F2"},
  {"= F3"},
  {"= F4"},
  {"= F5"},
  {"= F6"},
  {"= F7"},
  {"= F8"},
  {"= F9"},
  {"= F10"},
  {"= F11"},
  {"= F12"},
  {"--------------------------"},
  {"= Left Ctrl"},
  {"= Left Shift"},
  {"= Left Alt"},
  {"= Left Win"},
  {"= Right Ctrl"},
  {"= Right Shift"},
  {"= Right Alt"},
  {"= Right Win"},
  {"--------------------------"},
  {"= Caps Lock"},
  {"= Backspace"},
  {"= Return"},
  {"= Insert"},
  {"= Delete"},
  {"= Tab"},
  {"--------------------------"},
  {"= Home"},
  {"= End"},
  {"= Page Up"},
  {"= Page Down"},
  {"--------------------------"},
  {"= Up Arrow"},
  {"= Down Arrow"},
  {"= Left Arrow"},
  {"= Right Arrow"},
  {"--------------------------"},
};

M_SELECT volt_menu[] = 
{
  {"EV"},
  {"EA"},
//  {"A2"},
//  {"A3"},
//  {"A4"},
//  {"A5"},
//  {"A6"},
//  {"A7"},
//  {"B0"},
//  {"B1"},
};

M_SELECT setting_menu[] = 
{
  {"[ Setting ]"},
  {"~ Disp Bri"},
  {"~ Tile Ani"},
  {"~ List Ani"},
  {"~ Win Ani"},
  {"~ Spot Ani"},
  {"~ Tag Ani"},
  {"~ Fade Ani"},
  {"~ Btn SPT"},
  {"~ Btn LPT"},
  {"+ T Ufd Fm Scr"},
  {"+ L Ufd Fm Scr"},
  {"+ T Loop Mode"},
  {"+ L Loop Mode"},
  {"+ Win Bokeh Bg"},
  {"+ Knob Rot Dir"},
  {"+ Dark Mode"},
  {"- [ About ]"},
};

M_SELECT about_menu[] = 
{
  {"[ WouoUI ]"},
  {"- Version: v2.1"},
  {"- Board: STM32F103"},
  {"- Ram: 20k"},
  {"- Flash: 64k"},
  {"- Freq: 72Mhz"},
  {"- Creator: RQNG"},
  {"- Billi UID: 9182439"},  
};

/************************************* 图片内容 *************************************/

const uint8_t main_icon_pic[][120] = 
{
  {
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xF1,0x3F,
    0xFF,0xFF,0xC3,0x3F,0xFF,0xFF,0x87,0x3F,0xFF,0xFF,0x07,0x3F,0xFF,0xFF,0x0F,0x3E,
    0xFF,0xFF,0x0F,0x3E,0xFF,0xFF,0x0F,0x3C,0xFF,0xFF,0x0F,0x3C,0xFF,0xFF,0x0F,0x38,
    0xFF,0xFF,0x0F,0x38,0xFF,0xFF,0x0F,0x38,0xFF,0xFF,0x07,0x38,0xFF,0xFF,0x07,0x38,
    0xFF,0xFF,0x03,0x38,0xF7,0xFF,0x01,0x38,0xE7,0xFF,0x00,0x3C,0x87,0x3F,0x00,0x3C,
    0x0F,0x00,0x00,0x3E,0x0F,0x00,0x00,0x3E,0x1F,0x00,0x00,0x3F,0x3F,0x00,0x80,0x3F,
    0x7F,0x00,0xC0,0x3F,0xFF,0x01,0xF0,0x3F,0xFF,0x07,0xFC,0x3F,0xFF,0xFF,0xFF,0x3F,
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F
  },
  {
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xF9,0xE7,0x3F,
    0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,0xFF,0xF0,0xE7,0x3F,0x7F,0xE0,0xE7,0x3F,
    0x7F,0xE0,0xC3,0x3F,0x7F,0xE0,0xC3,0x3F,0x7F,0xE0,0xC3,0x3F,0x7F,0xE0,0xE7,0x3F,
    0xFF,0xF0,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,
    0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xC3,0x3F,0xFF,0xF9,0x81,0x3F,0xFF,0xF0,0x81,0x3F,
    0xFF,0xF0,0x81,0x3F,0xFF,0xF0,0x81,0x3F,0xFF,0xF9,0x81,0x3F,0xFF,0xF9,0xC3,0x3F,
    0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,0xFF,0xF9,0xE7,0x3F,0xFF,0xFF,0xFF,0x3F,
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F
  },
  {
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xEF,0xFF,0xFF,0x3F,0xC7,0xFF,0xFF,0x3F,
    0xC7,0xF3,0xFF,0x3F,0x83,0xC0,0xFF,0x3F,0xEF,0xCC,0xFF,0x3F,0x6F,0x9E,0xFF,0x3F,
    0x6F,0x9E,0xFF,0x3F,0x2F,0x3F,0xFF,0x3F,0x2F,0x3F,0xFF,0x3F,0x8F,0x7F,0xFE,0x3F,
    0x8F,0x7F,0xFE,0x39,0x8F,0x7F,0xFE,0x39,0xCF,0xFF,0xFC,0x3C,0xCF,0xFF,0xFC,0x3C,
    0xEF,0xFF,0xFC,0x3C,0xEF,0xFF,0x79,0x3E,0xEF,0xFF,0x79,0x3E,0xEF,0xFF,0x33,0x3F,
    0xEF,0xFF,0x33,0x3F,0xEF,0xFF,0x87,0x3F,0xEF,0xFF,0xCF,0x3F,0xEF,0xFF,0x7F,0x3E,
    0xEF,0xFF,0x7F,0x38,0x0F,0x00,0x00,0x30,0xFF,0xFF,0x7F,0x38,0xFF,0xFF,0x7F,0x3E,
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,
  },
  {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xDF,0xFF,0xFE,0xFF,0x8F,0x0F,0x7C,0xE6,0xC7,0xE7,0x9D,0xFF,0xCF,0xFB,0x9D,0xEF,
		0xEF,0xFB,0xFE,0xE3,0xE7,0x3F,0x1F,0xFF,0xF7,0x8F,0x1F,0xFF,0xF3,0xE7,0xFF,0xFB,
		0xF3,0xFB,0xFF,0xFB,0xFB,0xFD,0xFF,0xF9,0xF9,0x0E,0x7D,0xFC,0xF9,0x80,0x01,0xFE,
		0xFB,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  },
  {
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,0xCF,0xFF,0xFF,
		0xF3,0xE7,0xFE,0xFF,0xF3,0x67,0xFE,0xFF,0xF3,0x73,0xF6,0xFF,0xF3,0x33,0xE7,0xFF,
		0xF3,0x99,0xF3,0xFF,0xF3,0x99,0x39,0xFF,0xF3,0x99,0x99,0xFF,0xF3,0xDD,0x9D,0xF1,
		0xE3,0xCC,0xCC,0xF9,0xC3,0xCC,0xCC,0xFC,0xC3,0xCC,0xCC,0xFC,0xC3,0xCC,0xCC,0xFC,
		0xC3,0xCC,0xCC,0xFC,0xE3,0xCC,0xCC,0xF9,0xF3,0xDD,0x9D,0xF1,0xF3,0x99,0x99,0xFF,
		0xF3,0x99,0x39,0xFF,0xF3,0x99,0xF3,0xFF,0xF3,0x33,0xE7,0xFF,0xF3,0x33,0xFE,0xFF,
		0xF3,0x67,0xFE,0xFF,0xF3,0xE7,0xFE,0xFF,0xF7,0xCF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
		0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
  },
  {
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,
    0xFF,0x1F,0xFE,0x3F,0xFF,0x1F,0xFE,0x3F,0xFF,0x0C,0xCC,0x3F,0x7F,0x00,0x80,0x3F,
    0x3F,0x00,0x00,0x3F,0x3F,0xE0,0x01,0x3F,0x7F,0xF8,0x87,0x3F,0x7F,0xFC,0x8F,0x3F,
    0x3F,0xFC,0x0F,0x3F,0x0F,0x3E,0x1F,0x3C,0x0F,0x1E,0x1E,0x3C,0x0F,0x1E,0x1E,0x3C,
    0x0F,0x3E,0x1F,0x3C,0x3F,0xFC,0x0F,0x3F,0x7F,0xFC,0x8F,0x3F,0x7F,0xF8,0x87,0x3F,
    0x3F,0xE0,0x01,0x3F,0x3F,0x00,0x00,0x3F,0x7F,0x00,0x80,0x3F,0xFF,0x0C,0xCC,0x3F,
    0xFF,0x1F,0xFE,0x3F,0xFF,0x1F,0xFE,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F
  },
};

char *itoa(uint32_t num)
{
  static char str[10] = {0};
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", num);
  return str;
}

char *ftoa(float num)
{
  static char str[10] = {0};
	memset(str, 0, sizeof(str));
	sprintf(str, "%.2f", num);
  return str;
}

/************************************ 初始化函数 ***********************************/

/********************************* 初始化数据处理函数 *******************************/

// 显示数值的初始化
void check_box_v_init(uint8_t *param)
{
  check_box.v = param;
}

// 多选框的初始化
void check_box_m_init(uint8_t *param)
{
  check_box.m = param;
}

// 单选框时的初始化
void check_box_s_init(uint8_t *param, uint8_t *param_p)
{
  check_box.s = param;
  check_box.s_p = param_p;
}

// 多选框处理函数
void check_box_m_select(uint8_t param)
{
  check_box.m[param] = !check_box.m[param];
}

// 单选框处理函数
void check_box_s_select(uint8_t val, uint8_t pos)
{
  *check_box.s = val;
  *check_box.s_p = pos;
}

// 弹窗数值初始化
void window_value_init(char title[], uint8_t select, uint8_t *value, uint8_t max, uint8_t min, uint8_t step, M_SELECT *bg, uint8_t index)
{
  strcpy(win.title, title);
  win.select = select;
  win.value = value;
  win.max = max;
  win.min = min;
  win.step = step;
  win.bg = bg;
  win.index = index;
  ui.index = M_WINDOW;
  ui.state = S_WINDOW;
}

// 确认弹窗数值初始化
void confirm_window_value_init(char title[], uint8_t _select_btn, fun_callback_t _cb, M_SELECT *bg, uint8_t index)
{
  strcpy(con_win.title, title);
	con_win.cb = _cb;
  con_win.select_btn = _select_btn;
  con_win.bg = bg;
  con_win.index = index;
  ui.index = M_CONFIRM_WINDOW;
  ui.state = S_CONFIRM_WINDOW;
}

// 确认弹窗数值初始化
void confirm_window_select_button(uint8_t btn)
{
	if(btn)
	{
		con_win.btn_x_trg = con_win.l;
	}
	else
	{
		con_win.btn_x_trg = con_win.l + 78;
	}
}

/********************************* 分页面初始化函数 ********************************/

// 进入磁贴类时的初始化
void tile_param_init(void)
{
  ui.init = false;
  tile.icon_x = 0;
  tile.icon_x_trg = TILE_ICON_S;
  tile.icon_y = -TILE_ICON_H;
  tile.icon_y_trg = 0;
  tile.indi_x = 0;
  tile.indi_x_trg = TILE_INDI_W;
  tile.title_y = tile.title_y_calc;
  tile.title_y_trg = tile.title_y_trg_calc;
}

// 进入睡眠时的初始化
void sleep_param_init(void)
{
  u8g2_SetDrawColor(&u8g2, 0);
  u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
  u8g2_SetPowerSave(&u8g2, 1);
  ui.state = S_NONE;
  ui.sleep = true;
}

// 旋钮设置页初始化
void knob_param_init(void) { check_box_v_init(knob.param); }

// 旋钮旋转页初始化
void krf_param_init(void) { check_box_s_init(&knob.param[KNOB_ROT], &knob.param[KNOB_ROT_P]); }

// 旋钮点按页初始化
void kpf_param_init(void) { check_box_s_init(&knob.param[KNOB_COD], &knob.param[KNOB_COD_P]); }

// 电压测量页初始化
void volt_param_init(void)
{
  volt.text_bg_r = 0;
  volt.text_bg_r_trg = VOLT_TEXT_BG_W;
}

// 设置页初始化
void setting_param_init(void)
{
  check_box_v_init(ui.param);
  check_box_m_init(ui.param);
}

/********************************** 通用初始化函数 *********************************/

/*
  页面层级管理逻辑是，把所有页面都先当作列表类初始化，不是列表类按需求再初始化对应函数
  这样做会浪费一些资源，但跳转页面时只需要考虑页面层级，逻辑上更清晰，减少出错
*/

// 弹窗动画初始化
void window_param_init(void)
{
  win.bar = 0;
  win.y = WIN_Y;
  win.y_trg = win.u;
  ui.state = S_NONE;
}

void confirm_windos_param_init(void)
{
  con_win.y = CON_WIN_Y;
  con_win.y_trg = con_win.u;
	confirm_window_select_button(con_win.select_btn);
  ui.state = S_NONE;
}

// 进入更深层级时的初始化
void layer_init_in(void)
{
  ui.layer++;
  ui.init = 0;
  list.y = 0;
  list.y_trg = LIST_LINE_H;
  list.box_x = 0;
  list.box_y = 0;
  list.bar_y = 0;
  ui.state = S_FADE;
  switch (ui.index)
  {
  case M_MAIN:
    tile_param_init();
    break; // 睡眠进入主菜单，动画初始化
  case M_KNOB:
    knob_param_init();
    break; // 旋钮设置页，行末尾文字初始化
  case M_KRF:
    krf_param_init();
    break; // 旋钮旋转页，单选框初始化
  case M_KPF:
    kpf_param_init();
    break; // 旋钮点按页，单选框初始化
  case M_VOLT:
    volt_param_init();
    break; // 主菜单进入电压测量页，动画初始化
  case M_COUNT:
    count_param_init();
    break; // 主菜单进入计数页，动画初始化
  case M_COUNT_SHOW:
    count_show_init();
    break; // 主菜单进入计数页，动画初始化
  case M_RANGING:
    ranging_param_init();
    break; // 主菜单进入计数页，动画初始化
  case M_SETTING:
    setting_param_init();
    break; // 主菜单进入设置页，单选框初始化
  }
}

// 进入更浅层级时的初始化
void layer_init_out(void)
{
  ui.select[ui.layer] = 0;
  list.box_y_trg[ui.layer] = 0;
  ui.layer--;
  ui.init = 0;
  list.y = 0;
  list.y_trg = LIST_LINE_H;
  list.bar_y = 0;
  ui.state = S_FADE;
  switch (ui.index)
  {
  case M_SLEEP:
    sleep_param_init();
    break; // 主菜单进入睡眠页，检查是否需要写EEPROM
  case M_MAIN:
    tile_param_init();
    break; // 不管什么页面进入主菜单时，动画初始化
  }
}

/************************************* 动画函数 *************************************/

// 动画函数
void animation(float *a, float *a_trg, uint8_t n)
{
  if (*a != *a_trg)
  {
    if (fabs(*a - *a_trg) < 0.15) *a = *a_trg;
    else *a += (*a_trg - *a) / (ui.param[n] / 10.0);
  }
}

// 消失函数
void fade(void)
{
  LL_mDelay(ui.param[FADE_ANI]);
  if (ui.param[DARK_MODE])
  {
    switch (ui.fade)
    {
    case 1:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 != 0)
          buf_ptr[i] = buf_ptr[i] & 0xAA;
      break;
    case 2:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 != 0)
          buf_ptr[i] = buf_ptr[i] & 0x00;
      break;
    case 3:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 == 0)
          buf_ptr[i] = buf_ptr[i] & 0x55;
      break;
    case 4:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 == 0)
          buf_ptr[i] = buf_ptr[i] & 0x00;
      break;
    default:
      ui.state = S_NONE;
      ui.fade = 0;
      break;
    }
  }
  else
  {
    switch (ui.fade)
    {
    case 1:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 != 0)
          buf_ptr[i] = buf_ptr[i] | 0xAA;
      break;
    case 2:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 != 0)
          buf_ptr[i] = buf_ptr[i] | 0x00;
      break;
    case 3:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 == 0)
          buf_ptr[i] = buf_ptr[i] | 0x55;
      break;
    case 4:
      for (uint16_t i = 0; i < buf_len; ++i)
        if (i % 2 == 0)
          buf_ptr[i] = buf_ptr[i] | 0x00;
      break;
    default:
      ui.state = S_NONE;
      ui.fade = 0;
      break;
    }
  }
  ui.fade++;
}

/************************************* 显示函数 *************************************/

// 磁贴类页面通用显示函数
void tile_show(struct MENU arr_1[], const uint8_t icon_pic[][120])
{
  // 计算动画过渡值
  animation(&tile.icon_x, &tile.icon_x_trg, TILE_ANI);
  animation(&tile.icon_y, &tile.icon_y_trg, TILE_ANI);
  animation(&tile.indi_x, &tile.indi_x_trg, TILE_ANI);
  animation(&tile.title_y, &tile.title_y_trg, TILE_ANI);

  // 设置大标题的颜色，0透显，1实显，2反色，这里用实显
  u8g2_SetDrawColor(&u8g2, 1);

  // 绘制大标题
  u8g2_SetFont(&u8g2, TILE_B_FONT);
  u8g2_DrawStr(&u8g2, ((DISP_W - TILE_INDI_W) - u8g2_GetStrWidth(&u8g2, arr_1[ui.select[ui.layer]].m_select)) / 2 + TILE_INDI_W, tile.title_y, arr_1[ui.select[ui.layer]].m_select);

  //绘制大标题指示器
  u8g2_DrawBox(&u8g2, 0, TILE_ICON_S, tile.indi_x, TILE_INDI_H);

  //绘制图标
  if (!ui.init)
  {
    for (uint8_t i = 0; i < ui.num[ui.index]; ++i)
    {
      if (ui.param[TILE_UFD])
        tile.temp = (DISP_W - TILE_ICON_W) / 2 + i * tile.icon_x - TILE_ICON_S * ui.select[ui.layer];
      else
        tile.temp = (DISP_W - TILE_ICON_W) / 2 + (i - ui.select[ui.layer]) * tile.icon_x;
      u8g2_DrawXBMP(&u8g2, tile.temp, (int16_t)tile.icon_y, TILE_ICON_W, TILE_ICON_H, icon_pic[i]);
    }
    if (tile.icon_x == tile.icon_x_trg)
    {
      ui.init = true;
      tile.icon_x = tile.icon_x_trg = -ui.select[ui.layer] * TILE_ICON_S;
    }
  }
  else for (uint8_t i = 0; i < ui.num[ui.index]; ++i) u8g2_DrawXBMP(&u8g2, (DISP_W - TILE_ICON_W) / 2 + (int16_t)tile.icon_x + i * TILE_ICON_S, 0, TILE_ICON_W, TILE_ICON_H, icon_pic[i]);

  //反转屏幕内元素颜色，白天模式遮罩
  u8g2_SetDrawColor(&u8g2, 2);
  if (!ui.param[DARK_MODE])
	{
		u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
	}
}

/*************** 根据列表每行开头符号，判断每行尾部是否绘制以及绘制什么内容 *************/

// 列表显示数值
void list_draw_value(int x, int y, int n) { u8g2_DrawStr(&u8g2, x, y, itoa(check_box.v[n - 1])); }

// 绘制外框
void list_draw_check_box_frame(void) { u8g2_DrawRFrame(&u8g2, CHECK_BOX_L_S, list.temp + CHECK_BOX_U_S, CHECK_BOX_F_W, CHECK_BOX_F_H, 1); }

// 绘制框里面的点
void list_draw_check_box_dot(void) { u8g2_DrawBox(&u8g2, CHECK_BOX_L_S + CHECK_BOX_D_S + 1, list.temp + CHECK_BOX_U_S + CHECK_BOX_D_S + 1, CHECK_BOX_F_W - (CHECK_BOX_D_S + 1) * 2, CHECK_BOX_F_H - (CHECK_BOX_D_S + 1) * 2); }

// 列表显示旋钮功能
void list_draw_krf(int x, int y, int n)
{
  switch (check_box.v[n - 1])
  {
  case 0:
    u8g2_DrawStr(&u8g2, x, y, "OFF");
    break;
  case 1:
    u8g2_DrawStr(&u8g2, x, y, "VOL");
    break;
  case 2:
    u8g2_DrawStr(&u8g2, x, y, "BRI");
    break;
  }
}

// 列表显示按键键值
void list_draw_kpf(int x, int y, int n)
{
  if (check_box.v[n - 1] == 0)
    u8g2_DrawStr(&u8g2, x, y, "OFF");
  else if (check_box.v[n - 1] <= 90)
    u8g2_DrawStr(&u8g2, x, y, itoa((char)check_box.v[n - 1]));
  else
    u8g2_DrawStr(&u8g2, x, y, "?");
}

// 判断列表尾部内容
void list_draw_text_and_check_box(struct MENU arr[], int i)
{
  u8g2_DrawStr(&u8g2, LIST_TEXT_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, arr[i].m_select);
  switch (arr[i].m_select[0])
  {
  case '~':
    list_draw_value(CHECK_BOX_L_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, i);
    break;
  case '+':
    list_draw_check_box_frame();
    if (check_box.m[i - 1] == 1)
      list_draw_check_box_dot();
    break;
  case '=':
    list_draw_check_box_frame();
    if (*check_box.s_p == i)
      list_draw_check_box_dot();
    break;
  case '#':
    list_draw_krf(CHECK_BOX_L_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, i);
    break;
  case '$':
    list_draw_kpf(CHECK_BOX_L_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, i);
    break;
  }
}

/******************************** 列表显示函数 **************************************/

// 列表类页面通用显示函数
void list_show(struct MENU arr[], uint8_t ui_index)
{
  // 更新动画目标值
  u8g2_SetFont(&u8g2, LIST_FONT);
	list.box_x_trg = u8g2_GetStrWidth(&u8g2, arr[ui.select[ui.layer]].m_select) + LIST_TEXT_S * 2;
  list.bar_y_trg = ceil((ui.select[ui.layer]) * ((float)DISP_H / (ui.num[ui_index] - 1)));
  
  //计算动画过渡值
  animation(&list.y, &list.y_trg, LIST_ANI);
  animation(&list.box_x, &list.box_x_trg, LIST_ANI);
  animation(&list.box_y, &list.box_y_trg[ui.layer], LIST_ANI);
  animation(&list.bar_y, &list.bar_y_trg, LIST_ANI);

  //检查循环动画是否结束
  if (list.loop && list.box_y == list.box_y_trg[ui.layer]) 
		list.loop = false;

  //设置文字和进度条颜色，0透显，1实显，2反色，这里都用实显
  u8g2_SetDrawColor(&u8g2, 1);
  
  //绘制进度条
  u8g2_DrawHLine(&u8g2, DISP_W - LIST_BAR_W, 0, LIST_BAR_W);
  u8g2_DrawHLine(&u8g2, DISP_W - LIST_BAR_W, DISP_H - 1, LIST_BAR_W);
  u8g2_DrawVLine(&u8g2, DISP_W - ceil((float)LIST_BAR_W / 2), 0, DISP_H);
  u8g2_DrawBox(&u8g2, DISP_W - LIST_BAR_W, 0, LIST_BAR_W, list.bar_y);

  //绘制列表文字  
  if (!ui.init)
  {
    for (int i = 0; i < ui.num[ui_index]; ++i)
    {
      if (ui.param[LIST_UFD])
        list.temp = i * list.y - LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
      else
        list.temp = (i - ui.select[ui.layer]) * list.y + list.box_y_trg[ui.layer];
      list_draw_text_and_check_box(arr, i);
    }
    if (list.y == list.y_trg)
    {
      ui.init = true;
      list.y = list.y_trg = -LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
    }
  }
  else for (int i = 0; i < ui.num[ui_index]; ++ i)
  {
    list.temp = LIST_LINE_H * i + list.y;
    list_draw_text_and_check_box(arr, i);
  }

  //绘制文字选择框，0透显，1实显，2反色，这里用反色
  u8g2_SetDrawColor(&u8g2, 2);
  u8g2_DrawRBox(&u8g2, 0, list.box_y, list.box_x, LIST_LINE_H, (u8g2_uint_t)LIST_BOX_R);

  //反转屏幕内元素颜色，白天模式遮罩，在这里屏蔽有列表参与的页面，使遮罩作用在那个页面上
  if (!ui.param[DARK_MODE])
  {
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
    switch (ui.index)
    {
    case M_WINDOW:
    case M_CONFIRM_WINDOW:
    case M_VOLT:
      u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
    }
  }
}

// 电压页面显示函数
void volt_show(void)
{
  // 使用列表类显示选项
  list_show(volt_menu, M_VOLT);

  // 计算动画过渡值
  animation(&volt.text_bg_r, &volt.text_bg_r_trg, TAG_ANI);

  // 设置曲线颜色，0透显，1实显，2反色，这里用实显
  u8g2_SetDrawColor(&u8g2, 1);

  // 绘制电压曲线和外框
  volt.val = 0;
  u8g2_DrawFrame(&u8g2, WAVE_BOX_L_S, 0, WAVE_BOX_W, WAVE_BOX_H);
  u8g2_DrawFrame(&u8g2, WAVE_BOX_L_S + 1, 1, WAVE_BOX_W - 2, WAVE_BOX_H - 2);
  if (list.box_y == list.box_y_trg[ui.layer] && list.y == list.y_trg)
  {
		memcpy(volt.ch0_adc, volt.ch0_adc + 1, WAVE_W-2);
		volt.ch0_adc[WAVE_W - 1] = power_monitor_read_value(ui.select[ui.layer]);
    for (int i = 1; i < WAVE_W - 1; i++)
    {
			volt.val += power_monitor_read_value(ui.select[ui.layer]);
      volt.ch0_wave[i] = map_number(power_monitor_read_value(ui.select[ui.layer]), 0, 4095, WAVE_MAX, WAVE_MIN);
      u8g2_DrawLine(&u8g2, WAVE_L + i - 1, WAVE_U + volt.ch0_wave[i - 1], WAVE_L + i, WAVE_U + volt.ch0_wave[i]);
    }
  }

  // 绘制电压值
  u8g2_SetFontDirection(&u8g2, 0);
  u8g2_SetFont(&u8g2, VOLT_FONT);
	if(ui.select[ui.layer] == 0)
	{
			volt.val = volt.val / WAVE_W;
	  u8g2_DrawStr(&u8g2, 39, DISP_H - 6, ftoa(volt.val / 4096.0 * 13.8));
		u8g2_DrawStr(&u8g2, DISP_W-28, DISP_H - 6, "V");
	}
	else if((ui.select[ui.layer] == 1))
	{
		volt.val = volt.val / WAVE_W;
		volt.val = ((volt.val / 4096.0 * 3.3)-1.648)/(AMPS_RATIO);
	  u8g2_DrawStr(&u8g2, 39, DISP_H - 6, ftoa(volt.val));
		u8g2_DrawStr(&u8g2, DISP_W-28, DISP_H - 6, "A");
	}

  // 绘制列表选择框和电压文字背景
  u8g2_SetDrawColor(&u8g2, 2);
  u8g2_DrawBox(&u8g2, VOLT_TEXT_BG_L_S, DISP_H - VOLT_TEXT_BG_H, volt.text_bg_r, VOLT_TEXT_BG_H);

  // 反转屏幕内元素颜色，白天模式遮罩
  if (!ui.param[DARK_MODE])
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
}

// 弹窗通用显示函数
void window_show(void)
{
  // 绘制背景列表，根据开关判断背景是否要虚化
  list_show(win.bg, win.index);
  if (ui.param[WIN_BOK])
    for (uint16_t i = 0; i < buf_len; ++i)
      buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);

  // 更新动画目标值
  u8g2_SetFont(&u8g2, WIN_FONT);
  win.bar_trg = (float)(*win.value - win.min) / (float)(win.max - win.min) * (WIN_BAR_W - 4);

  // 计算动画过渡值
  animation(&win.bar, &win.bar_trg, WIN_ANI);
  animation(&win.y, &win.y_trg, WIN_ANI);

  // 绘制窗口
  u8g2_SetDrawColor(&u8g2, 0);
  u8g2_DrawRBox(&u8g2, win.l, (int16_t)win.y, WIN_W, WIN_H, 2); // 绘制外框背景
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawRFrame(&u8g2, win.l, (int16_t)win.y, WIN_W, WIN_H, 2);                  // 绘制外框描边
  u8g2_DrawRFrame(&u8g2, win.l + 5, (int16_t)win.y + 20, WIN_BAR_W, WIN_BAR_H, 1); // 绘制进度条外框
  u8g2_DrawBox(&u8g2, win.l + 7, (int16_t)win.y + 22, win.bar, WIN_BAR_H - 4);     // 绘制进度条
  u8g2_DrawStr(&u8g2, win.l + 5, (int16_t)win.y + 14, win.title); // 绘制标题
  u8g2_DrawStr(&u8g2, win.l + 78, (int16_t)win.y + 14, itoa(*win.value)); // 绘制当前值

  // 需要在窗口修改参数时立即见效的函数
  if (!strcmp(win.title, "Disp Bri"))
    u8g2_SetContrast(&u8g2, ui.param[DISP_BRI]);

  // 反转屏幕内元素颜色，白天模式遮罩
  u8g2_SetDrawColor(&u8g2, 2);
  if (!ui.param[DARK_MODE])
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
}

// 确认弹窗通用显示函数
void confirm_window_show(void)
{
  // 绘制背景列表，根据开关判断背景是否要虚化
  list_show(con_win.bg, con_win.index);
  if (ui.param[WIN_BOK])
    for (uint16_t i = 0; i < buf_len; ++i)
      buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);

  // 计算动画过渡值
  animation(&con_win.y, &con_win.y_trg, WIN_ANI);
  animation(&con_win.btn_x, &con_win.btn_x_trg, WIN_ANI);

  // 绘制窗口
  u8g2_SetDrawColor(&u8g2, 0);
  u8g2_DrawRBox(&u8g2, con_win.l, (int16_t)con_win.y, CON_WIN_W, CON_WIN_H, 2); 	// 绘制外框背景
  u8g2_SetDrawColor(&u8g2, 1);
  u8g2_DrawRFrame(&u8g2, con_win.l, (int16_t)con_win.y, CON_WIN_W, CON_WIN_H, 2);	// 绘制外框描边
	u8g2_DrawStr(&u8g2, con_win.l + 40, (int16_t)con_win.y + 14, con_win.title); 		// 绘制标题
	u8g2_DrawStr(&u8g2, con_win.l + 5, (int16_t)con_win.y + 28, "YES"); 						// 绘制YES
	u8g2_DrawStr(&u8g2, DISP_W - con_win.l - 20, (int16_t)con_win.y + 28, "NO"); 		// 绘制NO
  u8g2_SetDrawColor(&u8g2, 2);
	if(con_win.select_btn)
		u8g2_DrawRBox(&u8g2, con_win.btn_x + 4, (int16_t)con_win.y + 15, 21, LIST_LINE_H, (u8g2_uint_t)LIST_BOX_R); 	// 绘制外框背景
	else
		u8g2_DrawRBox(&u8g2, con_win.btn_x + 4, (int16_t)con_win.y + 15, 15, LIST_LINE_H, (u8g2_uint_t)LIST_BOX_R); 	// 绘制外框背景
	
  // 反转屏幕内元素颜色，白天模式遮罩
  u8g2_SetDrawColor(&u8g2, 2);
  if (!ui.param[DARK_MODE])
    u8g2_DrawBox(&u8g2, 0, 0, DISP_W, DISP_H);
}

/************************************* 处理函数 *************************************/

/*********************************** 通用处理函数 ***********************************/

// 磁贴类页面旋转时判断通用函数
void tile_rotate_switch(void)
{
  switch (btn.id)
  {
  case BTN_ID_CC:
    if (ui.init)
    {
      if (ui.select[ui.layer] > 0)
      {
        ui.select[ui.layer] -= 1;
        tile.icon_x_trg += TILE_ICON_S;
        tile.select_flag = false;
      }
      else
      {
        if (ui.param[TILE_LOOP])
        {
          ui.select[ui.layer] = ui.num[ui.index] - 1;
          tile.icon_x_trg = -TILE_ICON_S * (ui.num[ui.index] - 1);
          break;
        }
        else
          tile.select_flag = true;
      }
    }
    break;

  case BTN_ID_CW:
    if (ui.init)
    {
      if (ui.select[ui.layer] < (ui.num[ui.index] - 1))
      {
        ui.select[ui.layer] += 1;
        tile.icon_x_trg -= TILE_ICON_S;
        tile.select_flag = false;
      }
      else
      {
        if (ui.param[TILE_LOOP])
        {
          ui.select[ui.layer] = 0;
          tile.icon_x_trg = 0;
          break;
        }
        else
          tile.select_flag = true;
      }
    }
    break;
  }
}

// 列表类页面旋转时判断通用函数
void list_rotate_switch(void)
{
  if (!list.loop)
  {
    switch (btn.id)
    {
    case BTN_ID_CC:
      if (ui.select[ui.layer] == 0)
      {
        if (ui.param[LIST_LOOP] && ui.init)
        {
          list.loop = true;
          ui.select[ui.layer] = ui.num[ui.index] - 1;
          if (ui.num[ui.index] > list.line_n)
          {
            list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
            list.y_trg = DISP_H - ui.num[ui.index] * LIST_LINE_H;
          }
          else
            list.box_y_trg[ui.layer] = (ui.num[ui.index] - 1) * LIST_LINE_H;
          break;
        }
        else
          break;
      }
      if (ui.init)
      {
        ui.select[ui.layer] -= 1;
        if (ui.select[ui.layer] < -(list.y_trg / LIST_LINE_H))
        {
          if (!(DISP_H % LIST_LINE_H))
            list.y_trg += LIST_LINE_H;
          else
          {
            if (list.box_y_trg[ui.layer] == DISP_H - LIST_LINE_H * list.line_n)
            {
              list.y_trg += (list.line_n + 1) * LIST_LINE_H - DISP_H;
              list.box_y_trg[ui.layer] = 0;
            }
            else if (list.box_y_trg[ui.layer] == LIST_LINE_H)
            {
              list.box_y_trg[ui.layer] = 0;
            }
            else
              list.y_trg += LIST_LINE_H;
          }
        }
        else
          list.box_y_trg[ui.layer] -= LIST_LINE_H;
        break;
      }

    case BTN_ID_CW:
      if (ui.select[ui.layer] == (ui.num[ui.index] - 1))
      {
        if (ui.param[LIST_LOOP] && ui.init)
        {
          list.loop = true;
          ui.select[ui.layer] = 0;
          list.y_trg = 0;
          list.box_y_trg[ui.layer] = 0;
          break;
        }
        else
          break;
      }
      if (ui.init)
      {
        ui.select[ui.layer] += 1;
        if ((ui.select[ui.layer] + 1) > (list.line_n - list.y_trg / LIST_LINE_H))
        {
          if (!(DISP_H % LIST_LINE_H))
            list.y_trg -= LIST_LINE_H;
          else
          {
            if (list.box_y_trg[ui.layer] == LIST_LINE_H * (list.line_n - 1))
            {
              list.y_trg -= (list.line_n + 1) * LIST_LINE_H - DISP_H;
              list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
            }
            else if (list.box_y_trg[ui.layer] == DISP_H - LIST_LINE_H * 2)
            {
              list.box_y_trg[ui.layer] = DISP_H - LIST_LINE_H;
            }
            else
              list.y_trg -= LIST_LINE_H;
          }
        }
        else
          list.box_y_trg[ui.layer] += LIST_LINE_H;
        break;
      }
      break;
    }
  }
}

// 弹窗通用处理函数
void window_proc(void)
{
  window_show();
  if (win.y == WIN_Y_TRG)
    ui.index = win.index;
  if (btn.pressed && win.y == win.y_trg && win.y != WIN_Y_TRG)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
      if (*win.value < win.max)
        *win.value += win.step;
				eeprom_notify_change();
      break;
    case BTN_ID_CC:
      if (*win.value > win.min)
        *win.value -= win.step;
				eeprom_notify_change();
      break;
    case BTN_ID_SP:
    case BTN_ID_LP:
      win.y_trg = WIN_Y_TRG;
      break;
    }
  }
}

// 弹窗通用处理函数
void confirm_window_proc(void)
{
  confirm_window_show();
  if (con_win.y == CON_WIN_Y_TRG)
    ui.index = con_win.index;
  if (btn.pressed && con_win.y == con_win.y_trg && con_win.y != CON_WIN_Y_TRG)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
				con_win.select_btn = 0;
				confirm_window_select_button(con_win.select_btn);
				eeprom_notify_change();
      break;
    case BTN_ID_CC:
				con_win.select_btn = 1;
				confirm_window_select_button(con_win.select_btn);
				eeprom_notify_change();
      break;
    case BTN_ID_SP:
    case BTN_ID_LP:
			con_win.cb(con_win.select_btn);
      con_win.y_trg = CON_WIN_Y_TRG;
      break;
    }
  }
}

/********************************** 分页面处理函数 **********************************/

// 睡眠页面处理函数
void sleep_proc(void)
{
  while (ui.sleep)
  {
    // 睡眠时循环执行的函数

    // 睡眠时需要扫描旋钮才能退出睡眠
//    btn_scan();

    // 当旋钮有动作时
    if (btn.pressed)
    {
      btn.pressed = false;
      switch (btn.id)
      {

      // 睡眠时顺时针旋转执行的函数
      case BTN_ID_CW:
        switch (knob.param[KNOB_ROT])
        {
        case KNOB_ROT_VOL:
//          Consumer.press(HIDConsumer::VOLUME_UP);
//          Consumer.release();
          break;
        case KNOB_ROT_BRI:
//          Consumer.press(HIDConsumer::BRIGHTNESS_UP);
//          Consumer.release();
          break;
        }
        break;

      // 睡眠时逆时针旋转执行的函数
      case BTN_ID_CC:
        switch (knob.param[KNOB_ROT])
        {
        case KNOB_ROT_VOL:
//          Consumer.press(HIDConsumer::VOLUME_DOWN);
//          Consumer.release();
          break;
        case KNOB_ROT_BRI:
//          Consumer.press(HIDConsumer::BRIGHTNESS_DOWN);
//          Consumer.release();
          break;
        }
        break;

      // 睡眠时短按执行的函数
      case BTN_ID_SP:
//        Keyboard.press(knob.param[KNOB_COD]);
//        Keyboard.release(knob.param[KNOB_COD]);
        break;

      // 睡眠时长按执行的函数
      case BTN_ID_LP:
        ui.index = M_MAIN;
        ui.state = S_LAYER_IN;
        u8g2_SetPowerSave(&u8g2, 0);
        ui.sleep = false;
        break;
      }
    }
  }
}

// 主菜单处理函数，磁贴类模板
void main_proc(void)
{
  tile_show(main_menu, main_icon_pic);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      tile_rotate_switch();
      break;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
				case 0:
					ui.index = M_SLEEP;
					ui.state = S_LAYER_OUT;
					break;
				case 1:
					ui.index = M_EDITOR;
					ui.state = S_LAYER_IN;
					break;
				case 2:
					ui.index = M_VOLT;
					ui.state = S_LAYER_IN;
					break;
				case 3:
					ui.index = M_COUNT;
					ui.state = S_LAYER_IN;
					break;
				case 4:
					ui.index = M_RANGING;
					ui.state = S_LAYER_IN;
					break;
				case 5:
					ui.index = M_SETTING;
					ui.state = S_LAYER_IN;
					break;
      }
    }
    if (!tile.select_flag && ui.init)
    {
      tile.indi_x = 0;
      tile.title_y = tile.title_y_calc;
    }
  }
}

// 编辑器菜单处理函数
void editor_proc(void)
{
  list_show(editor_menu, M_EDITOR);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      list_rotate_switch();
      break;
    case BTN_ID_LP:
      ui.select[ui.layer] = 0;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
				case 0:
					ui.index = M_MAIN;
					ui.state = S_LAYER_OUT;
					break;
				case 11:
					ui.index = M_KNOB;
					ui.state = S_LAYER_IN;
					break;
      }
    }
  }
}

// 旋钮编辑菜单处理函数
void knob_proc(void)
{
  list_show(knob_menu, M_KNOB);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      list_rotate_switch();
      break;
    case BTN_ID_LP:
      ui.select[ui.layer] = 0;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
				case 0:
					ui.index = M_EDITOR;
					ui.state = S_LAYER_OUT;
					break;
				case 1:
					ui.index = M_KRF;
					ui.state = S_LAYER_IN;
					check_box_s_init(&knob.param[KNOB_ROT], &knob.param[KNOB_ROT_P]);
					break;
				case 2:
					ui.index = M_KPF;
					ui.state = S_LAYER_IN;
					check_box_s_init(&knob.param[KNOB_COD], &knob.param[KNOB_COD_P]);
					break;
      }
    }
  }
}

// 旋钮旋转功能菜单处理函数
void krf_proc(void)
{
  list_show(krf_menu, M_KRF);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      list_rotate_switch();
      break;
    case BTN_ID_LP:
      ui.select[ui.layer] = 0;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
				case 0:
					ui.index = M_KNOB;
					ui.state = S_LAYER_OUT;
					break;
				case 1:
					break;
				case 2:
					check_box_s_select(KNOB_DISABLE, ui.select[ui.layer]);
					break;
				case 3:
					break;
				case 4:
					check_box_s_select(KNOB_ROT_VOL, ui.select[ui.layer]);
					break;
				case 5:
					check_box_s_select(KNOB_ROT_BRI, ui.select[ui.layer]);
					break;
				case 6:
					break;
      }
    }
  }
}

// 旋钮点按功能菜单处理函数
void kpf_proc(void)
{
  list_show(kpf_menu, M_KPF);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      list_rotate_switch();
      break;
    case BTN_ID_LP:
      ui.select[ui.layer] = 0;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
      case 0:
        ui.index = M_KNOB;
        ui.state = S_LAYER_OUT;
        break;
      case 1:
        break;
      case 2:
        check_box_s_select(KNOB_DISABLE, ui.select[ui.layer]);
        break;
      case 3:
        break;
      case 4:
        check_box_s_select('A', ui.select[ui.layer]);
        break;
      case 5:
        check_box_s_select('B', ui.select[ui.layer]);
        break;
      case 6:
        check_box_s_select('C', ui.select[ui.layer]);
        break;
      case 7:
        check_box_s_select('D', ui.select[ui.layer]);
        break;
      case 8:
        check_box_s_select('E', ui.select[ui.layer]);
        break;
      case 9:
        check_box_s_select('F', ui.select[ui.layer]);
        break;
      case 10:
        check_box_s_select('G', ui.select[ui.layer]);
        break;
      case 11:
        check_box_s_select('H', ui.select[ui.layer]);
        break;
      case 12:
        check_box_s_select('I', ui.select[ui.layer]);
        break;
      case 13:
        check_box_s_select('J', ui.select[ui.layer]);
        break;
      case 14:
        check_box_s_select('K', ui.select[ui.layer]);
        break;
      case 15:
        check_box_s_select('L', ui.select[ui.layer]);
        break;
      case 16:
        check_box_s_select('M', ui.select[ui.layer]);
        break;
      case 17:
        check_box_s_select('N', ui.select[ui.layer]);
        break;
      case 18:
        check_box_s_select('O', ui.select[ui.layer]);
        break;
      case 19:
        check_box_s_select('P', ui.select[ui.layer]);
        break;
      case 20:
        check_box_s_select('Q', ui.select[ui.layer]);
        break;
      case 21:
        check_box_s_select('R', ui.select[ui.layer]);
        break;
      case 22:
        check_box_s_select('S', ui.select[ui.layer]);
        break;
      case 23:
        check_box_s_select('T', ui.select[ui.layer]);
        break;
      case 24:
        check_box_s_select('U', ui.select[ui.layer]);
        break;
      case 25:
        check_box_s_select('V', ui.select[ui.layer]);
        break;
      case 26:
        check_box_s_select('W', ui.select[ui.layer]);
        break;
      case 27:
        check_box_s_select('X', ui.select[ui.layer]);
        break;
      case 28:
        check_box_s_select('Y', ui.select[ui.layer]);
        break;
      case 29:
        check_box_s_select('Z', ui.select[ui.layer]);
        break;
      case 30:
        break;
      case 31:
        check_box_s_select('0', ui.select[ui.layer]);
        break;
      case 32:
        check_box_s_select('1', ui.select[ui.layer]);
        break;
      case 33:
        check_box_s_select('2', ui.select[ui.layer]);
        break;
      case 34:
        check_box_s_select('3', ui.select[ui.layer]);
        break;
      case 35:
        check_box_s_select('4', ui.select[ui.layer]);
        break;
      case 36:
        check_box_s_select('5', ui.select[ui.layer]);
        break;
      case 37:
        check_box_s_select('6', ui.select[ui.layer]);
        break;
      case 38:
        check_box_s_select('7', ui.select[ui.layer]);
        break;
      case 39:
        check_box_s_select('8', ui.select[ui.layer]);
        break;
      case 40:
        check_box_s_select('9', ui.select[ui.layer]);
        break;
      case 41:
        break;
#if 0
      case 42:
        check_box_s_select(KEY_ESC, ui.select[ui.layer]);
        break;
      case 43:
        check_box_s_select(KEY_F1, ui.select[ui.layer]);
        break;
      case 44:
        check_box_s_select(KEY_F2, ui.select[ui.layer]);
        break;
      case 45:
        check_box_s_select(KEY_F3, ui.select[ui.layer]);
        break;
      case 46:
        check_box_s_select(KEY_F4, ui.select[ui.layer]);
        break;
      case 47:
        check_box_s_select(KEY_F5, ui.select[ui.layer]);
        break;
      case 48:
        check_box_s_select(KEY_F6, ui.select[ui.layer]);
        break;
      case 49:
        check_box_s_select(KEY_F7, ui.select[ui.layer]);
        break;
      case 50:
        check_box_s_select(KEY_F8, ui.select[ui.layer]);
        break;
      case 51:
        check_box_s_select(KEY_F9, ui.select[ui.layer]);
        break;
      case 52:
        check_box_s_select(KEY_F10, ui.select[ui.layer]);
        break;
      case 53:
        check_box_s_select(KEY_F11, ui.select[ui.layer]);
        break;
      case 54:
        check_box_s_select(KEY_F12, ui.select[ui.layer]);
        break;
      case 55:
        break;
      case 56:
        check_box_s_select(KEY_LEFT_CTRL, ui.select[ui.layer]);
        break;
      case 57:
        check_box_s_select(KEY_LEFT_SHIFT, ui.select[ui.layer]);
        break;
      case 58:
        check_box_s_select(KEY_LEFT_ALT, ui.select[ui.layer]);
        break;
      case 59:
        check_box_s_select(KEY_LEFT_GUI, ui.select[ui.layer]);
        break;
      case 60:
        check_box_s_select(KEY_RIGHT_CTRL, ui.select[ui.layer]);
        break;
      case 61:
        check_box_s_select(KEY_RIGHT_SHIFT, ui.select[ui.layer]);
        break;
      case 62:
        check_box_s_select(KEY_RIGHT_ALT, ui.select[ui.layer]);
        break;
      case 63:
        check_box_s_select(KEY_RIGHT_GUI, ui.select[ui.layer]);
        break;
      case 64:
        break;
      case 65:
        check_box_s_select(KEY_CAPS_LOCK, ui.select[ui.layer]);
        break;
      case 66:
        check_box_s_select(KEY_BACKSPACE, ui.select[ui.layer]);
        break;
      case 67:
        check_box_s_select(KEY_RETURN, ui.select[ui.layer]);
        break;
      case 68:
        check_box_s_select(KEY_INSERT, ui.select[ui.layer]);
        break;
      case 69:
        check_box_s_select(KEY_DELETE, ui.select[ui.layer]);
        break;
      case 70:
        check_box_s_select(KEY_TAB, ui.select[ui.layer]);
        break;
      case 71:
        break;
      case 72:
        check_box_s_select(KEY_HOME, ui.select[ui.layer]);
        break;
      case 73:
        check_box_s_select(KEY_END, ui.select[ui.layer]);
        break;
      case 74:
        check_box_s_select(KEY_PAGE_UP, ui.select[ui.layer]);
        break;
      case 75:
        check_box_s_select(KEY_PAGE_DOWN, ui.select[ui.layer]);
        break;
      case 76:
        break;
      case 77:
        check_box_s_select(KEY_UP_ARROW, ui.select[ui.layer]);
        break;
      case 78:
        check_box_s_select(KEY_DOWN_ARROW, ui.select[ui.layer]);
        break;
      case 79:
        check_box_s_select(KEY_LEFT_ARROW, ui.select[ui.layer]);
        break;
      case 80:
        check_box_s_select(KEY_RIGHT_ARROW, ui.select[ui.layer]);
        break;
#endif
      case 81:
        break;
      }
    }
  }
}

// 电压测量页处理函数
void volt_proc(void)
{
  volt_show();
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
				ui.index = M_MAIN;
				ui.state = S_LAYER_OUT;
				break;
    }
  }
}

// 设置菜单处理函数，多选框列表类模板，弹窗模板
void setting_proc(void)
{
  list_show(setting_menu, M_SETTING);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
    case BTN_ID_CW:
    case BTN_ID_CC:
      list_rotate_switch();
      break;
    case BTN_ID_LP:
      ui.select[ui.layer] = 0;
    case BTN_ID_SP:
      switch (ui.select[ui.layer])
      {
      // 返回更浅层级，长按被当作选择这一项，也是执行这一行
      case 0:
        ui.index = M_MAIN;
        ui.state = S_LAYER_OUT;
        break;
      // 弹出窗口，参数初始化：标题，参数名，参数值，最大值，最小值，步长，背景列表名，背景列表标签
      case 1:
        window_value_init("Disp Bri", DISP_BRI, &ui.param[DISP_BRI], 255, 0, 5, setting_menu, M_SETTING);
        break;
      case 2:
        window_value_init("Tile Ani", TILE_ANI, &ui.param[TILE_ANI], 100, 10, 1, setting_menu, M_SETTING);
        break;
      case 3:
        window_value_init("List Ani", LIST_ANI, &ui.param[LIST_ANI], 100, 10, 1, setting_menu, M_SETTING);
        break;
      case 4:
        window_value_init("Win Ani", WIN_ANI, &ui.param[WIN_ANI], 100, 10, 1, setting_menu, M_SETTING);
        break;
      case 5:
        window_value_init("Spot Ani", SPOT_ANI, &ui.param[SPOT_ANI], 100, 10, 1, setting_menu, M_SETTING);
        break;
      case 6:
        window_value_init("Tag Ani", TAG_ANI, &ui.param[TAG_ANI], 100, 10, 1, setting_menu, M_SETTING);
        break;
      case 7:
        window_value_init("Fade Ani", FADE_ANI, &ui.param[FADE_ANI], 255, 0, 1, setting_menu, M_SETTING);
        break;
      case 8:
        window_value_init("Btn SPT", BTN_SPT, &ui.param[BTN_SPT], 255, 0, 1, setting_menu, M_SETTING);
        break;
      case 9:
        window_value_init("Btn LPT", BTN_LPT, &ui.param[BTN_LPT], 255, 0, 1, setting_menu, M_SETTING);
        break;
      // 多选框
      case 10:
        check_box_m_select(TILE_UFD);
        break;
      case 11:
        check_box_m_select(LIST_UFD);
        break;
      case 12:
        check_box_m_select(TILE_LOOP);
        break;
      case 13:
        check_box_m_select(LIST_LOOP);
        break;
      case 14:
        check_box_m_select(WIN_BOK);
        break;
      case 15:
        check_box_m_select(KNOB_DIR);
        break;
      case 16:
        check_box_m_select(DARK_MODE);
        break;
      // 关于本机
      case 17:
        ui.index = M_ABOUT;
        ui.state = S_LAYER_IN;
        break;
      }
    }
  }
}

// 关于本机页
void about_proc(void)
{
  list_show(about_menu, M_ABOUT);
  if (btn.pressed)
  {
    btn.pressed = false;
    switch (btn.id)
    {
			case BTN_ID_CW:
			case BTN_ID_CC:
				list_rotate_switch();
				break;
			case BTN_ID_LP:
				ui.select[ui.layer] = 0;
			case BTN_ID_SP:{
				switch (ui.select[ui.layer])
				{
					case 0:
					ui.index = M_SETTING;
					ui.state = S_LAYER_OUT;
					break;
				}
			};break;
    }
  }
}

// 总的UI进程
void ui_proc(void)
{
  u8g2_SendBuffer(&u8g2);
  switch (ui.state)
  {
  case S_FADE:
    fade();
    break; // 转场动画
  case S_WINDOW:
    window_param_init();
    break; // 弹窗初始化
	case S_CONFIRM_WINDOW:
		confirm_windos_param_init();
		break;// 确认弹窗初始化
  case S_LAYER_IN:
    layer_init_in();
    break; // 层级初始化
  case S_LAYER_OUT:
    layer_init_out();
    break; // 层级初始化
  case S_NONE:
    u8g2_ClearBuffer(&u8g2);
    switch (ui.index) // 直接选择页面
    {
    case M_WINDOW:
      window_proc();
      break;
    case M_CONFIRM_WINDOW:
      confirm_window_proc();
      break;
    case M_SLEEP:
      sleep_proc();
      break;
    case M_MAIN:
      main_proc();
      break;
    case M_EDITOR:
      editor_proc();
      break;
    case M_KNOB:
      knob_proc();
      break;
    case M_KRF:
      krf_proc();
      break;
    case M_KPF:
      kpf_proc();
      break;
    case M_VOLT:
      volt_proc();
      break;
    case M_COUNT:
      count_proc();
      break;
    case M_RANGING:
      ranging_proc();
      break;
    case M_COUNT_SHOW:
      count_show_proc();
      break;
    case M_SETTING:
      setting_proc();
      break;
    case M_ABOUT:
      about_proc();
      break;
    }
  }
}

/*********************************** UI 初始化函数 *********************************/

// 在初始化EEPROM时，选择性初始化的默认设置
void ui_param_init(void)
{
  ui.param[DISP_BRI] = 255; // 屏幕亮度
  ui.param[TILE_ANI] = 30;  // 磁贴动画速度
  ui.param[LIST_ANI] = 60;  // 列表动画速度
  ui.param[WIN_ANI] = 25;   // 弹窗动画速度
  ui.param[SPOT_ANI] = 50;  // 聚光动画速度
  ui.param[TAG_ANI] = 60;   // 标签动画速度
  ui.param[FADE_ANI] = 30;  // 消失动画速度
  ui.param[BTN_SPT] = 25;   // 按键短按时长
  ui.param[BTN_LPT] = 150;  // 按键长按时长
  ui.param[TILE_UFD] = 1;   // 磁贴图标从头展开开关
  ui.param[LIST_UFD] = 1;   // 菜单列表从头展开开关
  ui.param[TILE_LOOP] = 0;  // 磁贴图标循环模式开关
  ui.param[LIST_LOOP] = 0;  // 菜单列表循环模式开关
  ui.param[WIN_BOK] = 0;    // 弹窗背景虚化开关
  ui.param[KNOB_DIR] = 0;   // 旋钮方向切换开关
  ui.param[DARK_MODE] = 1;  // 黑暗模式开关
}

// 列表类页面列表行数初始化，必须初始化的参数
void ui_init(void)
{
  ui.num[M_MAIN] = sizeof(main_menu) / sizeof(M_SELECT);
  ui.num[M_EDITOR] = sizeof(editor_menu) / sizeof(M_SELECT);
  ui.num[M_KNOB] = sizeof(knob_menu) / sizeof(M_SELECT);
  ui.num[M_KRF] = sizeof(krf_menu) / sizeof(M_SELECT);
  ui.num[M_KPF] = sizeof(kpf_menu) / sizeof(M_SELECT);
  ui.num[M_VOLT] = sizeof(volt_menu) / sizeof(M_SELECT);
  ui.num[M_SETTING] = sizeof(setting_menu) / sizeof(M_SELECT);
  ui.num[M_ABOUT] = sizeof(about_menu) / sizeof(M_SELECT);

  ui.num[M_COUNT] = sizeof(count_menu) / sizeof(M_SELECT);

//	ui_param_init();

  ui.index = M_MAIN;
  ui.state = S_LAYER_IN;
  u8g2_SetPowerSave(&u8g2, 0);
  ui.sleep = false;
  ui.fade = 1;

  win.l = (DISP_W - WIN_W) / 2;
  win.u = (DISP_H - WIN_H) / 2;
	
  con_win.l = (DISP_W - CON_WIN_W) / 2;
  con_win.u = (DISP_H - CON_WIN_H) / 2;

  list.line_n = DISP_H / LIST_LINE_H;
	
	tile.title_y_calc = TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H * 2;
  tile.title_y_trg_calc = TILE_INDI_S + (TILE_INDI_H - TILE_B_TITLE_H) / 2 + TILE_B_TITLE_H;
	
	knob.param[0] = KNOB_DISABLE; // 禁用在列表的第2个选项，第0个是标题，第1个是分界线
	knob.param[1] = KNOB_DISABLE; // 禁用在列表的第2个选项，第0个是标题，第1个是分界线
	knob.param[2] = 2; // 禁用在列表的第2个选项，第0个是标题，第1个是分界线
	knob.param[3] = 2; // 禁用在列表的第2个选项，第0个是标题，第1个是分界线
}


//求和校验
unsigned short config_check(unsigned short *_data, unsigned char len)
{
	unsigned short ret = 0;
	for(unsigned char i = 0;i<len;i++)
	{
		ret+=_data[i];
	}
	return ret;
}

/************************************* 断电保存 *************************************/
void eeprom_notify_change(void)
{
	eeprom_change = true;
}

//内存排列顺序：UI参数+计数翻转时间+计数次数+校验
#define USER_CONFIG_SIZE	UI_PARAM + 1 + 4 + 1
//EEPROM写数据，回到睡眠时执行一遍
void eeprom_write_all_data(void)
{
	uint32_t ret = 0;
	uint16_t *user_config = (uint16_t * ) malloc (USER_CONFIG_SIZE * sizeof(uint16_t));

  user_config[DISP_BRI] = ui.param[DISP_BRI]; 		// 屏幕亮度
	user_config[TILE_ANI] = ui.param[TILE_ANI];  		// 磁贴动画速度
	user_config[LIST_ANI] = ui.param[LIST_ANI];  		// 列表动画速度
	user_config[WIN_ANI] = ui.param[WIN_ANI];   		// 弹窗动画速度
	user_config[SPOT_ANI] = ui.param[SPOT_ANI];  		// 聚光动画速度
	user_config[TAG_ANI] = ui.param[TAG_ANI];   		// 标签动画速度
	user_config[FADE_ANI] = ui.param[FADE_ANI];  		// 消失动画速度
	user_config[BTN_SPT] = ui.param[BTN_SPT];   		// 按键短按时长
	user_config[BTN_LPT] = ui.param[BTN_LPT];  			// 按键长按时长
	user_config[TILE_UFD] = ui.param[TILE_UFD];   	// 磁贴图标从头展开开关
	user_config[LIST_UFD] = ui.param[LIST_UFD];   	// 菜单列表从头展开开关
	user_config[TILE_LOOP] = ui.param[TILE_LOOP];  	// 磁贴图标循环模式开关
	user_config[LIST_LOOP] = ui.param[LIST_LOOP];  	// 菜单列表循环模式开关
	user_config[WIN_BOK]= ui.param[WIN_BOK];   			// 弹窗背景虚化开关
	user_config[KNOB_DIR] =   ui.param[KNOB_DIR];		// 旋钮方向切换开关
  user_config[DARK_MODE] = ui.param[DARK_MODE];  	// 黑暗模式开关
	
  ret = ir_sw_get_time();  												// 翻转计数时间
	user_config[UI_PARAM] = ret;
	
	ret = ir_sw_get_count();												// 触发计数
	user_config[UI_PARAM+1] = (uint8_t)ret>>24;
	user_config[UI_PARAM+2] = (uint8_t)ret>>16;
	user_config[UI_PARAM+3] = (uint8_t)ret>>8;
	user_config[UI_PARAM+4] = (uint8_t)ret;
	
	ret = config_check(user_config, USER_CONFIG_SIZE - 1);
	user_config[UI_PARAM+5] = ret;									//校验
	STMFLASH_Write(USER_CONFIG_ADDR, user_config, USER_CONFIG_SIZE);
	free(user_config);
}

//EEPROM读数据，开机初始化时执行一遍
void eeprom_read_all_data(void)
{
	uint16_t ret = 0;
	uint16_t *user_config = (uint16_t * ) malloc (USER_CONFIG_SIZE * sizeof(uint16_t));
	STMFLASH_Read(USER_CONFIG_ADDR, user_config, USER_CONFIG_SIZE);
	ret = config_check(user_config, USER_CONFIG_SIZE - 1);
	if(ret != user_config[USER_CONFIG_SIZE - 1])
	{
		ui_param_init();
		ir_sw_set_time(10);
		ir_sw_set_count(0);
		eeprom_write_all_data();
	}
  ui.param[DISP_BRI] = user_config[DISP_BRI]; 		// 屏幕亮度
  ui.param[TILE_ANI] = user_config[TILE_ANI];  		// 磁贴动画速度
  ui.param[LIST_ANI] = user_config[LIST_ANI];  		// 列表动画速度
  ui.param[WIN_ANI] = user_config[WIN_ANI];   		// 弹窗动画速度
  ui.param[SPOT_ANI] = user_config[SPOT_ANI];  		// 聚光动画速度
  ui.param[TAG_ANI] = user_config[TAG_ANI];   		// 标签动画速度
  ui.param[FADE_ANI] = user_config[FADE_ANI];  		// 消失动画速度
  ui.param[BTN_SPT] = user_config[BTN_SPT];   		// 按键短按时长
  ui.param[BTN_LPT] = user_config[BTN_LPT];  			// 按键长按时长
  ui.param[TILE_UFD] = user_config[TILE_UFD];   	// 磁贴图标从头展开开关
  ui.param[LIST_UFD] = user_config[LIST_UFD];   	// 菜单列表从头展开开关
  ui.param[TILE_LOOP] = user_config[TILE_LOOP];  	// 磁贴图标循环模式开关
  ui.param[LIST_LOOP] = user_config[LIST_LOOP];  	// 菜单列表循环模式开关
  ui.param[WIN_BOK] = user_config[WIN_BOK];    		// 弹窗背景虚化开关
  ui.param[KNOB_DIR] = user_config[KNOB_DIR];   	// 旋钮方向切换开关
  ui.param[DARK_MODE] = user_config[DARK_MODE];  	// 黑暗模式开关
	
  ir_sw_set_time(user_config[UI_PARAM]);  				// 翻转计数时间
	ir_sw_set_count((user_config[UI_PARAM+1]<<24) | (user_config[UI_PARAM+2]<<16) | (user_config[UI_PARAM+3]<<8) | (user_config[UI_PARAM+4]));	// 触发计数
	
	free(user_config);
}

// u8g2初始化函数
void u8g2_init(void)
{
  buf_ptr = u8g2_GetBufferPtr(&u8g2);
  buf_len = 8 * u8g2_GetBufferTileHeight(&u8g2) * u8g2_GetBufferTileWidth(&u8g2);
	u8g2_SetContrast(&u8g2, ui.param[DISP_BRI]);
}

void gui_btn_send_signal(uint8_t btn_id, uint8_t event)
{
  static uint8_t flag[3] = {0}; // 解决长按松开还会触发一下按键抬起的，又发送一次的问题
  if (btn_id == 1 && event == PRESS_UP)
  {
    if (flag[0] == 0)
    {
      btn.id = BTN_ID_CC;
      btn.pressed = true;
    }
    else
    {
      flag[0] = 0;
    }
  }
  else if (btn_id == 2 && event == PRESS_UP)
  {
    if (flag[1] == 0)
    {
      btn.id = BTN_ID_SP;
      btn.pressed = true;
    }
    else
    {
      flag[1] = 0;
    }
  }
  else if (btn_id == 3 && event == PRESS_UP)
  {
    if (flag[2] == 0)
    {
      btn.id = BTN_ID_CW;
      btn.pressed = true;
    }
    else
    {
      flag[2] = 0;
    }
  }
  else if (btn_id == 1 && event == LONG_PRESS_HOLD)
  {
    btn.id = BTN_ID_CC;
    btn.pressed = true;
    flag[0] = 1;
  }
  else if (btn_id == 2 && event == LONG_PRESS_START)
  {
    btn.id = BTN_ID_LP;
    btn.pressed = true;
    flag[1] = 1;
  }
  else if (btn_id == 3 && event == LONG_PRESS_HOLD)
  {
    btn.id = BTN_ID_CW;
    btn.pressed = true;
    flag[2] = 1;
  }
}

void gui_task(void)
{
  ui_proc();
	if(eeprom_change && ((HAL_GetTick() & 60000) == 0))	//最快60秒间隔
	{
		eeprom_change = false;
		eeprom_write_all_data();
	}
}

void gui_init(void)
{
	eeprom_read_all_data();
	oled_init();
  ui_init();
  u8g2_init();
}
