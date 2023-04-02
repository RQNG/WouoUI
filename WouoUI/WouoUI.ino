/*
  此程序用于实现稚晖君MonoUI风格的超丝滑菜单，使用0.96寸OLED显示，EC11旋转编码器控制。

  UI示例是从我的另一个项目简化而来，只保留了列表部分，简化了添加和修改列表的操作，如需更多功能，请参考原项目：https://github.com/RQNG/Rapid-trigger-minipad

  当前版本 v 1.0 

  示例实现了以下功能：

    * 休眠模式扫描，同时可旋转旋钮
    * 休眠模式下点按旋钮停止扫描，进入主菜单
    * 主菜单下，旋转旋钮滚动列表，点按选择列表
    * 列表长度无限制
    * 动画速度可调，1 - 无穷大，数字越小动画速度越快，1没有动画，不支持小于1的数值

  推荐把返回选项放在开头，个人认为体验比较好。

  项目参考：
    
    * 旋钮功能：https://zhuanlan.zhihu.com/p/453130384
    * UI：https://www.bilibili.com/video/BV1HA411S7pv/ ; https://www.bilibili.com/video/BV1xd4y1C7BE/

  本项目使用Apache 2.0开源协议，如需商用或借鉴，请阅读此协议。
  
  欢迎关注我的B站账号，一个只分享osu!相关内容，很无聊的帐号。
  
  用户名：音游玩的人，B站主页：https://space.bilibili.com/9182439?spm_id_from=..0.0
*/  

/************************************* 旋钮相关 *************************************/

#define   AIO       PB12
#define   BIO       PB13
#define   SW        PB14
#define   DEBOUNCE  50

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
      btn_id = 0;
      btn_pressed = true;
    }
    if (CW_1 == false && CW_2 == false) 
    {
      btn_id = 1;
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

/************************************* 屏幕和UI *************************************/

#include <U8g2lib.h>
#include <Wire.h>

#define   SCL   PB6
#define   SDA   PB7
#define   RST   U8X8_PIN_NONE

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, RST);

//状态
enum
{
  S_NONE,
  S_DISAPPEAR,
};

//菜单结构体
typedef struct MENU
{
  char *select;
} SELECT_LIST;

//UI变量
uint8_t   ui_index;                 //目录变量
uint8_t   ui_state;                 //状态变量
uint8_t   *buf_ptr;                 //指向buf首地址的指针
uint16_t  buf_len;                  //buf的长度
bool      sleep_flag = true;        //休眠标志
uint8_t   disappear_step = 1;       //消失步数

/********************************** 需要修改的东西 ***********************************/

//UI参数
#define   SPEED               6         //动画速度，越小越快，1没有动画
#define   PAGES               4         //页面数量，列表类页面的总数量

#define   ui_select           0
#define   x                   1
#define   y                   2 
#define   y_trg               3
#define   box_width           4
#define   box_width_trg       5
#define   box_y               6
#define   box_y_trg           7
#define   num                 8
#define   line_y              9
#define   line_y_trg          10
int16_t   ui_param[11][PAGES];

//目录
enum
{
  M_SLEEP,
  M_MAIN,
  M_NUMB,
  M_ALPH,
};

//主菜单内容
SELECT_LIST m_main[]
{
  {"<< Sleep"},
  {"- Number"},
  {"- 1"},
  {"- 11"},
  {"- 111"},
  {"- 1111"},
  {"- 11111"},
  {"- 111111"},
  {"- 1111111"},
  {"- 11111111"},
  {"- 111111111"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
  {"- 1111111111"},
  {"- 1"},
};

//数字菜单内容
SELECT_LIST m_numb[]
{
  {"<< Main"},
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

//睡眠页面处理函数
void sleep_proc()
{
  //在这里执行功能
  while (sleep_flag)
  {
    //需要扫描的功能
    Serial.println("Scan");

    //旋钮扫描
    btn_scan();
    if (btn_pressed)
    {
      btn_pressed = false;
      switch (btn_id)
      {
        case 0:
          //睡眠时顺时针旋转功能
          Serial.println("Clockwise");
          break;

        case 1:
          //睡眠时逆时针旋转功能
          Serial.println("Anticlockwise");
          break;  

        case 2:
          ui_index = M_MAIN;
          ui_state = S_NONE;
          sleep_flag = false;
          u8g2.setPowerSave(0);       
          break;

        default: break;
      }
    }    
  }
}

//主菜单处理函数
void main_proc()
{
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0:
      case 1:
        rotate_switch(M_MAIN);
        break; 

      case 2:
        switch (ui_param[ui_select][M_MAIN])
        {
          case 0:
            ui_index = M_SLEEP;
            ui_state = S_NONE;
            u8g2.setPowerSave(1);  
            sleep_flag = true;
            break;

          case 1:
            ui_index = M_NUMB;
            ui_state = S_DISAPPEAR;
            break;

          default: break;
        }
        
      default: break;
    }
    ui_param[box_width_trg][M_MAIN] = u8g2.getStrWidth(m_main[ui_param[ui_select][M_MAIN]].select) + ui_param[x][M_MAIN] * 2;
  }
  menu_ui_show(m_main, M_MAIN);
}

//数字菜单处理函数
void numb_proc()
{
  if (btn_pressed)
  {
    btn_pressed = false;
    switch (btn_id)
    {
      case 0:
      case 1:
        rotate_switch(M_NUMB);
        break; 

      case 2:
        switch (ui_param[ui_select][M_NUMB])
        {
          case 0:
            ui_index = M_MAIN;
            ui_state = S_DISAPPEAR;
            break;

          default: break;
        }
        
      default: break;
    }
    ui_param[box_width_trg][M_NUMB] = u8g2.getStrWidth(m_numb[ui_param[ui_select][M_NUMB]].select) + ui_param[x][M_NUMB] * 2;
  }
  menu_ui_show(m_numb, M_NUMB);
}

//总的UI进程
void ui_proc()
{
  switch (ui_state)
  {
    case S_NONE:
      u8g2.clearBuffer();
      switch (ui_index)
      {
        //目录里所有页面都要放在这里
        
        case M_SLEEP:
          sleep_proc();
          break;
        
        case M_MAIN:
          main_proc();
          break;

        case M_NUMB:
          numb_proc();
          break;

        default: break;
      }
      break;

    case S_DISAPPEAR:
      disappear();
      break;

    default: break;
  }
  u8g2.sendBuffer();
}

//ui初始化
void ui_init()
{
  for(int i = 0 ; i < PAGES ; i++)
  {
    ui_param[ui_select][i] = 0;
    ui_param[x][i] = 4;
    ui_param[y][i] = 0;
    ui_param[y_trg][i] = 0;
  }

  ui_index = M_SLEEP;
  ui_state = S_NONE;

  //主菜单
  ui_param[box_width][M_MAIN] = ui_param[box_width_trg][M_MAIN] = u8g2.getStrWidth(m_main[ui_param[ui_select][M_MAIN]].select) + ui_param[x][M_MAIN] * 2;
  ui_param[num][M_MAIN] = sizeof(m_main) / sizeof(SELECT_LIST);

  //数字菜单
  ui_param[box_width][M_NUMB] = ui_param[box_width_trg][M_NUMB] = u8g2.getStrWidth(m_numb[ui_param[ui_select][M_NUMB]].select) + ui_param[x][M_NUMB] * 2;
  ui_param[num][M_NUMB] = sizeof(m_numb) / sizeof(SELECT_LIST);
}

/************************************* 动画函数 *************************************/

//移动函数
void move(int16_t *a, int16_t *a_trg)
{
  if (*a < *a_trg) *a += ceil(fabs((float)(*a_trg - *a) / SPEED));
  else if (*a > *a_trg) *a -= ceil(fabs((float)(*a_trg - *a) / SPEED));
}

//消失函数
void disappear()
{
  switch (disappear_step)
  {
    case 1: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x55; break;
    case 2: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0xAA; break;
    case 3: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 == 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;
    case 4: for (uint16_t i = 0; i < buf_len; ++i)  if (i % 2 != 0) buf_ptr[i] = buf_ptr[i] & 0x00; break;

    default: ui_state = S_NONE; disappear_step = 0; break;
  }
  disappear_step++;
}

/************************************* 显示函数 *************************************/

//列表类页面通用显示函数
void menu_ui_show(struct  MENU arr[], int16_t n)
{
  for(uint8_t i = 0 ; i < ui_param[num][n] ; ++i) u8g2.drawStr(ui_param[x][n], 16 * i + ui_param[y][n] + 12, arr[i].select);

  ui_param[line_y_trg][n] = ceil((ui_param[ui_select][n]) * ((float)64 / (ui_param[num][n] - 1)));
  ui_param[box_width_trg][n] = u8g2.getStrWidth(arr[ui_param[ui_select][n]].select) + 8;
  
  move(&ui_param[y][n], &ui_param[y_trg][n]);
  move(&ui_param[box_y][n], &ui_param[box_y_trg][n]);
  move(&ui_param[box_width][n], &ui_param[box_width_trg][n]);
  move(&ui_param[line_y][n], &ui_param[line_y_trg][n]);

  u8g2.drawBox(125, 0, 3, ui_param[line_y][n]);
  u8g2.setDrawColor(2);
  u8g2.drawRBox(0, ui_param[box_y][n], ui_param[box_width][n], 16, 1);
  u8g2.setDrawColor(1);
}

/************************************* 处理函数 *************************************/

//列表类页面旋转时判断通用函数
void rotate_switch(int16_t n)
{
  switch (btn_id)
  {
    case 0:
      if (ui_param[ui_select][n] < 1) break;
      ui_param[ui_select][n] -= 1;
      if (ui_param[ui_select][n] < -(ui_param[y][n] / 16)) ui_param[y_trg][n] += 16;
      else ui_param[box_y_trg][n] -= 16;
      break;

    case 1:
      if ((ui_param[ui_select][n] + 2) > ui_param[num][n]) break;
      ui_param[ui_select][n] += 1;
      if ((ui_param[ui_select][n] + 1) > (4 - ui_param[y][n] / 16)) ui_param[y_trg][n] -= 16;
      else ui_param[box_y_trg][n] += 16;
      break;

    default: break;
  }
}

/************************************ 初始化函数 ************************************/

//OLED初始化函数
void oled_init()
{
  u8g2.setBusClock(800000);
  u8g2.begin();
  u8g2.setFont(u8g2_font_wqy12_t_chinese1);
  buf_ptr = u8g2.getBufferPtr();
  buf_len = 8 * u8g2.getBufferTileHeight() * u8g2.getBufferTileWidth();
}

/************************************ 主循环函数 ************************************/

void setup() 
{
  oled_init();
  ui_init();
  btn_init();

  Serial.begin(115200);
}

void loop() 
{
  btn_scan();
  ui_proc();
}
