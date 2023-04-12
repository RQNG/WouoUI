/*
  此程序用于实现 wooting 键盘中的 Rapid trigger 功能。
  
  主功能移植并优化自 Github 上的一个开源项目，添加了按键的独立参数和两端的死区，优化了初始化方式，增加控制器，模仿稚晖君 MonoUI 的超丝滑菜单。

  当前版本：v2.1

    * 重构代码，只保存路径和选择框所在位置，分页面类型定义变量。
    * 增加图标和列表两种主界面风格。
    * 支持本地查看所有配置，在关于本机页面。
    * 支持128*64，128*32两种主流OLED屏幕分辨率，只需要替换驱动即可。
    * 增加列表展开动画效果。
    * 增加选择框展开和跳转动画效果。
    * 增强动画速度的可调精度。
    * 增加循环模式。
    * 增加旋钮方向反转选项。
  
  控制器实现了以下功能：

    * 丝滑菜单，模仿UltraLink的MonoUI，实现主界面动画，列表动画，聚焦动画等，可分别调节动画速度。
    * 旋钮功能，在休眠界面旋转可调节音量或亮度，点按后进入主菜单，在菜单界面旋转滚动菜单，点按选择菜单。
    * 三种模式，支持不同极性的磁轴和传感器的组合，还有禁用模式。
    * 参数设置，支持设置几乎所有参数，包括：触发值，释放值，上死区，下死区，键值，模式，初始化时放大抖动的倍数，每个按键都可以独立设置，也可以一起设置。
    * 断电保存，回到休眠模式即保存当前设置，断电后不会丢失。
    * 测试引脚，支持查看每一个引脚的电压变化情况，用于测试传感器和引脚是否正常。
  
  测试用开发板为STM32 F103C8T6，以下是使用说明：

    硬件：
    * EC11：A -> PB13 ; B -> PB12 ; S -> PB14 ; 电源3.3v
    * OLED：四线，0.96 SSD1306 128*64，SDA -> PB7 ; SCL -> PB6 ; 电源3.3v
    * 板子上的跳线帽设置：BOOT0 -> 1 ; BOOT1 -> 0 
    * 串口模块与板子连线：GND -> GND ; 3V3 -> 3.3V ; RX -> PA9 ; TX -> PA10，其余引脚空着。
    * 串口与电脑连接，用数据线连接板子上的母口和电脑。
    * 点击IDE左上角向右箭头的按钮，等待编译和上传完成。
    * 上传完成后如要测试手感，无需断开串口模块，但需要拔插数据线，电脑才能识别到是USB设备。
    * 当BOOT在第一条所述设置时，上传完成后，第一次拔插数据线时电脑会认到USB设备，从第二次开始就认不到了。
    * 手感调试合适后修改跳线帽的设置为：BOOT0 -> 0 ; BOOT1 -> 0。设置后就能收起串口模块了。
  
    软件：
    
    * 文件 -> 首选项 -> 其他开发板管理器地址 -> 添加一行：http://dan.drown.org/stm32duino/package_STM32duino_index.json
    * 工具 -> 开发板 -> 开发板管理器 -> 搜索stm32F1，下载STM32F1xx开头的库
    * 工具 -> 开发板 -> stm32F1... -> Generic STM32F103C series
    * 工具 -> 端口：“COMX” -> COMX（X为设备对应端口号）
    * 工具 -> Upload method -> Serial
    * 进入目录C:\Users{username}\AppData\Local\Arduino15\packages\stm32duino\hardware\STM32F1{版本名称}\libraries\USBComposite下，打开usb_hid.c文件，搜索bInterval，将该值修改为0x01，即轮询间隔修改为1ms
    
    注意：

    * 上传程序前，先确认使用的轴的总键程是否为4mm，如果不是，需要修改 key 数组变量中代表 travel 一行的相关参数，单位0.1mm，即4mm的值为40。
    * 拔插USB后，正常情况下，屏幕会变黑，板子上的蓝灯会点亮一秒钟，点亮时不要动按键，这是在初始化。
    * 初始化后，每个按键都需要按下一次后才能正常使用，按下时要按到底。

  遇到困难的话参考以下资料：

    设置参考：
    
    * https://github.com/rogerclarkmelbourne/Arduino_STM32
    * https://baijiahao.baidu.com/s?id=1727012582049990775&wfr=spider&for=pc
    * https://blog.csdn.net/oChiTu1/article/details/108166233?depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-2-108166233-blog-119909968.pc_relevant_3mothn_strategy_and_data_recovery
    
    项目参考：
    
    * Rapid trigger功能：https://github.com/chent7/hall-2k-keypad-handmade
    * STM32触盘：https://mp.weixin.qq.com/s/o-8_3SQS2AGT_WTyTLYQnA
    * UI：https://www.bilibili.com/video/BV1HA411S7pv/ ; https://www.bilibili.com/video/BV1xd4y1C7BE/
    
  本项目使用Apache 2.0开源协议，如需商用或借鉴，请阅读此协议。
  
  欢迎关注我的B站账号，一个只分享osu!相关内容，很无聊的帐号。
  
  用户名：音游玩的人，B站主页：https://space.bilibili.com/9182439?spm_id_from=..0.0
*/

/************************************* 屏幕和UI *************************************/

#include <U8g2lib.h>
#include <Wire.h>

#define   SCL   PB6
#define   SDA   PB7
#define   RST   U8X8_PIN_NONE

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, RST);     // 分辨率：128*64  驱动：SSD1306  接口：IIC（硬件）
//U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C  u8g2(U8G2_R0, SCL, SDA, RST); // 分辨率：128*32  驱动：SSD1306  接口：IIC（硬件）

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

/************************************* 按键相关 *************************************/

// 按键参数
#define         KEYS        10
#define         LIGHT       PC13
#define         TRIGGER     0
#define         RELEASE     1
#define         TOP         2
#define         BOTTOM      3
#define         TIMES       4
#define         CODES       5
#define         MODES       6
#define         PINS        7
#define         TRAVEL      8

// 按键变量
uint8_t         key        [9][KEYS + 1] =
{ 
  // key0  key1  key2  key3  key4  key5  key6  key7  key8  key9   all
  {    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 }, // trigger
  {    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 }, // release
  {    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 }, // top
  {    3,    3,    3,    3,    3,    3,    3,    3,    3,    3,    3 }, // bottom
  {    5,    5,    5,    5,    5,    5,    5,    5,    5,    5,    5 }, // times
  {  'a',  'b',  'c',  'd',  'e',  'f',  'g',  'h',  'i',  'j',  'k' }, // codes
  {    1,    1,    1,    1,    1,    1,    1,    1,    1,    1,    1 }, // modes
  {  PA0,  PA1,  PA2,  PA3,  PA4,  PA5,  PA6,  PA7,  PB0,  PB1,  PB1 }, // pins
  {   40,   40,   40,   40,   40,   40,   40,   40,   40,   40,   40 }, // travel
}; 

/************************************* 按键扫描 *************************************/

// 按键参数结构体
struct 
{
  int   min;
  int   max;
  int   trigger;
  int   release;
  int   value;
  int   reference;
  int   unit;
  int   cushion;
  bool  init;  
  bool  pressed;
} keyx[KEYS];

/************************************* 不分模式 *************************************/

// 初始化
void key_init() 
{
  // 打开初始化指示灯
  pinMode(LIGHT, OUTPUT);
  digitalWrite(LIGHT, LOW);

  // 读模拟量，最大值和最小值先暂定为当前值
  for (uint8_t n = 0; n < KEYS; ++n) 
  {
    pinMode(key[PINS][n],INPUT_ANALOG);
    keyx[n].value = analogRead(key[PINS][n]);
    keyx[n].min = keyx[n].value;
    keyx[n].max = keyx[n].value;
  }

  // 并行扫描所有传感器，记录每个传感器的初始抖动范围，每1ms扫描一次，扫描1s
  for (int i = 0; i < 1000; ++i) 
  {
    for (uint8_t n = 0; n < KEYS; ++n) 
    {
      keyx[n].value = analogRead(key[PINS][n]);
      if (keyx[n].value < keyx[n].min) 
      {
        keyx[n].min = keyx[n].value;
      } 
      else if (keyx[n].value > keyx[n].max) 
      {
        keyx[n].max = keyx[n].value;
      }    
    }
    delay(1);
  }

  for (uint8_t n = 0; n < KEYS; ++n) 
  {
    keyx[n].cushion = (keyx[n].max - keyx[n].min) * key[TIMES][n];      // 计算缓冲值

    // 初始化一些参数
    keyx[n].init = false;                                               // 按键还没有被首次按下
    keyx[n].reference = keyx[n].value;                                  // 初始化参考值为当前值
    keyx[n].trigger = key[TRIGGER][n] * keyx[n].cushion;                // 初始化触发值，避免插上的瞬间有输入
    keyx[n].release = key[RELEASE][n] * keyx[n].cushion;                // 初始化释放值，避免插上的瞬间有输入

    if (key[MODES][n] == 1)
    {
      keyx[n].min = keyx[n].value;                                      // 初始化最小值，暂定为当前值
      keyx[n].max = keyx[n].value + keyx[n].cushion;                    // 初始化最大值，暂定为当前值 + 缓冲值
    }
    else if (key[MODES][n] == 2)
    {
      keyx[n].min = keyx[n].value - keyx[n].cushion;                    // 初始化最小值，暂定为当前值
      keyx[n].max = keyx[n].value;                                      // 初始化最大值，暂定为当前值 + 缓冲值
    }
  }

  // 关闭初始化指示灯
  digitalWrite(LIGHT, HIGH);
}

// 转换距离，将以毫米为单位的值转换成模拟量，假设磁铁外磁场强度均匀变化，则距离与模拟量线性相关
void convertDistance(uint8_t n) 
{
  keyx[n].unit = (keyx[n].max - keyx[n].min) / key[TRAVEL][n];
  keyx[n].trigger = keyx[n].unit * key[TRIGGER][n];
  keyx[n].release = keyx[n].unit * key[RELEASE][n];
}

/************************************* 区分模式 *************************************/

/*************************************  模式 1  *************************************/

// 按下按键，发送后改变按键状态，按键首次被按下
void pressKey_1(uint8_t n) 
{
  if (keyx[n].init)
  {
    Keyboard.press(key[CODES][n]);
  } 
  else if (keyx[n].value > keyx[n].min + keyx[n].cushion)
  {
    keyx[n].init = true;
  }
  keyx[n].pressed = true;
}

// 释放按键，发送后改变按键状态，按键首次被按下
void releaseKey_1(uint8_t n) 
{
  if (keyx[n].init)
  {
    Keyboard.release(key[CODES][n]);
  } 
  else if (keyx[n].value > keyx[n].min + keyx[n].cushion)
  {
    keyx[n].init = true;
  }
  keyx[n].pressed = false;
}

// 平衡按键，按下时探索更大的值，释放时探索更小的值，以免在使用时极值漂移使抖动能误触发
void balanceKey_1(uint8_t n) 
{
  if (!keyx[n].pressed && (keyx[n].value < keyx[n].min)) 
  {
    keyx[n].min = keyx[n].value;
    convertDistance(n);
  } 
  else if (keyx[n].pressed && (keyx[n].value > keyx[n].max)) 
  {
    keyx[n].max = keyx[n].value;
    convertDistance(n);
  }
}

// 按键移动时判断状态，同时动态改变参考值
void processKey_1(uint8_t n) 
{
  // 不在死区移动时的判断
  if ((keyx[n].value > keyx[n].min + keyx[n].unit * key[TOP][n]) && (keyx[n].value < keyx[n].max - keyx[n].unit * key[BOTTOM][n]))
  {
    if (!keyx[n].pressed) 
    {
      if (keyx[n].value > keyx[n].reference + keyx[n].trigger) 
      {
        pressKey_1(n);
      }
      else if (keyx[n].value < keyx[n].reference) 
      {
        keyx[n].reference = keyx[n].value;
      }
    } 
    else 
    {
      if (keyx[n].value <= keyx[n].reference - keyx[n].release) 
      {
        releaseKey_1(n);
      }
      else if (keyx[n].value > keyx[n].reference) 
      {
        keyx[n].reference = keyx[n].value;
      }
    }
  }

  // 在死区移动时的判断
  else if ((keyx[n].value < keyx[n].min + keyx[n].unit * key[TOP][n]) && keyx[n].pressed)
  {
    releaseKey_1(n);
  }
  else if ((keyx[n].value > keyx[n].max - keyx[n].unit * key[BOTTOM][n]) && !keyx[n].pressed)
  {
    pressKey_1(n);
  }
}

/*************************************  模式 2  *************************************/

// 按下按键，发送后改变按键状态，按键首次被按下
void pressKey_2(uint8_t n) 
{
  if (keyx[n].init)
  {
    Keyboard.press(key[CODES][n]);
  } 
  else if (keyx[n].value < keyx[n].max - keyx[n].cushion)
  {
    keyx[n].init = true;
  }
  keyx[n].pressed = true;
}

// 释放按键，发送后改变按键状态，按键首次被按下
void releaseKey_2(uint8_t n) 
{
  if (keyx[n].init)
  {
    Keyboard.release(key[CODES][n]);
  } 
  else if (keyx[n].value < keyx[n].min - keyx[n].cushion)
  {
    keyx[n].init = true;
  }
  keyx[n].pressed = false;
}

// 平衡按键，按下时探索更小的值，释放时探索更大的值，以免在使用时极值漂移使抖动能误触发
void balanceKey_2(uint8_t n) 
{
  if (keyx[n].pressed && (keyx[n].value < keyx[n].min)) 
  {
    keyx[n].min = keyx[n].value;
    convertDistance(n);
  } 
  else if (!keyx[n].pressed && (keyx[n].value > keyx[n].max)) 
  {
    keyx[n].max = keyx[n].value;
    convertDistance(n);
  }
}

// 按键移动时判断状态，同时动态改变参考值
void processKey_2(uint8_t n) 
{
  // 不在死区移动时的判断
  if ((keyx[n].value > keyx[n].min + keyx[n].unit * key[BOTTOM][n]) && (keyx[n].value < keyx[n].max - keyx[n].unit * key[TOP][n]))
  {
    if (!keyx[n].pressed) 
    {
      if (keyx[n].value < keyx[n].reference - keyx[n].trigger) 
      {
        pressKey_2(n);
      }
      else if (keyx[n].value > keyx[n].reference) 
      {
        keyx[n].reference = keyx[n].value;
      }
    } 
    else 
    {
      if (keyx[n].value >= keyx[n].reference + keyx[n].release) 
      {
        releaseKey_2(n);
      }
      else if (keyx[n].value < keyx[n].reference) 
      {
        keyx[n].reference = keyx[n].value;
      }
    }
  }

  // 在死区移动时的判断
  else if ((keyx[n].value < keyx[n].min + keyx[n].unit * key[BOTTOM][n]) && !keyx[n].pressed)
  {
    pressKey_2(n);
  }
  else if ((keyx[n].value > keyx[n].max - keyx[n].unit * key[TOP][n]) && keyx[n].pressed)
  {
    releaseKey_2(n);
  }
}

// 扫描一次按键
void runKey(uint8_t n) 
{
  if (key[MODES][n] == 1)
  {
    keyx[n].value = analogRead(key[PINS][n]);
    balanceKey_1(n);
    processKey_1(n);
  }
  else if (key[MODES][n] == 2)
  {
    keyx[n].value = analogRead(key[PINS][n]);
    balanceKey_2(n);
    processKey_2(n);
  }
}

/************************************* 定义页面 *************************************/

//总目录
enum 
{
  M_SLEEP,
    M_MAIN,
      M_EDITOR,
        M_EDIT,
          M_EDITING,
          M_CODE,
            M_ALPH,
            M_NUMB,
            M_FUNC,
            M_MORE,
          M_MODE,
      M_MEASURE,
        M_VOLTAGE,
      M_SETTINGS,
        M_CONTROLLER,
        M_KNOB,
        M_ABOUT,
};

//状态
enum
{
  S_NONE,
  S_DISAPPEAR,
};

//菜单结构体
typedef struct MENU
{
  char *m_select;
} M_SELECT;

/************************************* 定义内容 *************************************/

/************************************* 文字内容 *************************************/

M_SELECT main_menu[]
{
  {"Sleep"},
  {"Editor"},
  {"Measure"},
  {"Settings"},
};

M_SELECT editor_menu[]
{
  {"<< Main Menu"},
  {"- Key 0"},
  {"- Key 1"},
  {"- Key 2"},
  {"- Key 3"},
  {"- Key 4"},
  {"- Key 5"},
  {"- Key 6"},
  {"- Key 7"},
  {"- Key 8"},
  {"- Key 9"},
  {"- All"},
};

M_SELECT edit_menu[]
{
  {"<< Select Keys"},
  {"- Trigger"},
  {"- Release"},
  {"- Top"},
  {"- Bottom"},
  {"- Times"},
  {"- Codes"},
  {"- Modes"},
};

M_SELECT code_menu[]
{
  {"<< Editor Menu"},
  {"- Alphabet"},
  {"- Number"},
  {"- Function"},
  {"- More"},
};

M_SELECT alph_menu[]
{
  {"<< Code Category"},
  {"- a"},
  {"- b"},
  {"- c"},
  {"- d"},
  {"- e"},
  {"- f"},
  {"- g"},
  {"- h"},
  {"- i"},
  {"- j"},
  {"- k"},
  {"- l"},
  {"- m"},
  {"- n"},
  {"- o"},
  {"- p"},
  {"- q"},
  {"- r"},
  {"- s"},
  {"- t"},
  {"- u"},
  {"- v"},
  {"- w"},
  {"- x"},
  {"- y"},
  {"- z"},
};

M_SELECT numb_menu[]
{
  {"<< Code Category"},
  {"- 0"},
  {"- 1"},
  {"- 2"},
  {"- 3"},
  {"- 4"},
  {"- 5"},
  {"- 6"},
  {"- 7"},
  {"- 8"},
  {"- 9"},
};

M_SELECT func_menu[]
{
  {"<< Code Category"},
  {"- F1"},
  {"- F2"},
  {"- F3"},
  {"- F4"},
  {"- F5"},
  {"- F6"},
  {"- F7"},
  {"- F8"},
  {"- F9"},
  {"- F10"},
  {"- F11"},
  {"- F12"},
};

M_SELECT more_menu[]
{
  {"<< Code Category"},
  {"- LEFT CTRL"},
  {"- RIGHT CTRL"},
  {"- LEFT SHIFT"},
  {"- RIGHT SHIFT"},
  {"- LEFT ALT"},
  {"- RIGHT ALT"},
  {"- LEFT GUI"},
  {"- RIGHT GUI"},
  {"- UP ARROW"},
  {"- DOWN ARROW"},
  {"- LEFT ARROW"},
  {"- RIGHT ARROW"},
  {"- BACKSPACE"},
  {"- TAB"},
  {"- RETURN"},
  {"- ESC"},
  {"- INSERT"},
  {"- DELETE"},
  {"- PAGE UP"},
  {"- PAGE DOWN"},
  {"- HOME"},
  {"- END"},
  {"- CAPS LOCK"},
};

M_SELECT mode_menu[]
{
  {"<< Editor Menu"},
  {"- Mode 1"},
  {"- Mode 2"},
  {"- Disable"},
};

M_SELECT measure_menu[]
{
  {"<< Main Menu"},
  {"- Key 0 : PA 0"},
  {"- Key 1 : PA 1"},
  {"- Key 2 : PA 2"},
  {"- Key 3 : PA 3"},
  {"- Key 4 : PA 4"},
  {"- Key 5 : PA 5"},
  {"- Key 6 : PA 6"},
  {"- Key 7 : PA 7"},
  {"- Key 8 : PB 0"},
  {"- Key 9 : PB 1"},
};

M_SELECT settings_menu[]
{
  {"<< Main Menu"},
  {"- Knob Function"},
  {"- OLED Brightness"},
  {"- Main Ans Speed"},
  {"- List Ans Speed"},
  {"- Volt Ans Speed"},
  {"- Fade Ans Speed"},
  {"- Loop Mode"},
  {"- Switch Style"},
  {"- Switch Knob"},
  {"- [ About ]"},
};

M_SELECT knob_menu[]
{
  {"<< Settings Menu"},
  {"- Volume"},
  {"- Brightness"},
  {"- Disable"},
};

/************************************* 图片内容 *************************************/

//主菜单图片
PROGMEM const uint8_t main_icon_pic[][120]
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
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xDF,0x3F,
    0xFF,0xFF,0x8F,0x3F,0xFF,0xFF,0x07,0x3F,0xFF,0xFF,0x03,0x3E,0xFF,0xFF,0x01,0x3C,
    0xFF,0xFF,0x00,0x38,0xFF,0x7F,0x40,0x3C,0xFF,0x3F,0x80,0x3E,0xFF,0x1F,0x00,0x3F,
    0xFF,0x0F,0x88,0x3F,0xFF,0x07,0xD0,0x3F,0xFF,0x03,0xE0,0x3F,0xFF,0x01,0xF1,0x3F,
    0xFF,0x00,0xFA,0x3F,0x7F,0x00,0xFC,0x3F,0x3F,0x20,0xFE,0x3F,0x1F,0x40,0xFF,0x3F,
    0x0F,0x80,0xFF,0x3F,0x07,0xC4,0xFF,0x3F,0x0F,0xE8,0xFF,0x3F,0x1F,0xF0,0xFF,0x3F,
    0x3F,0xF8,0xFF,0x3F,0x7F,0xFC,0xFF,0x3F,0xFF,0xFE,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F,
    0xFF,0xFF,0xFF,0x3F,0xFF,0xFF,0xFF,0x3F
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

/************************************* 页面变量 *************************************/

//OLED变量
uint8_t   *buf_ptr;
uint16_t  buf_len;
uint8_t   display_width;
uint8_t   display_height;

//控制器变量
struct 
{
  uint8_t   select = 0;
  uint8_t   value[9] = { 100, 25, 25, 25, 15, 1, 0, 0, 0 }; //OLED亮度，主页面动画速度，列表动画速度，电压页面动画速度，消失动画速度，旋钮功能，循环列表，主菜单风格，旋钮方向
} controller;

//UI变量
#define     UI_DEPTH             20
struct 
{
  bool      init;
  uint8_t   num;
  uint8_t   select[UI_DEPTH];
  uint8_t   select_last;
  uint8_t   layer = 0;
  uint8_t   index = M_SLEEP;
  uint8_t   state = S_NONE;
  int16_t   indicator_y; 
  int16_t   indicator_y_trg;
  bool      sleep = true;
  uint8_t   disappear = 1;
} ui;

//主菜单变量
#define   ICON_INDICATOR_WIDTH    0   //在128*32分辨率时，图标下方有两个像素的空隙，可以在中心图标下方显示指示器，这是指示器的宽度
#define   ICON_HEIGHT             30
#define   ICON_WIDTH              30
#define   ICON_SPACE              36
#define   TITLE_HIGH              14
#define   TITLE_SPACE             36
#define   TITLE_Y                 77
#define   TITLE_Y_TRG             58
#define   TITLE_INDICATOR_WIDTH   7
#define   TITLE_INDICATOR_HEIGHT  27
struct 
{
  int16_t   icon;
  int16_t   icon_trg;
  int16_t   indicator_x = 0; 
  int16_t   indicator_x_trg = TITLE_INDICATOR_WIDTH;
  int16_t   title_y;
  int16_t   title_y_trg;
  bool      select_flag = false;
} tile;

//进入主菜单时的初始化
void main_param_init()
{
  switch (controller.value[7])
  {
    case 0:
      ui.init = true;
      ui.indicator_y = 0;
      tile.icon = 0;
      tile.icon_trg = - ui.select[ui.layer] * ICON_SPACE;
      tile.title_y = 0; 
      tile.title_y_trg = - ui.select[ui.layer] * TITLE_SPACE;   
      break;

    case 1:
      ui.init = false;
      ui.select_last = ui.select[ui.layer] + 1;
      tile.icon = 0;
      tile.icon_trg = ICON_SPACE;
      tile.title_y = TITLE_Y;
      tile.title_y_trg = TITLE_Y_TRG;
      break;
  }
}

//列表变量
#define     LIST_BAR_WIDTH        7
struct
{
  int16_t   y;
  int16_t   y_trg;
  int16_t   box_x;
  int16_t   box_x_trg;
  int16_t   box_y;
  int16_t   box_y_trg[UI_DEPTH];
} list;

//进入更深层级时的初始化
void list_param_init_in()
{
  //每次进入页面都有动画
  ui.layer++;
  ui.init = 0;
  ui.indicator_y = 0;
  list.y = 0;
  list.y_trg = 16;
  list.box_x = 0;
}

//进入更浅层级时的初始化
void list_param_init_out()
{
  //离开当前页时重置当前页，确保下次来时从头开始
  ui.select[ui.layer] = 0;
  list.box_y_trg[ui.layer] = 0;

  //每次进入页面都有动画
  ui.layer--;
  ui.init = 0;
  ui.indicator_y = 0;
  list.y = 0;
  list.y_trg = 16;
  list.box_x = 0;
}

//电压测量页面
struct
{
  uint8_t   left_space   =   28;
  uint8_t   right_space  =   28;
  uint8_t   up_space;
  uint8_t   down_space;
  int16_t   left; 
  int16_t   left_trg; 
  int16_t   right; 
  int16_t   right_trg; 
  int16_t   up; 
  int16_t   up_trg; 
  int16_t   down; 
  int16_t   down_trg; 
} focus;

//进入电压测量界面的初始化
void voltage_param_init()
{
  //每次进入页面都有动画
  focus.up_space = focus.down_space = (display_height - 28) / 2;
  focus.left = 0;  
  focus.left_trg = focus.left_space; 
  focus.right = display_width - 1;  
  focus.right_trg = display_width - focus.right_space - 1; 
  focus.up = 0; 
  focus.up_trg = focus.up_space; 
  focus.down = display_height - 1; 
  focus.down_trg = display_height - focus.down_space - 1; 
}

//关于本机页面
struct
{
  int16_t   y;
  int16_t   y_trg;
} about;

//进入关于本机界面的初始化
void about_param_init()
{
  //每次进入页面都从头开始
  about.y = 0;
  about.y_trg = 0;
}

/************************************* 旋钮相关 *************************************/

#define   AIO   PB12
#define   BIO   PB13
#define   SW    PB14

#define   DEBOUNCE 50
uint8_t   btn_id = 0;
uint8_t   btn_flag = 0;
bool      btn_val = false;
bool      btn_val_last = false;
bool      btn_pressed = false;
bool      CW_1 = false;
bool      CW_2 = false;

void btn_inter() 
{
  bool alv = digitalRead(AIO);
  bool blv = digitalRead(BIO);
  if (btn_flag == 0 && alv == LOW) 
  {
    CW_1 = blv;
    btn_flag = 1;
  }
  if (btn_flag && alv) 
  {
    CW_2 = !blv;
    if (CW_1 && CW_2)
     {
      btn_id = controller.value[8];
      btn_pressed = true;
    }
    if (CW_1 == false && CW_2 == false) 
    {
      btn_id = !controller.value[8];
      btn_pressed = true;
    }
    btn_flag = 0;
  }
}

void btn_init() 
{
  pinMode(AIO, INPUT);
  pinMode(BIO, INPUT);
  pinMode(SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(AIO), btn_inter, CHANGE);
}

void btn_scan() 
{
  btn_val = digitalRead(SW);
  if (btn_val != btn_val_last)
  {
    delay(DEBOUNCE);
    btn_val_last = btn_val;
    if (btn_val == LOW)
    {
      btn_pressed = true;
      btn_id = 2;
    }
  }
}

/************************************* 断电保存 *************************************/

#include <EEPROM.h>

uint8_t   init_check[11] = { 'a', 'b', 'c', 'd', 'e', 'f','g', 'h', 'i', 'j', 'k' }; 
bool      eeprom_change = false;
int       address;

//EEPROM写数据，回到主菜单时执行一遍
void eeprom_write_all_data()
{
  address =  0;  for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, init_check[i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[0][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[1][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[2][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[3][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[4][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[5][i]);
  address += 11; for (uint8_t i = 0; i < 11; ++i) EEPROM.write(address + i, key[6][i]);
  address += 11; for (uint8_t i = 0; i < 9;  ++i) EEPROM.write(address + i, controller.value[i]);
}

//EEPROM读数据，开机初始化时执行一遍
void eeprom_read_all_data()
{
  address =  11; for (uint8_t i = 0; i < 11; ++i) key[0][i] = EEPROM.read(address + i);
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[1][i] = EEPROM.read(address + i);
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[2][i] = EEPROM.read(address + i); 
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[3][i] = EEPROM.read(address + i);
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[4][i] = EEPROM.read(address + i);
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[5][i] = EEPROM.read(address + i); 
  address += 11; for (uint8_t i = 0; i < 11; ++i) key[6][i] = EEPROM.read(address + i);
  address += 11; for (uint8_t i = 0; i < 9;  ++i) controller.value[i] = EEPROM.read(address + i);
}

//开机检查是否已经修改过，没修改过则跳过读配置步骤，用默认设置
void eeprom_init()
{
  uint8_t check = 0;
  address = 0;  for (uint8_t i = 0; i < 11; ++i)  if (EEPROM.read(address + i) != init_check[i])  check ++;
  if (check <= 1) eeprom_read_all_data();
}

/************************************* 动画函数 *************************************/

//移动函数
void move(int16_t *a, int16_t *a_trg, uint8_t n)
{
  if (*a < *a_trg) *a += ceil(fabs((float)(*a_trg - *a) / (controller.value[n] / 10.0)));
  else if (*a > *a_trg) *a -= ceil(fabs((float)(*a_trg - *a) / (controller.value[n] / 10.0)));
}

//消失函数
void disappear()
{
  delay(controller.value[4]);
  switch (ui.disappear)
  {
    case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55; break;
    case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA; break;
    case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
    case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
    default: ui.state = S_NONE; ui.disappear = 0; break;
  }
  ui.disappear++;
}

/************************************* 显示函数 *************************************/

/********************************** 列表风格主菜单 ***********************************/

//主菜单列表风格显示函数
void main_show_list()
{
  u8g2.setFont(u8g2_font_crox4hb_tr); 
  ui.indicator_y_trg = ceil((ui.select[ui.layer]) * ((float)display_height / (ui.num - 1)));
  move(&tile.icon, &tile.icon_trg, 1);
  move(&tile.title_y, &tile.title_y_trg, 1);
  move(&ui.indicator_y, &ui.indicator_y_trg, 1);
  u8g2.drawBox(display_width - TITLE_INDICATOR_WIDTH, 0, TITLE_INDICATOR_WIDTH, ui.indicator_y);
  for(uint8_t i = 0 ; i < ui.num ; ++ i)  
  {
    u8g2.drawXBMP(0, tile.icon + i * ICON_SPACE + 1 + (display_height - ICON_HEIGHT) / 2, ICON_WIDTH, ICON_HEIGHT, main_icon_pic[i]);
    u8g2.drawStr(((display_width - u8g2.getStrWidth(main_menu[i].m_select)) / 2)  + ceil((float)(ICON_WIDTH - TITLE_INDICATOR_WIDTH) / 2), tile.title_y + (i + 1) * TITLE_SPACE - TITLE_HIGH  + (display_height - ICON_HEIGHT) / 2, main_menu[i].m_select);
  }
}

/************************************* 图标主菜单 *************************************/

//主菜单图标风格显示函数
void main_show_icon()
{
  u8g2.setFont(u8g2_font_helvB18_tf); 
  move(&tile.icon, &tile.icon_trg, 1);
  if (ui.select[ui.layer] != ui.select_last)  
  {
    move(&tile.indicator_x, &tile.indicator_x_trg, 1);
    move(&tile.title_y, &tile.title_y_trg, 1);
  }
  if (tile.select_flag)
  {
    tile.indicator_x = TITLE_INDICATOR_WIDTH;
    tile.title_y = TITLE_Y_TRG;
  }
  if (tile.icon == tile.icon_trg && tile.indicator_x ==  tile.indicator_x_trg && tile.title_y ==  tile.title_y_trg)  ui.select_last = ui.select[ui.layer];
  switch (display_height)
  {
    case 32: 
      u8g2.drawBox((display_width - ICON_INDICATOR_WIDTH) / 2, 31, ICON_INDICATOR_WIDTH, 1);
      break;

    case 64: 
      u8g2.drawBox(0, display_height - TITLE_INDICATOR_HEIGHT - 1, tile.indicator_x, TITLE_INDICATOR_HEIGHT);
      u8g2.drawStr(((display_width - u8g2.getStrWidth(main_menu[ui.select[ui.layer]].m_select)) / 2) + TITLE_INDICATOR_WIDTH, tile.title_y, main_menu[ui.select[ui.layer]].m_select);
      break; 
  }
  if (!ui.init)
  {
    for(uint8_t i = 0 ; i < ui.num ; ++ i)  u8g2.drawXBMP((display_width - ICON_WIDTH) / 2 + (i - ui.select[ui.layer]) * tile.icon, 0, ICON_WIDTH, ICON_HEIGHT, main_icon_pic[i]);
    if (tile.icon == tile.icon_trg) 
    {
      ui.init = true;
      tile.icon = tile.icon_trg = - ui.select[ui.layer] * ICON_SPACE;
    }
  }
  else  for(uint8_t i = 0 ; i < ui.num ; ++ i)  u8g2.drawXBMP((display_width - ICON_WIDTH) / 2 + tile.icon + i * ICON_SPACE, 0, ICON_WIDTH, ICON_HEIGHT, main_icon_pic[i]); 
}

/*************************************************************************************/

//列表类页面通用显示函数
void menu_ui_show(struct MENU arr[])
{
  u8g2.setFont(u8g2_font_wqy12_t_chinese1);
  ui.indicator_y_trg = ceil((ui.select[ui.layer]) * ((float)display_height / (ui.num - 1)));
  list.box_x_trg = u8g2.getStrWidth(arr[ui.select[ui.layer]].m_select) + 8;
  move(&list.y, &list.y_trg, 2);
  move(&list.box_x, &list.box_x_trg, 2);
  move(&list.box_y, &list.box_y_trg[ui.layer], 2);
  move(&ui.indicator_y, &ui.indicator_y_trg, 2);
  if (!ui.init)
  {
    for (uint8_t i = 0 ; i < ui.num ; ++ i) u8g2.drawStr(4, 12 + (i - ui.select[ui.layer]) * list.y + list.box_y_trg[ui.layer], arr[i].m_select);
    if (list.y == list.y_trg) 
    {
      ui.init = true;
      list.y = list.y_trg = - 16 * ui.select[ui.layer] + list.box_y_trg[ui.layer];
    }
  }
  else for (uint8_t i = 0 ; i < ui.num ; ++ i) u8g2.drawStr(4, 16 * i + list.y + 12, arr[i].m_select);
  u8g2.drawBox(display_width - LIST_BAR_WIDTH, 0, LIST_BAR_WIDTH, ui.indicator_y);
  u8g2.setDrawColor(2);
  u8g2.drawRBox(0, list.box_y, list.box_x, 16, 1);
  u8g2.setDrawColor(1);
}

//正在编辑界面显示函数
void editing_show()
{
  uint8_t editing_up_space = (display_height - 32) / 2;
  u8g2.setDrawColor(0);
  u8g2.drawRBox(12, editing_up_space, 104, 32, 8);
  u8g2.setDrawColor(1);
  u8g2.drawRFrame(12, editing_up_space, 104, 32, 8);
  u8g2.drawRFrame(18, editing_up_space + 19, 92, 8, 3);
  float width = (float)(key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1]) / (float)(key[TRAVEL][ui.select[ui.layer - 1] - 1]) * 86;
  if (width) u8g2.drawRBox(20, editing_up_space + 21, width + 2, 4, 1);
  u8g2.setCursor(20, editing_up_space + 14);
  switch (ui.select[ui.layer])
  {
    case 1: u8g2.print("Trigger");  break;
    case 2: u8g2.print("Release");  break;
    case 3: u8g2.print("Top");      break;
    case 4: u8g2.print("Bottom");   break;
    case 5: u8g2.print("Times");    break;
  }
  u8g2.setCursor(96, editing_up_space + 14);
  u8g2.print(key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1]);
}

//控制器正在编辑界面显示函数
void controller_show()
{
  uint8_t max_value;
  uint8_t cursor_pos;
  switch (controller.select)
  {
    case 0: max_value = 255;  cursor_pos = 90;  break;
    case 1: max_value = 90;   cursor_pos = 96;  break;
    case 2: max_value = 90;   cursor_pos = 96;  break;
    case 3: max_value = 90;   cursor_pos = 96;  break;
    case 4: max_value = 50;   cursor_pos = 96;  break;
  }
  uint8_t editing_up_space = (display_height - 32) / 2;
  u8g2.setDrawColor(0);
  u8g2.drawRBox(12, editing_up_space, 104, 32, 8);
  u8g2.setDrawColor(1);
  u8g2.drawRFrame(12, editing_up_space, 104, 32, 8);
  u8g2.drawRFrame(18, editing_up_space + 19, 92, 8, 3);
  float width = (float)controller.value[controller.select] / (float)max_value * 85;
  if (width) u8g2.drawRBox(20, editing_up_space + 21, width + 3, 4, 1);
  u8g2.setCursor(20, editing_up_space + 14);
  switch (ui.select[ui.layer])
  {
    case 2: u8g2.print("Brightness"); break;
    case 3: u8g2.print("Main Speed"); break;
    case 4: u8g2.print("List Speed"); break;
    case 5: u8g2.print("Volt Speed"); break;
    case 6: u8g2.print("Fade Speed"); break;
  }
  u8g2.setCursor(cursor_pos, editing_up_space + 14);
  u8g2.print(controller.value[controller.select]);
}

//电压测量页面显示函数
void voltage_show()
{
  move(&focus.left, &focus.left_trg, 3);
  move(&focus.right, &focus.right_trg, 3);
  move(&focus.up, &focus.up_trg, 3);
  move(&focus.down, &focus.down_trg, 3);
  u8g2.setDrawColor(0);
  u8g2.setFont(u8g2_font_crox4hb_tr);
  u8g2.drawBox(0, 0, display_width, display_height);
  u8g2.setDrawColor(1);
  u8g2.drawRBox(focus.left, focus.up, focus.right - focus.left, focus.down - focus.up, 8);
  float pin_val = analogRead(key[PINS][ui.select[ui.layer - 1] - 1]);
  float pin_vol = (pin_val / 4096) * 3.3;
  u8g2.setDrawColor(0);
  u8g2.setCursor(36, focus.up_space + 21);
  u8g2.print(pin_vol);
  u8g2.print(" v");
  u8g2.setDrawColor(1);
}

//关于本机界面显示函数
void about_show()
{
  move(&about.y, &about.y_trg, 2);
  u8g2.setFont(u8g2_font_t0_11_tf);
  u8g2.setCursor(0, 10 - about.y);
  u8g2.print("K");  
  u8g2.setCursor(10 + 16 * 0, 10 - about.y);
  u8g2.print("TR");
  u8g2.setCursor(10 + 16 * 1, 10 - about.y);
  u8g2.print("RL");
  u8g2.setCursor(10 + 16 * 2, 10 - about.y);
  u8g2.print("TP");
  u8g2.setCursor(10 + 16 * 3, 10 - about.y);
  u8g2.print("BT");
  u8g2.setCursor(10 + 16 * 4, 10 - about.y);
  u8g2.print("TM");
  u8g2.setCursor(10 + 16 * 5, 10 - about.y);
  u8g2.print("MD");
  u8g2.setCursor(10 + 16 * 6, 10 - about.y);
  u8g2.print("CD");
  for (uint8_t i = 0; i < 7; ++i)  u8g2.drawBox(8 + 16 * i, 0 - about.y, 1, 130);
  for (uint8_t j = 0; j < 10; ++j)
  {
    u8g2.setCursor(0, 22 + 12 * j - about.y);
    u8g2.print(j);
    for (uint8_t i = 0; i < 7; ++i)
    {
      u8g2.setCursor(10 + 16 * i, 22 + 12 * j - about.y);
      if (i == 5) u8g2.print(key[6][j]);
      else if (i == 6) u8g2.print(key[5][j]);
      else u8g2.print(key[i][j]);
    }
  }
}

/************************************* 处理函数 *************************************/

//列表类页面旋转时判断通用函数
void rotate_switch()
{
  switch (btn_id)
  {
    case 0:
      if (ui.select[ui.layer] == 0)
      {
        if (controller.value[6])
        {
          ui.select[ui.layer] = ui.num - 1;
          list.y_trg =  - 16 * (ui.num - display_height / 16);
          list.box_y_trg[ui.layer] = (display_height / 16 - 1) * 16;
          break;
        }
        else  break;
      }
      if (ui.init)
      {
        ui.select[ui.layer] -= 1;
        if (ui.select[ui.layer] < -(list.y / 16)) list.y_trg += 16;
        else list.box_y_trg[ui.layer] -= 16;
        break;
      }

    case 1:
      if (ui.select[ui.layer] == (ui.num - 1))
      {
        if (controller.value[6])
        {
          ui.select[ui.layer] = 0;
          list.y_trg = 0;
          list.box_y_trg[ui.layer] = 0;
          break;
        }
        else  break;
      }
      if (ui.init)
      {
        ui.select[ui.layer] += 1;
        if ((ui.select[ui.layer] + 1) > (display_height / 16 - list.y / 16)) list.y_trg -= 16;
        else list.box_y_trg[ui.layer] += 16;
        break;
      }
      break;
  }
}

//睡眠页面处理函数
void sleep_proc()
{
  while (ui.sleep)
  {
    //按键扫描
    for (int n = 0; n < KEYS; n++) runKey(n);

    //旋钮扫描
    btn_scan();
    if (btn_pressed)
    {
      btn_pressed = false;

      switch (btn_id)
      {
        case 0: //顺时针旋转
          switch (controller.value[5])
          {
            case 1: Consumer.press(HIDConsumer::VOLUME_UP);       Consumer.release(); break;
            case 2: Consumer.press(HIDConsumer::BRIGHTNESS_UP);   Consumer.release(); break;
          }
          break;

        case 1: //逆时针旋转
          switch (controller.value[5])
          {
            case 1: Consumer.press(HIDConsumer::VOLUME_DOWN);     Consumer.release(); break;
            case 2: Consumer.press(HIDConsumer::BRIGHTNESS_DOWN); Consumer.release(); break;
          }
          break;

        case 2:
          ui.index = M_MAIN;
          ui.state = S_NONE;
          ui.sleep = false;
          u8g2.setPowerSave(0); 
          main_param_init();
          break;
      }
    }    
  }
}

void main_proc(void)
{
  ui.num = sizeof(main_menu) / sizeof(M_SELECT);
  switch (controller.value[7])
  {
    case 0: main_show_list(); break;
    case 1: main_show_icon(); break;
  }
  if(btn_pressed)
  {
    btn_pressed = false;
    if (controller.value[7])
    {
      tile.indicator_x = 0;
      tile.title_y = TITLE_Y;      
    }
    switch(btn_id)
    { 
      case 0:
        if (ui.init)
        {
          if (ui.select[ui.layer] > 0)
          {
            ui.select_last = ui.select[ui.layer];
            ui.select[ui.layer] -= 1;
            tile.icon_trg += ICON_SPACE;
            tile.select_flag = false;
            if (!controller.value[7]) tile.title_y_trg += TITLE_SPACE;
          }
          else 
          {
            if (controller.value[6])
            {
              ui.select[ui.layer] = ui.num - 1;
              tile.icon_trg = - ICON_SPACE * (ui.num - 1);
              if (!controller.value[7]) tile.title_y_trg = - TITLE_SPACE * (ui.num - 1);
              break;
            }
            else tile.select_flag = true;
          }
        }
        break;

      case 1:
        if (ui.init)
        {
          if (ui.select[ui.layer] < (ui.num - 1)) 
          {
            ui.select_last = ui.select[ui.layer];
            ui.select[ui.layer] += 1;
            tile.icon_trg -= ICON_SPACE;
            tile.select_flag = false;
            if (!controller.value[7]) tile.title_y_trg -= TITLE_SPACE;
          }
          else 
          {
            if (controller.value[6])
            {
              ui.select[ui.layer] = 0;
              tile.icon_trg = 0;
              if (!controller.value[7]) tile.title_y_trg = 0;
              break;
            }
            else tile.select_flag = true;
          }
        }
        break;

      case 2:
        switch (ui.select[ui.layer])
        {
          case 0:
            ui.index = M_SLEEP;
            ui.state = S_NONE;
            u8g2.setPowerSave(1);
            ui.sleep = true;
            key_init();
            if (eeprom_change == true)
            {
              eeprom_write_all_data();
              eeprom_change = false;
            }
            break;

          case 1:  ui.index = M_EDITOR;     ui.state = S_DISAPPEAR;  list_param_init_in();  break;
          case 2:  ui.index = M_MEASURE;    ui.state = S_DISAPPEAR;  list_param_init_in();  break;
          case 3:  ui.index = M_SETTINGS;   ui.state = S_DISAPPEAR;  list_param_init_in();  break;
        }
        break;
    }
  }
}

void editor_proc()
{
  ui.num = sizeof(editor_menu) / sizeof(M_SELECT);
  menu_ui_show(editor_menu);
  list.box_x_trg = u8g2.getStrWidth(editor_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 0: ui.index = M_MAIN;    ui.state = S_DISAPPEAR;   list_param_init_out();  main_param_init();  break;
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
          case 9:
          case 10:
          case 11: ui.index = M_EDIT;   ui.state = S_DISAPPEAR;   list_param_init_in();   break;
        }
        break;
    }
  }
}

void edit_proc()
{
  ui.num = sizeof(edit_menu) / sizeof(M_SELECT);
  menu_ui_show(edit_menu);
  list.box_x_trg = u8g2.getStrWidth(edit_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 0: ui.index = M_EDITOR;    ui.state = S_DISAPPEAR;   list_param_init_out();  break;
          case 1: 
          case 2: 
          case 3: 
          case 4: 
          case 5: ui.index = M_EDITING;   ui.state = S_NONE;                                break;
          case 6: ui.index = M_CODE;      ui.state = S_DISAPPEAR;   list_param_init_in();   break;
          case 7: ui.index = M_MODE;      ui.state = S_DISAPPEAR;   list_param_init_in();   break;
        }
        break;
    }
  }
}

//正在编辑页面处理函数
void editing_proc()
{
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0:
        if (key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1] < key[TRAVEL][ui.select[ui.layer - 1] - 1]) key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1] += 1;
        eeprom_change = true;
        break;

      case 1:
        if (key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1] > 0)  key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1] -= 1;
        eeprom_change = true;
        break;  

      case 2:
        if (ui.select[ui.layer - 1] == 11)  for (uint8_t i = 0 ; i < KEYS ; ++i)  key[ui.select[ui.layer] - 1][i] = key[ui.select[ui.layer] - 1][ui.select[ui.layer - 1] - 1];      
        ui.index = M_EDIT;
        ui.state = S_NONE;
        break;
    }
  }
  menu_ui_show(edit_menu);
  for (uint16_t i = 0; i < buf_len; ++i)  buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);
  editing_show();
}

void code_proc()
{
  ui.num = sizeof(code_menu) / sizeof(M_SELECT);
  menu_ui_show(code_menu);
  list.box_x_trg = u8g2.getStrWidth(code_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 0: if (ui.select[ui.layer - 2] == 11) for (uint8_t i = 0 ; i < KEYS ; ++i) key[CODES][i] = key[CODES][ui.select[ui.layer - 2] - 1];
                  ui.index = M_EDIT;  ui.state = S_DISAPPEAR;   list_param_init_out();    break;
          case 1: ui.index = M_ALPH;  ui.state = S_DISAPPEAR;   list_param_init_in();     break;
          case 2: ui.index = M_NUMB;  ui.state = S_DISAPPEAR;   list_param_init_in();     break;
          case 3: ui.index = M_FUNC;  ui.state = S_DISAPPEAR;   list_param_init_in();     break;
          case 4: ui.index = M_MORE;  ui.state = S_DISAPPEAR;   list_param_init_in();     break;
        }
        break;
    }
  }
}

void alph_proc()
{
  ui.num = sizeof(alph_menu) / sizeof(M_SELECT);
  menu_ui_show(alph_menu);
  list.box_x_trg = u8g2.getStrWidth(alph_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1:   key[CODES][ui.select[ui.layer - 3] - 1] = 'a'; eeprom_change = true; break;
          case 2:   key[CODES][ui.select[ui.layer - 3] - 1] = 'b'; eeprom_change = true; break;
          case 3:   key[CODES][ui.select[ui.layer - 3] - 1] = 'c'; eeprom_change = true; break;
          case 4:   key[CODES][ui.select[ui.layer - 3] - 1] = 'd'; eeprom_change = true; break;
          case 5:   key[CODES][ui.select[ui.layer - 3] - 1] = 'e'; eeprom_change = true; break;
          case 6:   key[CODES][ui.select[ui.layer - 3] - 1] = 'f'; eeprom_change = true; break;
          case 7:   key[CODES][ui.select[ui.layer - 3] - 1] = 'g'; eeprom_change = true; break;
          case 8:   key[CODES][ui.select[ui.layer - 3] - 1] = 'h'; eeprom_change = true; break;
          case 9:   key[CODES][ui.select[ui.layer - 3] - 1] = 'i'; eeprom_change = true; break;
          case 10:  key[CODES][ui.select[ui.layer - 3] - 1] = 'j'; eeprom_change = true; break;
          case 11:  key[CODES][ui.select[ui.layer - 3] - 1] = 'k'; eeprom_change = true; break;
          case 12:  key[CODES][ui.select[ui.layer - 3] - 1] = 'l'; eeprom_change = true; break;
          case 13:  key[CODES][ui.select[ui.layer - 3] - 1] = 'm'; eeprom_change = true; break;
          case 14:  key[CODES][ui.select[ui.layer - 3] - 1] = 'n'; eeprom_change = true; break;
          case 15:  key[CODES][ui.select[ui.layer - 3] - 1] = 'o'; eeprom_change = true; break;
          case 16:  key[CODES][ui.select[ui.layer - 3] - 1] = 'p'; eeprom_change = true; break;
          case 17:  key[CODES][ui.select[ui.layer - 3] - 1] = 'q'; eeprom_change = true; break;
          case 18:  key[CODES][ui.select[ui.layer - 3] - 1] = 'r'; eeprom_change = true; break;
          case 19:  key[CODES][ui.select[ui.layer - 3] - 1] = 's'; eeprom_change = true; break;
          case 20:  key[CODES][ui.select[ui.layer - 3] - 1] = 't'; eeprom_change = true; break;
          case 21:  key[CODES][ui.select[ui.layer - 3] - 1] = 'u'; eeprom_change = true; break;
          case 22:  key[CODES][ui.select[ui.layer - 3] - 1] = 'v'; eeprom_change = true; break;
          case 23:  key[CODES][ui.select[ui.layer - 3] - 1] = 'w'; eeprom_change = true; break;
          case 24:  key[CODES][ui.select[ui.layer - 3] - 1] = 'x'; eeprom_change = true; break;
          case 25:  key[CODES][ui.select[ui.layer - 3] - 1] = 'y'; eeprom_change = true; break;
          case 26:  key[CODES][ui.select[ui.layer - 3] - 1] = 'z'; eeprom_change = true; break;
        }
        ui.index = M_CODE;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

void numb_proc()
{
  ui.num = sizeof(numb_menu) / sizeof(M_SELECT);
  menu_ui_show(numb_menu);
  list.box_x_trg = u8g2.getStrWidth(numb_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1:   key[CODES][ui.select[ui.layer - 3] - 1] = 0; eeprom_change = true; break;
          case 2:   key[CODES][ui.select[ui.layer - 3] - 1] = 1; eeprom_change = true; break;
          case 3:   key[CODES][ui.select[ui.layer - 3] - 1] = 2; eeprom_change = true; break;
          case 4:   key[CODES][ui.select[ui.layer - 3] - 1] = 3; eeprom_change = true; break;
          case 5:   key[CODES][ui.select[ui.layer - 3] - 1] = 4; eeprom_change = true; break;
          case 6:   key[CODES][ui.select[ui.layer - 3] - 1] = 5; eeprom_change = true; break;
          case 7:   key[CODES][ui.select[ui.layer - 3] - 1] = 6; eeprom_change = true; break;
          case 8:   key[CODES][ui.select[ui.layer - 3] - 1] = 7; eeprom_change = true; break;
          case 9:   key[CODES][ui.select[ui.layer - 3] - 1] = 8; eeprom_change = true; break;
          case 10:  key[CODES][ui.select[ui.layer - 3] - 1] = 9; eeprom_change = true; break;
        }
        ui.index = M_CODE;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

void func_proc()
{
  ui.num = sizeof(func_menu) / sizeof(M_SELECT);
  menu_ui_show(func_menu);
  list.box_x_trg = u8g2.getStrWidth(func_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F1;  eeprom_change = true; break;
          case 2:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F2;  eeprom_change = true; break;
          case 3:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F3;  eeprom_change = true; break;
          case 4:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F4;  eeprom_change = true; break;
          case 5:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F5;  eeprom_change = true; break;
          case 6:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F6;  eeprom_change = true; break;
          case 7:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F7;  eeprom_change = true; break;
          case 8:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F8;  eeprom_change = true; break;
          case 9:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F9;  eeprom_change = true; break;
          case 10: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F10; eeprom_change = true; break;
          case 11: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F11; eeprom_change = true; break;
          case 12: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_F12; eeprom_change = true; break;
        }
        ui.index = M_CODE;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

void more_proc()
{
  ui.num = sizeof(more_menu) / sizeof(M_SELECT);
  menu_ui_show(more_menu);
  list.box_x_trg = u8g2.getStrWidth(more_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_LEFT_CTRL;    eeprom_change = true; break;
          case 2:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RIGHT_CTRL;   eeprom_change = true; break;
          case 3:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_LEFT_SHIFT;   eeprom_change = true; break;
          case 4:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RIGHT_SHIFT;  eeprom_change = true; break;
          case 5:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_LEFT_ALT;     eeprom_change = true; break;
          case 6:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RIGHT_ALT;    eeprom_change = true; break;
          case 7:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_LEFT_GUI;     eeprom_change = true; break;
          case 8:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RIGHT_GUI;    eeprom_change = true; break;
          case 9:  key[CODES][ui.select[ui.layer - 3] - 1] = KEY_UP_ARROW;     eeprom_change = true; break;
          case 10: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_DOWN_ARROW;   eeprom_change = true; break;
          case 11: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_LEFT_ARROW;   eeprom_change = true; break;
          case 12: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RIGHT_ARROW;  eeprom_change = true; break;
          case 13: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_BACKSPACE;    eeprom_change = true; break;
          case 14: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_TAB;          eeprom_change = true; break;
          case 15: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_RETURN;       eeprom_change = true; break;
          case 16: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_ESC;          eeprom_change = true; break;
          case 17: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_INSERT;       eeprom_change = true; break;
          case 18: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_DELETE;       eeprom_change = true; break;
          case 19: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_PAGE_UP;      eeprom_change = true; break;
          case 21: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_PAGE_DOWN;    eeprom_change = true; break;
          case 22: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_HOME;         eeprom_change = true; break;
          case 23: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_END;          eeprom_change = true; break;
          case 24: key[CODES][ui.select[ui.layer - 3] - 1] = KEY_CAPS_LOCK;    eeprom_change = true; break;
        }
        ui.index = M_CODE;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

void mode_proc()
{
  ui.num = sizeof(mode_menu) / sizeof(M_SELECT);
  menu_ui_show(mode_menu);
  list.box_x_trg = u8g2.getStrWidth(mode_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1: key[MODES][ui.select[ui.layer - 2] - 1] = 1; eeprom_change = true; break;
          case 2: key[MODES][ui.select[ui.layer - 2] - 1] = 2; eeprom_change = true; break;
          case 3: key[MODES][ui.select[ui.layer - 2] - 1] = 0; eeprom_change = true; break;
        }
        if (ui.select[ui.layer - 2] == 11)  for (uint8_t i = 0 ; i < KEYS ; ++i)  key[MODES][i] = key[MODES][ui.select[ui.layer - 2] - 1];
        ui.index = M_EDIT;
        ui.state = S_DISAPPEAR;  
        list_param_init_out(); 
        break;
    }
  }
}

void measure_proc()
{
  ui.num = sizeof(measure_menu) / sizeof(M_SELECT);
  menu_ui_show(measure_menu);
  list.box_x_trg = u8g2.getStrWidth(measure_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 0:   ui.index = M_MAIN;      ui.state = S_DISAPPEAR;   list_param_init_out();  main_param_init();      break;
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
          case 8:
          case 9:
          case 10:  ui.index = M_VOLTAGE;   ui.state = S_DISAPPEAR;   list_param_init_in();   voltage_param_init();   break;
        }
        break;
    }
  }
}

void settings_proc()
{
  ui.num = sizeof(settings_menu) / sizeof(M_SELECT);
  menu_ui_show(settings_menu);
  list.box_x_trg = u8g2.getStrWidth(settings_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 0:  ui.index = M_MAIN;        ui.state = S_DISAPPEAR;   list_param_init_out();  main_param_init();                            break;
          case 1:  ui.index = M_KNOB;        ui.state = S_DISAPPEAR;   list_param_init_in();                                                 break;
          case 2:  ui.index = M_CONTROLLER;  ui.state = S_NONE;                                controller.select = 0;                        break;
          case 3:  ui.index = M_CONTROLLER;  ui.state = S_NONE;                                controller.select = 1;                        break;
          case 4:  ui.index = M_CONTROLLER;  ui.state = S_NONE;                                controller.select = 2;                        break;
          case 5:  ui.index = M_CONTROLLER;  ui.state = S_NONE;                                controller.select = 3;                        break;
          case 6:  ui.index = M_CONTROLLER;  ui.state = S_NONE;                                controller.select = 4;                        break;
          case 7:  ui.index = M_SETTINGS;    ui.state = S_DISAPPEAR;   eeprom_change = true;   controller.value[6] = !controller.value[6];   break;
          case 8:  ui.index = M_SETTINGS;    ui.state = S_DISAPPEAR;   eeprom_change = true;   controller.value[7] = !controller.value[7];   break;
          case 9:  ui.index = M_SETTINGS;    ui.state = S_DISAPPEAR;   eeprom_change = true;   controller.value[8] = !controller.value[8];   break;
          case 10: ui.index = M_ABOUT;       ui.state = S_DISAPPEAR;   list_param_init_in();   about_param_init();                           break;
        }
        break;
    }
  }
}

void knob_proc()
{
  ui.num = sizeof(knob_menu) / sizeof(M_SELECT);
  menu_ui_show(knob_menu);
  list.box_x_trg = u8g2.getStrWidth(knob_menu[ui.select[ui.layer]].m_select) + 8;
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: case 1: rotate_switch(); break;      
      case 2:
        switch (ui.select[ui.layer])
        {
          case 1: controller.value[5] = 1; eeprom_change = true; break;
          case 2: controller.value[5] = 2; eeprom_change = true; break;
          case 3: controller.value[5] = 0; eeprom_change = true; break;
        }
        ui.index = M_SETTINGS;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

//正在编辑页面处理函数
void controller_proc()
{
  uint8_t max_value;
  uint8_t min_value;
  uint8_t step_value;
  switch (controller.select)
  {
    case 0: max_value = 255;  min_value = 0;   step_value = 5; break;
    case 1: max_value = 90;   min_value = 10;  step_value = 1; break;
    case 2: max_value = 90;   min_value = 10;  step_value = 1; break;
    case 3: max_value = 90;   min_value = 10;  step_value = 1; break;
    case 4: max_value = 50;   min_value = 0;   step_value = 5; break;
  }
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0: if (controller.value[controller.select] < max_value)  controller.value[controller.select] += step_value; eeprom_change = true; break;
      case 1: if (controller.value[controller.select] > min_value)  controller.value[controller.select] -= step_value; eeprom_change = true; break;  
      case 2: ui.index = M_SETTINGS; ui.state = S_NONE; break;
    }
    if (controller.select == 0) u8g2.setContrast(controller.value[0]);
  }
  menu_ui_show(settings_menu);
  for (uint16_t i = 0; i < buf_len; ++i)  buf_ptr[i] = buf_ptr[i] & (i % 2 == 0 ? 0x55 : 0xAA);
  controller_show();
}

void voltage_proc()
{
  voltage_show();
  if (btn_pressed)
  {
    btn_pressed = false;
    if (btn_id == 2)
    {
      ui.index = M_MEASURE;
      ui.state = S_DISAPPEAR;
      list_param_init_out();
    }
  }
}

void about_proc(void)
{
  about_show();
  if(btn_pressed)
  {
    btn_pressed = false;
    switch(btn_id)
    { 
      case 0: if (about.y_trg > 0) about.y_trg -= 12; break;
      case 1: if (about.y_trg < 130 - display_height)about.y_trg += 12; break;
      case 2:
        ui.index = M_SETTINGS;
        ui.state = S_DISAPPEAR;
        list_param_init_out();
        break;
    }
  }
}

//总的UI进程
void ui_proc()
{
  switch (ui.state)
  {
    case S_NONE: u8g2.clearBuffer();
      switch (ui.index)
      {
        case M_SLEEP:         sleep_proc();         break;
        case M_MAIN:          main_proc();          break;
        case M_EDITOR:        editor_proc();        break;
        case M_EDIT:          edit_proc();          break;
        case M_EDITING:       editing_proc();       break;
        case M_CODE:          code_proc();          break;
        case M_ALPH:          alph_proc();          break;
        case M_NUMB:          numb_proc();          break;
        case M_FUNC:          func_proc();          break;
        case M_MORE:          more_proc();          break;
        case M_MODE:          mode_proc();          break;
        case M_MEASURE:       measure_proc();       break;
        case M_SETTINGS:      settings_proc();      break;
        case M_CONTROLLER:    controller_proc();    break;
        case M_KNOB:          knob_proc();          break;
        case M_VOLTAGE:       voltage_proc();       break;
        case M_ABOUT:         about_proc();         break;
      }
      break;
    case S_DISAPPEAR: disappear(); break;
  }
  u8g2.sendBuffer();
}

//OLED初始化函数
void oled_init()
{
  u8g2.setBusClock(1000000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_helvB18_tf);
  u8g2.setContrast(controller.value[0]);
  buf_ptr = u8g2.getBufferPtr();
  buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();
  display_width = u8g2.getDisplayWidth();
  display_height = u8g2.getDisplayHeight();
}

void setup() 
{
  eeprom_init();
  oled_init();
  btn_init();
  hid_init();
  key_init();
}

void loop() 
{
  btn_scan();
  ui_proc();
}


