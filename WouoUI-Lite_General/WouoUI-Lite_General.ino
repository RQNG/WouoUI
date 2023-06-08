
/*
  此项目模仿自稚晖君暂未开源的 MonoUI，用于实现类似 UltraLink 的丝滑界面

  有四个版本：

    * 分辨率 128 * 128 ：主菜单，列表和关于本机页基本还原了 UltraLink 的界面和动态效果，电压测量页为原创设计
    * 分辨率 128 * 64 ：主菜单模仿 UltraLink 重新设计，去掉了小标题，电压测量页重新设计，关于本机页面改为列表，列表适配了该分辨率
    * 分辨率 128 * 32 ：在 128 * 64 分辨率的基础上，主界面只保留图标，电压测量页重新设计
    * 通用版本：仅保留列表，主菜单也改为列表，删除电压测量页和与列表无关的动画（保留弹窗效果），经过简单修改可以适配任何分辨率，任何行高度的情况

  WouoUI v2.1 更新内容：

    * 修复EC11旋钮使界面卡死的问题，感谢 GitHub 安红豆 提供的线索。

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

  本项目使用Apache 2.0开源协议，如需商用或借鉴，请在醒目处标注本项目开源地址
  欢迎关注我的B站账号，用户名：音游玩的人，B站主页：https://space.bilibili.com/9182439?spm_id_from=..0.0
*/

/************************************* 屏幕驱动 *************************************/

//分辨率128*64，可以使用硬件IIC接口

#include <U8g2lib.h>
#include <Wire.h>

#define   SCL   PB6
#define   SDA   PB7
#define   RST   U8X8_PIN_NONE

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, RST);     // 分辨率：128*64  驱动：SSD1306  接口：IIC（硬件）

/************************************* 定义页面 *************************************/

//总目录，缩进表示页面层级
enum 
{
  M_WINDOW,
  M_SLEEP,
    M_MAIN, 
      M_EDITOR,
        M_KNOB,
          M_KRF,
          M_KPF,
      M_SETTING,
        M_ABOUT,
};

//状态，初始化标签
enum
{
  S_FADE,       //转场动画
  S_WINDOW,     //弹窗初始化
  S_LAYER_IN,   //层级初始化
  S_LAYER_OUT,  //层级初始化
  S_NONE,       //直接选择页面
};

//菜单结构体
typedef struct MENU
{
  char *m_select;
} M_SELECT;

/************************************* 定义内容 *************************************/

M_SELECT main_menu[]
{
  {"[ Main Menu ]"},
  {"- Editor"},
  {"- Setting"},
};

M_SELECT editor_menu[]
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

M_SELECT knob_menu[]
{
  {"[ Knob ]"},
  {"# Rotate Func"},
  {"$ Press Func"},
};

M_SELECT krf_menu[]
{
  {"[ Rotate Function ]"},
  {"--------------------------"},
  {"= Disable"},
  {"--------------------------"},
  {"= Volume"},
  {"= Brightness"},
  {"--------------------------"},
};

M_SELECT kpf_menu[]
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

M_SELECT setting_menu[]
{
  {"[ Setting ]"},
  {"~ Disp Bri"},
  {"~ List Ani"},
  {"~ Win Ani"},
  {"~ Fade Ani"},
  {"~ Btn SPT"},
  {"~ Btn LPT"},
  {"+ L Ufd Fm Scr"},
  {"+ L Loop Mode"},
  {"+ Win Bokeh Bg"},
  {"+ Knob Rot Dir"},
  {"+ Dark Mode"},
  {"- [ About ]"},
};

M_SELECT about_menu[]
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

/************************************* 页面变量 *************************************/

//OLED变量
#define   DISP_H              64    //屏幕高度
#define   DISP_W              128   //屏幕宽度
uint8_t   *buf_ptr;                 //指向屏幕缓冲的指针
uint16_t  buf_len;                  //缓冲长度

//UI变量
#define   UI_DEPTH            20    //最深层级数
#define   UI_MNUMB            100   //菜单数量
#define   UI_PARAM            11    //参数数量
enum 
{
  DISP_BRI,     //屏幕亮度
  LIST_ANI,     //列表动画速度
  WIN_ANI,      //弹窗动画速度
  FADE_ANI,     //消失动画速度
  BTN_SPT,      //按键短按时长
  BTN_LPT,      //按键长按时长
  LIST_UFD,     //菜单列表从头展开开关
  LIST_LOOP,    //菜单列表循环模式开关
  WIN_BOK,      //弹窗背景虚化开关
  KNOB_DIR,     //旋钮方向切换开关
  DARK_MODE,    //黑暗模式开关
};
struct 
{
  bool      init;
  uint8_t   num[UI_MNUMB];
  uint8_t   select[UI_DEPTH];
  uint8_t   layer;
  uint8_t   index = M_SLEEP;
  uint8_t   state = S_NONE;
  bool      sleep = true;
  uint8_t   fade = 1;
  uint8_t   param[UI_PARAM];
} ui;

//列表变量
//默认参数
/*
#define   LIST_FONT           u8g2_font_HelvetiPixel_tr   //列表字体
#define   LIST_TEXT_H         8                           //列表每行文字字体的高度
#define   LIST_LINE_H         16                          //列表单行高度
#define   LIST_TEXT_S         4                           //列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define   LIST_BAR_W          5                           //列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define   LIST_BOX_R          0.5                         //列表选择框圆角
*/

//超窄行高度测试
#define   LIST_FONT           u8g2_font_4x6_tr            //列表字体
#define   LIST_TEXT_H         5                           //列表每行文字字体的高度
#define   LIST_LINE_H         7                           //列表单行高度
#define   LIST_TEXT_S         1                           //列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define   LIST_BAR_W          7                           //列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define   LIST_BOX_R          0.5                         //列表选择框圆角

struct
{
  uint8_t line_n = DISP_H / LIST_LINE_H;
  int16_t temp;
  bool    loop;
  float   y;
  float   y_trg;
  float   box_x;
  float   box_x_trg;
  float   box_y;
  float   box_y_trg[UI_DEPTH];
  float   bar_y;
  float   bar_y_trg;
} list;

//选择框变量
/*
//默认参数
#define   CHECK_BOX_L_S       95                          //选择框在每行的左边距
#define   CHECK_BOX_U_S       2                           //选择框在每行的上边距
#define   CHECK_BOX_F_W       12                          //选择框外框宽度
#define   CHECK_BOX_F_H       12                          //选择框外框高度
#define   CHECK_BOX_D_S       2                           //选择框里面的点距离外框的边距
*/

//超窄行高度测试
#define   CHECK_BOX_L_S       99                          //选择框在每行的左边距
#define   CHECK_BOX_U_S       0                           //选择框在每行的上边距
#define   CHECK_BOX_F_W       5                           //选择框外框宽度
#define   CHECK_BOX_F_H       5                           //选择框外框高度
#define   CHECK_BOX_D_S       1                           //选择框里面的点距离外框的边距

struct
{
  uint8_t *v;
  uint8_t *m;
  uint8_t *s;
  uint8_t *s_p;
} check_box;

//弹窗变量
#define   WIN_FONT            u8g2_font_HelvetiPixel_tr   //弹窗字体
#define   WIN_H               32                          //弹窗高度
#define   WIN_W               102                         //弹窗宽度
#define   WIN_BAR_W           92                          //弹窗进度条宽度
#define   WIN_BAR_H           7                           //弹窗进度条高度
#define   WIN_Y               - WIN_H - 2                 //弹窗竖直方向出场起始位置
#define   WIN_Y_TRG           - WIN_H - 2                 //弹窗竖直方向退场终止位置
struct
{
  //uint8_t
  uint8_t   *value;
  uint8_t   max;
  uint8_t   min;
  uint8_t   step;

  MENU      *bg;
  uint8_t   index;
  char      title[20];
  uint8_t   select;
  uint8_t   l = (DISP_W - WIN_W) / 2;
  uint8_t   u = (DISP_H - WIN_H) / 2;
  float     bar;
  float     bar_trg;
  float     y;
  float     y_trg;
} win;

/************************************* USB 相关 *************************************/

#include <USBComposite.h>

USBHID HID;

const uint8_t reportDescription[] = 
{
   HID_CONSUMER_REPORT_DESCRIPTOR(),
   HID_KEYBOARD_REPORT_DESCRIPTOR()
};

HIDConsumer Consumer(HID);
HIDKeyboard Keyboard(HID);

void hid_init()
{
  HID.begin(reportDescription, sizeof(reportDescription));
  while (!USBComposite);
}

/********************************** 自定义功能变量 **********************************/

//旋钮功能变量
#define   KNOB_PARAM          4
#define   KNOB_DISABLE        0
#define   KNOB_ROT_VOL        1
#define   KNOB_ROT_BRI        2
enum 
{
  KNOB_ROT,       //睡眠下旋转旋钮的功能，0 禁用，1 音量，2 亮度
  KNOB_COD,       //睡眠下短按旋钮输入的字符码，0 禁用
  KNOB_ROT_P,     //旋转旋钮功能在单选框中选择的位置
  KNOB_COD_P,     //字符码在单选框中选择的位置
};
struct 
{
  uint8_t param[KNOB_PARAM] = { KNOB_DISABLE, KNOB_DISABLE, 2, 2 }; //禁用在列表的第2个选项，第0个是标题，第1个是分界线
} knob;

/************************************* 断电保存 *************************************/

#include <EEPROM.h>

//EEPROM变量
#define   EEPROM_CHECK        11
struct
{
  bool    init;
  bool    change;
  int     address;
  uint8_t check;
  uint8_t check_param[EEPROM_CHECK] = { 'a', 'b', 'c', 'd', 'e', 'f','g', 'h', 'i', 'j', 'k' }; 
} eeprom;

//EEPROM写数据，回到睡眠时执行一遍
void eeprom_write_all_data()
{
  eeprom.address = 0;
  for (uint8_t i = 0; i < EEPROM_CHECK; ++i)    EEPROM.write(eeprom.address + i, eeprom.check_param[i]);  eeprom.address += EEPROM_CHECK;
  for (uint8_t i = 0; i < UI_PARAM; ++i)        EEPROM.write(eeprom.address + i, ui.param[i]);            eeprom.address += UI_PARAM;
  for (uint8_t i = 0; i < KNOB_PARAM; ++i)      EEPROM.write(eeprom.address + i, knob.param[i]);          eeprom.address += KNOB_PARAM;
}

//EEPROM读数据，开机初始化时执行一遍
void eeprom_read_all_data()
{
  eeprom.address = EEPROM_CHECK;   
  for (uint8_t i = 0; i < UI_PARAM; ++i)        ui.param[i]   = EEPROM.read(eeprom.address + i);          eeprom.address += UI_PARAM;
  for (uint8_t i = 0; i < KNOB_PARAM; ++i)      knob.param[i] = EEPROM.read(eeprom.address + i);          eeprom.address += KNOB_PARAM;
}

//开机检查是否已经修改过，没修改过则跳过读配置步骤，用默认设置
void eeprom_init()
{
  eeprom.check = 0;
  eeprom.address = 0; for (uint8_t i = 0; i < EEPROM_CHECK; ++i)  if (EEPROM.read(eeprom.address + i) != eeprom.check_param[i])  eeprom.check ++;
  if (eeprom.check <= 1) eeprom_read_all_data();  //允许一位误码
  else ui_param_init();
}

/************************************* 旋钮相关 *************************************/

//可按下旋钮引脚
#define   AIO   PB12
#define   BIO   PB13
#define   SW    PB14

//按键ID
#define   BTN_ID_CC           0   //逆时针旋转
#define   BTN_ID_CW           1   //顺时针旋转
#define   BTN_ID_SP           2   //短按
#define   BTN_ID_LP           3   //长按

//按键变量
#define   BTN_PARAM_TIMES     2   //由于uint8_t最大值可能不够，但它存储起来方便，这里放大两倍使用
struct
{
  uint8_t   id;
  bool      flag;
  bool      pressed;
  bool      CW_1;
  bool      CW_2;
  bool      val;
  bool      val_last;  
  bool      alv;  
  bool      blv;
  long      count;
} volatile btn;

void knob_inter() 
{
  btn.alv = digitalRead(AIO);
  btn.blv = digitalRead(BIO);
  if (!btn.flag && btn.alv == LOW) 
  {
    btn.CW_1 = btn.blv;
    btn.flag = true;
  }
  if (btn.flag && btn.alv) 
  {
    btn.CW_2 = !btn.blv;
    if (btn.CW_1 && btn.CW_2)
     {
      btn.id = ui.param[KNOB_DIR];
      btn.pressed = true;
    }
    if (btn.CW_1 == false && btn.CW_2 == false) 
    {
      btn.id = !ui.param[KNOB_DIR];
      btn.pressed = true;
    }
    btn.flag = false;
  }
}

void btn_scan() 
{
  btn.val = digitalRead(SW);
  if (btn.val != btn.val_last)
  {
    btn.val_last = btn.val;
    delay(ui.param[BTN_SPT] * BTN_PARAM_TIMES);
    btn.val = digitalRead(SW);
    if (btn.val == LOW)
    {
      btn.pressed = true;
      btn.count = 0;
      while (!digitalRead(SW))
      {
        btn.count++;
        delay(1);
      }
      if (btn.count < ui.param[BTN_LPT] * BTN_PARAM_TIMES)  btn.id = BTN_ID_SP;
      else  btn.id = BTN_ID_LP;
    }
  }
}

void btn_init() 
{
  pinMode(AIO, INPUT);
  pinMode(BIO, INPUT);
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(AIO), knob_inter, CHANGE);
}

/************************************ 初始化函数 ***********************************/

/********************************* 初始化数据处理函数 *******************************/

//显示数值的初始化
void check_box_v_init(uint8_t *param)
{
  check_box.v = param;
}

//多选框的初始化
void check_box_m_init(uint8_t *param)
{
  check_box.m = param;
}

//单选框时的初始化
void check_box_s_init(uint8_t *param, uint8_t *param_p)
{
  check_box.s = param;
  check_box.s_p = param_p;
}

//多选框处理函数
void check_box_m_select(uint8_t param)
{
  check_box.m[param] = !check_box.m[param];
  eeprom.change = true;
}

//单选框处理函数
void check_box_s_select(uint8_t val, uint8_t pos)
{
  *check_box.s = val;
  *check_box.s_p = pos;
  eeprom.change = true;
}

//弹窗数值初始化
void window_value_init(char title[], uint8_t select, uint8_t *value, uint8_t max, uint8_t min, uint8_t step, MENU *bg, uint8_t index)
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

/*********************************** UI 初始化函数 *********************************/

//在初始化EEPROM时，选择性初始化的默认设置
void ui_param_init()
{
  ui.param[DISP_BRI]  = 255;      //屏幕亮度
  ui.param[LIST_ANI]  = 60;       //列表动画速度
  ui.param[WIN_ANI]   = 25;       //弹窗动画速度
  ui.param[FADE_ANI]  = 30;       //消失动画速度
  ui.param[BTN_SPT]   = 25;       //按键短按时长
  ui.param[BTN_LPT]   = 150;      //按键长按时长
  ui.param[LIST_UFD]  = 1;        //菜单列表从头展开开关
  ui.param[LIST_LOOP] = 0;        //菜单列表循环模式开关
  ui.param[WIN_BOK]   = 0;        //弹窗背景虚化开关
  ui.param[KNOB_DIR]  = 0;        //旋钮方向切换开关   
  ui.param[DARK_MODE] = 1;        //黑暗模式开关   
}

//列表类页面列表行数初始化，必须初始化的参数
void ui_init()
{
  ui.num[M_MAIN]      = sizeof( main_menu     )   / sizeof(M_SELECT);
  ui.num[M_EDITOR]    = sizeof( editor_menu   )   / sizeof(M_SELECT);
  ui.num[M_KNOB]      = sizeof( knob_menu     )   / sizeof(M_SELECT);
  ui.num[M_KRF]       = sizeof( krf_menu      )   / sizeof(M_SELECT);
  ui.num[M_KPF]       = sizeof( kpf_menu      )   / sizeof(M_SELECT);
  ui.num[M_SETTING]   = sizeof( setting_menu  )   / sizeof(M_SELECT);
  ui.num[M_ABOUT]     = sizeof( about_menu    )   / sizeof(M_SELECT);   
}

/********************************* 分页面初始化函数 ********************************/

//进入睡眠时的初始化
void sleep_param_init()
{
  u8g2.setDrawColor(0);
  u8g2.drawBox(0, 0, DISP_W, DISP_H);
  u8g2.setPowerSave(1);
  ui.state = S_NONE;  
  ui.sleep = true;
  if (eeprom.change)
  {
    eeprom_write_all_data();
    eeprom.change = false;
  }
}

//旋钮设置页初始化
void knob_param_init() { check_box_v_init(knob.param); }

//旋钮旋转页初始化
void krf_param_init() { check_box_s_init(&knob.param[KNOB_ROT], &knob.param[KNOB_ROT_P]); }

//旋钮点按页初始化
void kpf_param_init() { check_box_s_init(&knob.param[KNOB_COD], &knob.param[KNOB_COD_P]); }

//设置页初始化
void setting_param_init()
{
  check_box_v_init(ui.param);
  check_box_m_init(ui.param);
}

/********************************** 通用初始化函数 *********************************/

/*
  页面层级管理逻辑是，把所有页面都先当作列表类初始化，不是列表类按需求再初始化对应函数
  这样做会浪费一些资源，但跳转页面时只需要考虑页面层级，逻辑上更清晰，减少出错
*/

//弹窗动画初始化
void window_param_init()
{
  win.bar = 0;
  win.y = WIN_Y;
  win.y_trg = win.u;
  ui.state = S_NONE;
}

//进入更深层级时的初始化
void layer_init_in()
{
  ui.layer ++;
  ui.init = 0;
  list.y = 0;
  list.y_trg = LIST_LINE_H;
  list.box_x = 0;
  list.box_y = 0;
  list.bar_y = 0;
  ui.state = S_FADE;
  switch (ui.index)
  {   
    case M_KNOB:    knob_param_init();    break;  //旋钮设置页，行末尾文字初始化
    case M_KRF:     krf_param_init();     break;  //旋钮旋转页，单选框初始化  
    case M_KPF:     kpf_param_init();     break;  //旋钮点按页，单选框初始化  
    case M_SETTING: setting_param_init(); break;  //主菜单进入设置页，单选框初始化
  }
}

//进入更浅层级时的初始化
void layer_init_out()
{
  ui.select[ui.layer] = 0;
  list.box_y_trg[ui.layer] = 0;
  ui.layer --;
  ui.init = 0;
  list.y = 0;
  list.y_trg = LIST_LINE_H;
  list.bar_y = 0;
  ui.state = S_FADE;
  switch (ui.index)
  {
    case M_SLEEP: sleep_param_init(); break;    //主菜单进入睡眠页，检查是否需要写EEPROM
  }
}

/************************************* 动画函数 *************************************/

//动画函数
void animation(float *a, float *a_trg, uint8_t n)
{
  if (fabs(*a - *a_trg) < 0.15) *a = *a_trg;
  if (*a != *a_trg) *a += (*a_trg - *a) / (ui.param[n] / 10.0);
}

//消失函数
void fade()
{
  delay(ui.param[FADE_ANI]);
  if (ui.param[DARK_MODE])
  {
    switch (ui.fade)
    {
      case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA; break;
      case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
      case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55; break;
      case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
      default: ui.state = S_NONE; ui.fade = 0; break;
    }
  }
  else
  {
    switch (ui.fade)
    {
      case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] | 0xAA; break;
      case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] | 0x00; break;
      case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] | 0x55; break;
      case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] | 0x00; break;
      default: ui.state = S_NONE; ui.fade = 0; break;
    }    
  }
  ui.fade++;
}

/************************************* 显示函数 *************************************/

/*************** 根据列表每行开头符号，判断每行尾部是否绘制以及绘制什么内容 *************/

//列表显示数值
void list_draw_value(int n) { u8g2.print(check_box.v[n - 1]); }

//绘制外框
void list_draw_check_box_frame() { u8g2.drawRFrame(CHECK_BOX_L_S, list.temp + CHECK_BOX_U_S, CHECK_BOX_F_W, CHECK_BOX_F_H, 1); }

//绘制框里面的点
void list_draw_check_box_dot() { u8g2.drawBox(CHECK_BOX_L_S + CHECK_BOX_D_S + 1, list.temp + CHECK_BOX_U_S + CHECK_BOX_D_S + 1, CHECK_BOX_F_W - (CHECK_BOX_D_S + 1) * 2, CHECK_BOX_F_H - (CHECK_BOX_D_S + 1) * 2); }

//列表显示旋钮功能
void list_draw_krf(int n) 
{ 
  switch (check_box.v[n - 1])
  {
    case 0: u8g2.print("OFF"); break;
    case 1: u8g2.print("VOL"); break;
    case 2: u8g2.print("BRI"); break;
  }
}

//列表显示按键键值
void list_draw_kpf(int n) 
{ 
  if (check_box.v[n - 1] == 0) u8g2.print("OFF");
  else if (check_box.v[n - 1] <= 90) u8g2.print((char)check_box.v[n - 1]);
  else u8g2.print("?");
}

//判断列表尾部内容
void list_draw_text_and_check_box(struct MENU arr[], int i)
{
  u8g2.drawStr(LIST_TEXT_S, list.temp + LIST_TEXT_H + LIST_TEXT_S, arr[i].m_select);
  u8g2.setCursor(CHECK_BOX_L_S, list.temp + LIST_TEXT_H + LIST_TEXT_S);
  switch (arr[i].m_select[0])
  {
    case '~': list_draw_value(i); break;
    case '+': list_draw_check_box_frame(); if (check_box.m[i - 1] == 1)  list_draw_check_box_dot(); break;
    case '=': list_draw_check_box_frame(); if (*check_box.s_p == i)      list_draw_check_box_dot(); break;
    case '#': list_draw_krf(i);   break;
    case '$': list_draw_kpf(i);   break;
  }
}

/******************************** 列表显示函数 **************************************/

//列表类页面通用显示函数
void list_show(struct MENU arr[], uint8_t ui_index)
{
  //更新动画目标值
  u8g2.setFont(LIST_FONT);
  list.box_x_trg = u8g2.getStrWidth(arr[ui.select[ui.layer]].m_select) + LIST_TEXT_S * 2;
  list.bar_y_trg = ceil((ui.select[ui.layer]) * ((float)DISP_H / (ui.num[ui_index] - 1)));
  
  //计算动画过渡值
  animation(&list.y, &list.y_trg, LIST_ANI);
  animation(&list.box_x, &list.box_x_trg, LIST_ANI);
  animation(&list.box_y, &list.box_y_trg[ui.layer], LIST_ANI);
  animation(&list.bar_y, &list.bar_y_trg, LIST_ANI);

  //检查循环动画是否结束
  if (list.loop && list.box_y == list.box_y_trg[ui.layer]) list.loop = false;

  //设置文字和进度条颜色，0透显，1实显，2反色，这里都用实显
  u8g2.setDrawColor(1);
  
  //绘制进度条
  u8g2.drawHLine(DISP_W - LIST_BAR_W, 0, LIST_BAR_W);
  u8g2.drawHLine(DISP_W - LIST_BAR_W, DISP_H - 1, LIST_BAR_W);
  u8g2.drawVLine(DISP_W - ceil((float)LIST_BAR_W / 2), 0, DISP_H);
  u8g2.drawBox(DISP_W - LIST_BAR_W, 0, LIST_BAR_W, list.bar_y);

  //绘制列表文字  
  if (!ui.init)
  {
    for (int i = 0; i < ui.num[ui_index]; ++ i)
    {
      if (ui.param[LIST_UFD]) list.temp = i * list.y - LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
      else list.temp = (i - ui.select[ui.layer]) * list.y + list.box_y_trg[ui.layer];
      list_draw_text_and_check_box(arr, i);
    }
    if (list.y == list.y_trg) 
    {
      ui.init = true;
      list.y = list.y_trg = - LIST_LINE_H * ui.select[ui.layer] + list.box_y_trg[ui.layer];
    }
  }
  else for (int i = 0; i < ui.num[ui_index]; ++ i)
  {
    list.temp = LIST_LINE_H * i + list.y;
    list_draw_text_and_check_box(arr, i);
  }

  //绘制文字选择框，0透显，1实显，2反色，这里用反色
  u8g2.setDrawColor(2);
  u8g2.drawRBox(0, list.box_y, list.box_x, LIST_LINE_H, LIST_BOX_R);

  //反转屏幕内元素颜色，白天模式遮罩，在这里屏蔽有列表参与的页面，使遮罩作用在那个页面上
  if (!ui.param[DARK_MODE])
  {
    u8g2.drawBox(0, 0, DISP_W, DISP_H);
    switch(ui.index)
    {
      case M_WINDOW: 
      u8g2.drawBox(0, 0, DISP_W, DISP_H);  
    }
  }
}

//弹窗通用显示函数
void window_show()
{
  //绘制背景列表，根据开关判断背景是否要虚化
  list_show(win.bg, win.index);
  if (ui.param[WIN_BOK]) for (uint16_t i = 0; i < buf_len; ++i)  buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);

  //更新动画目标值
  u8g2.setFont(WIN_FONT);
  win.bar_trg = (float)(*win.value - win.min) / (float)(win.max - win.min) * (WIN_BAR_W - 4);

  //计算动画过渡值
  animation(&win.bar, &win.bar_trg, WIN_ANI);
  animation(&win.y, &win.y_trg, WIN_ANI);

  //绘制窗口
  u8g2.setDrawColor(0); u8g2.drawRBox(win.l, (int16_t)win.y, WIN_W, WIN_H, 2);    //绘制外框背景
  u8g2.setDrawColor(1); u8g2.drawRFrame(win.l, (int16_t)win.y, WIN_W, WIN_H, 2);  //绘制外框描边
  u8g2.drawRFrame(win.l + 5, (int16_t)win.y + 20, WIN_BAR_W, WIN_BAR_H, 1);       //绘制进度条外框
  u8g2.drawBox(win.l + 7, (int16_t)win.y + 22, win.bar, WIN_BAR_H - 4);           //绘制进度条
  u8g2.setCursor(win.l + 5, (int16_t)win.y + 14); u8g2.print(win.title);          //绘制标题
  u8g2.setCursor(win.l + 78, (int16_t)win.y + 14); u8g2.print(*win.value);        //绘制当前值
  
  //需要在窗口修改参数时立即见效的函数
  if (!strcmp(win.title, "Disp Bri")) u8g2.setContrast(ui.param[DISP_BRI]);

  //反转屏幕内元素颜色，白天模式遮罩
  u8g2.setDrawColor(2);
  if (!ui.param[DARK_MODE]) u8g2.drawBox(0, 0, DISP_W, DISP_H);
}

/************************************* 处理函数 *************************************/

/*********************************** 通用处理函数 ***********************************/

//列表类页面旋转时判断通用函数
void list_rotate_switch()
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
            else list.box_y_trg[ui.layer] = (ui.num[ui.index] - 1) * LIST_LINE_H;
            break;
          }
          else break;
        }
        if (ui.init)
        {
          ui.select[ui.layer] -= 1;
          if (ui.select[ui.layer] < - (list.y_trg / LIST_LINE_H)) 
          {
            if (!(DISP_H % LIST_LINE_H)) list.y_trg += LIST_LINE_H;
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
              else list.y_trg += LIST_LINE_H;
            }
          }
          else list.box_y_trg[ui.layer] -= LIST_LINE_H;
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
          else break;
        }
        if (ui.init)
        {
          ui.select[ui.layer] += 1;
          if ((ui.select[ui.layer] + 1) > (list.line_n - list.y_trg / LIST_LINE_H))
          {
            if (!(DISP_H % LIST_LINE_H)) list.y_trg -= LIST_LINE_H;
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
              else list.y_trg -= LIST_LINE_H;
            }
          }
          else list.box_y_trg[ui.layer] += LIST_LINE_H;
          break;
        }
        break;
    }
  }
}

//弹窗通用处理函数
void window_proc()
{
  window_show();
  if (win.y == WIN_Y_TRG) ui.index = win.index;
  if (btn.pressed && win.y == win.y_trg && win.y != WIN_Y_TRG)
  {
    btn.pressed = false;
    switch (btn.id)
    {
      case BTN_ID_CW: if (*win.value < win.max)  *win.value += win.step;  eeprom.change = true;  break;
      case BTN_ID_CC: if (*win.value > win.min)  *win.value -= win.step;  eeprom.change = true;  break;  
      case BTN_ID_SP: case BTN_ID_LP: win.y_trg = WIN_Y_TRG; break;
    }
  }
}

/********************************** 分页面处理函数 **********************************/

//睡眠页面处理函数
void sleep_proc()
{
  while (ui.sleep)
  {
    //睡眠时循环执行的函数

    //睡眠时需要扫描旋钮才能退出睡眠
    btn_scan();

    //当旋钮有动作时
    if (btn.pressed) { btn.pressed = false; switch (btn.id) {    

        //睡眠时顺时针旋转执行的函数
        case BTN_ID_CW:
          switch (knob.param[KNOB_ROT])
          {
            case KNOB_ROT_VOL: Consumer.press(HIDConsumer::VOLUME_UP);       Consumer.release(); break;
            case KNOB_ROT_BRI: Consumer.press(HIDConsumer::BRIGHTNESS_UP);   Consumer.release(); break;
          }
          break;

        //睡眠时逆时针旋转执行的函数
        case BTN_ID_CC:
          switch (knob.param[KNOB_ROT])
          {
            case KNOB_ROT_VOL: Consumer.press(HIDConsumer::VOLUME_DOWN);     Consumer.release(); break;
            case KNOB_ROT_BRI: Consumer.press(HIDConsumer::BRIGHTNESS_DOWN); Consumer.release(); break;
          }
          break;

        //睡眠时短按执行的函数
        case BTN_ID_SP: Keyboard.press(knob.param[KNOB_COD]);  Keyboard.release(knob.param[KNOB_COD]); break;   
        
        //睡眠时长按执行的函数
        case BTN_ID_LP: ui.index = M_MAIN;  ui.state = S_LAYER_IN; u8g2.setPowerSave(0); ui.sleep = false; break;
      }
    }
  }
}

//主菜单处理函数，无选项框列表类模板
void main_proc()
{
  list_show(main_menu, M_MAIN); 
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
        
        case 0:   ui.index = M_SLEEP;   ui.state = S_LAYER_OUT; break;
        case 1:   ui.index = M_EDITOR;  ui.state = S_LAYER_IN;  break;
        case 2:   ui.index = M_SETTING; ui.state = S_LAYER_IN;  break;
      }
    }
  }
}

//编辑器菜单处理函数
void editor_proc()
{
  list_show(editor_menu, M_EDITOR); 
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
        
        case 0:   ui.index = M_MAIN;  ui.state = S_LAYER_OUT; break;
        case 11:  ui.index = M_KNOB;  ui.state = S_LAYER_IN;  break;
      }
    }
  }
}

//旋钮编辑菜单处理函数
void knob_proc()
{
  list_show(knob_menu, M_KNOB);
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
        
        case 0: ui.index = M_EDITOR;  ui.state = S_LAYER_OUT; break;
        case 1: ui.index = M_KRF;     ui.state = S_LAYER_IN;  check_box_s_init(&knob.param[KNOB_ROT], &knob.param[KNOB_ROT_P]); break;
        case 2: ui.index = M_KPF;     ui.state = S_LAYER_IN;  check_box_s_init(&knob.param[KNOB_COD], &knob.param[KNOB_COD_P]); break;
      }
    }
  }
}

//旋钮旋转功能菜单处理函数
void krf_proc()
{
  list_show(krf_menu, M_KRF);
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
        
        case 0: ui.index = M_KNOB;  ui.state = S_LAYER_OUT; break;
        case 1: break;
        case 2: check_box_s_select(KNOB_DISABLE, ui.select[ui.layer]); break;
        case 3: break;
        case 4: check_box_s_select(KNOB_ROT_VOL, ui.select[ui.layer]); break;
        case 5: check_box_s_select(KNOB_ROT_BRI, ui.select[ui.layer]); break;
        case 6: break;
      }
    }
  }
}

//旋钮点按功能菜单处理函数
void kpf_proc()
{
  list_show(kpf_menu, M_KPF);
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break;  case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
    
        case 0:   ui.index = M_KNOB;  ui.state = S_LAYER_OUT; break;
        case 1:   break;
        case 2:   check_box_s_select(KNOB_DISABLE, ui.select[ui.layer]); break;
        case 3:   break;
        case 4:   check_box_s_select('A', ui.select[ui.layer]); break;
        case 5:   check_box_s_select('B', ui.select[ui.layer]); break;
        case 6:   check_box_s_select('C', ui.select[ui.layer]); break;
        case 7:   check_box_s_select('D', ui.select[ui.layer]); break;
        case 8:   check_box_s_select('E', ui.select[ui.layer]); break;
        case 9:   check_box_s_select('F', ui.select[ui.layer]); break;
        case 10:  check_box_s_select('G', ui.select[ui.layer]); break;
        case 11:  check_box_s_select('H', ui.select[ui.layer]); break;
        case 12:  check_box_s_select('I', ui.select[ui.layer]); break;
        case 13:  check_box_s_select('J', ui.select[ui.layer]); break;
        case 14:  check_box_s_select('K', ui.select[ui.layer]); break;
        case 15:  check_box_s_select('L', ui.select[ui.layer]); break;
        case 16:  check_box_s_select('M', ui.select[ui.layer]); break;
        case 17:  check_box_s_select('N', ui.select[ui.layer]); break;
        case 18:  check_box_s_select('O', ui.select[ui.layer]); break;
        case 19:  check_box_s_select('P', ui.select[ui.layer]); break;
        case 20:  check_box_s_select('Q', ui.select[ui.layer]); break;
        case 21:  check_box_s_select('R', ui.select[ui.layer]); break;
        case 22:  check_box_s_select('S', ui.select[ui.layer]); break;
        case 23:  check_box_s_select('T', ui.select[ui.layer]); break;
        case 24:  check_box_s_select('U', ui.select[ui.layer]); break;
        case 25:  check_box_s_select('V', ui.select[ui.layer]); break;
        case 26:  check_box_s_select('W', ui.select[ui.layer]); break;
        case 27:  check_box_s_select('X', ui.select[ui.layer]); break;
        case 28:  check_box_s_select('Y', ui.select[ui.layer]); break;
        case 29:  check_box_s_select('Z', ui.select[ui.layer]); break;
        case 30:  break;
        case 31:  check_box_s_select('0', ui.select[ui.layer]); break;
        case 32:  check_box_s_select('1', ui.select[ui.layer]); break;
        case 33:  check_box_s_select('2', ui.select[ui.layer]); break;
        case 34:  check_box_s_select('3', ui.select[ui.layer]); break;
        case 35:  check_box_s_select('4', ui.select[ui.layer]); break;
        case 36:  check_box_s_select('5', ui.select[ui.layer]); break;
        case 37:  check_box_s_select('6', ui.select[ui.layer]); break;
        case 38:  check_box_s_select('7', ui.select[ui.layer]); break;
        case 39:  check_box_s_select('8', ui.select[ui.layer]); break;
        case 40:  check_box_s_select('9', ui.select[ui.layer]); break;
        case 41:  break;
        case 42:  check_box_s_select( KEY_ESC, ui.select[ui.layer]); break;
        case 43:  check_box_s_select( KEY_F1,  ui.select[ui.layer]); break;
        case 44:  check_box_s_select( KEY_F2,  ui.select[ui.layer]); break;
        case 45:  check_box_s_select( KEY_F3,  ui.select[ui.layer]); break;
        case 46:  check_box_s_select( KEY_F4,  ui.select[ui.layer]); break;
        case 47:  check_box_s_select( KEY_F5,  ui.select[ui.layer]); break;
        case 48:  check_box_s_select( KEY_F6,  ui.select[ui.layer]); break;
        case 49:  check_box_s_select( KEY_F7,  ui.select[ui.layer]); break;
        case 50:  check_box_s_select( KEY_F8,  ui.select[ui.layer]); break;
        case 51:  check_box_s_select( KEY_F9,  ui.select[ui.layer]); break;
        case 52:  check_box_s_select( KEY_F10, ui.select[ui.layer]); break;
        case 53:  check_box_s_select( KEY_F11, ui.select[ui.layer]); break;
        case 54:  check_box_s_select( KEY_F12, ui.select[ui.layer]); break;
        case 55:  break;
        case 56:  check_box_s_select( KEY_LEFT_CTRL,   ui.select[ui.layer]); break;
        case 57:  check_box_s_select( KEY_LEFT_SHIFT,  ui.select[ui.layer]); break;
        case 58:  check_box_s_select( KEY_LEFT_ALT,    ui.select[ui.layer]); break;
        case 59:  check_box_s_select( KEY_LEFT_GUI,    ui.select[ui.layer]); break;
        case 60:  check_box_s_select( KEY_RIGHT_CTRL,  ui.select[ui.layer]); break;
        case 61:  check_box_s_select( KEY_RIGHT_SHIFT, ui.select[ui.layer]); break;
        case 62:  check_box_s_select( KEY_RIGHT_ALT,   ui.select[ui.layer]); break;
        case 63:  check_box_s_select( KEY_RIGHT_GUI,   ui.select[ui.layer]); break;
        case 64:  break;
        case 65:  check_box_s_select( KEY_CAPS_LOCK,   ui.select[ui.layer]); break;
        case 66:  check_box_s_select( KEY_BACKSPACE,   ui.select[ui.layer]); break;
        case 67:  check_box_s_select( KEY_RETURN,      ui.select[ui.layer]); break;
        case 68:  check_box_s_select( KEY_INSERT,      ui.select[ui.layer]); break;
        case 69:  check_box_s_select( KEY_DELETE,      ui.select[ui.layer]); break;
        case 70:  check_box_s_select( KEY_TAB,         ui.select[ui.layer]); break;
        case 71:  break;
        case 72:  check_box_s_select( KEY_HOME,        ui.select[ui.layer]); break;
        case 73:  check_box_s_select( KEY_END,         ui.select[ui.layer]); break;
        case 74:  check_box_s_select( KEY_PAGE_UP,     ui.select[ui.layer]); break;
        case 75:  check_box_s_select( KEY_PAGE_DOWN,   ui.select[ui.layer]); break;
        case 76:  break;
        case 77:  check_box_s_select( KEY_UP_ARROW,    ui.select[ui.layer]); break;
        case 78:  check_box_s_select( KEY_DOWN_ARROW,  ui.select[ui.layer]); break;
        case 79:  check_box_s_select( KEY_LEFT_ARROW,  ui.select[ui.layer]); break;
        case 80:  check_box_s_select( KEY_RIGHT_ARROW, ui.select[ui.layer]); break;
        case 81:  break;
      }
    }
  }
}

//设置菜单处理函数，多选框列表类模板，弹窗模板
void setting_proc()
{
  list_show(setting_menu, M_SETTING);
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {
        
        //返回更浅层级，长按被当作选择这一项，也是执行这一行
        case 0:   ui.index = M_MAIN;  ui.state = S_LAYER_OUT; break;
        
        //弹出窗口，参数初始化：标题，参数名，参数值，最大值，最小值，步长，背景列表名，背景列表标签
        case 1:   window_value_init("Disp Bri", DISP_BRI, &ui.param[DISP_BRI],  255,  0,  5, setting_menu, M_SETTING);  break;
        case 2:   window_value_init("List Ani", LIST_ANI, &ui.param[LIST_ANI],  100, 10,  1, setting_menu, M_SETTING);  break;
        case 3:   window_value_init("Win Ani",  WIN_ANI,  &ui.param[WIN_ANI],   100, 10,  1, setting_menu, M_SETTING);  break;
        case 4:   window_value_init("Fade Ani", FADE_ANI, &ui.param[FADE_ANI],  255,  0,  1, setting_menu, M_SETTING);  break;
        case 5:   window_value_init("Btn SPT",  BTN_SPT,  &ui.param[BTN_SPT],   255,  0,  1, setting_menu, M_SETTING);  break;
        case 6:   window_value_init("Btn LPT",  BTN_LPT,  &ui.param[BTN_LPT],   255,  0,  1, setting_menu, M_SETTING);  break;

        //多选框
        case 7:   check_box_m_select( LIST_UFD  );  break;
        case 8:   check_box_m_select( LIST_LOOP );  break;
        case 9:   check_box_m_select( WIN_BOK   );  break;
        case 10:  check_box_m_select( KNOB_DIR  );  break;
        case 11:  check_box_m_select( DARK_MODE );  break;

        //关于本机
        case 12:  ui.index = M_ABOUT; ui.state = S_LAYER_IN; break;
      }
    }
  }
}

//关于本机页
void about_proc()
{
  list_show(about_menu, M_ABOUT);
  if (btn.pressed) { btn.pressed = false; switch (btn.id) { case BTN_ID_CW: case BTN_ID_CC: list_rotate_switch(); break; case BTN_ID_LP: ui.select[ui.layer] = 0; case BTN_ID_SP: switch (ui.select[ui.layer]) {

        case 0:   ui.index = M_SETTING;  ui.state = S_LAYER_OUT; break;
      }
    }
  }
}

//总的UI进程
void ui_proc()
{
  u8g2.sendBuffer();
  switch (ui.state)
  {
    case S_FADE:          fade();                   break;  //转场动画
    case S_WINDOW:        window_param_init();      break;  //弹窗初始化
    case S_LAYER_IN:      layer_init_in();          break;  //层级初始化
    case S_LAYER_OUT:     layer_init_out();         break;  //层级初始化
  
    case S_NONE: u8g2.clearBuffer(); switch (ui.index)      //直接选择页面
    {
      case M_WINDOW:      window_proc();            break;
      case M_SLEEP:       sleep_proc();             break;
      case M_MAIN:        main_proc();              break;
      case M_EDITOR:      editor_proc();            break;
      case M_KNOB:        knob_proc();              break;
      case M_KRF:         krf_proc();               break;
      case M_KPF:         kpf_proc();               break;
      case M_SETTING:     setting_proc();           break;
      case M_ABOUT:       about_proc();             break;
    }
  }
}

//OLED初始化函数
void oled_init()
{
  u8g2.setBusClock(1000000);  //硬件IIC接口使用
  u8g2.begin();
  u8g2.setContrast(ui.param[DISP_BRI]);
  buf_ptr = u8g2.getBufferPtr();
  buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();
}

void setup() 
{
  eeprom_init();
  ui_init();
  oled_init();
  btn_init();
  //hid_init();
}

void loop() 
{
  btn_scan();
  ui_proc();
}


