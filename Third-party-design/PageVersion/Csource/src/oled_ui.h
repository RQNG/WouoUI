#ifndef __OLED_UI_H__
#define __OLED_UI_H__

#include "oled_g.h"
#include "oled_conf.h"
/*============================================常量定义=========================================*/
#define True  1
#define False 0


// 输入消息缓冲队列的大小(至少为2)
#define INPUT_MSG_QUNEE_SIZE  UI_INPUT_MSG_QUNEE_SIZE

//磁贴默认参数
//所有磁贴页面都使用同一套参数
#define   TILE_B_TITLE_H      24                          //磁贴大标题字体高度
#define   TILE_ICON_H         30                          //磁贴图标高度
#define   TILE_ICON_W         30                          //磁贴图标宽度
#define   TILE_ICON_S         36                          //磁贴图标间距
#define   TILE_INDI_H         27                          //磁贴大标题指示器高度
#define   TILE_INDI_W         8                           //磁贴大标题指示器宽度
#define   TILE_INDI_S         36                          //磁贴大标题指示器上边距
#define   ICON_BUFFSIZE (TILE_ICON_W*(TILE_ICON_H/8+1))

// 列表默认参数
//所有列表使用同一套参数
#define LIST_TEXT_H 8                       // 列表每行文字字体的高度
#define LIST_LINE_H 16                      // 列表单行高度
#define LIST_TEXT_S 4                       // 列表每行文字的上边距，左边距和右边距，下边距由它和字体高度和行高度决定
#define LIST_BAR_W  5                       // 列表进度条宽度，需要是奇数，因为正中间有1像素宽度的线
#define LIST_BOX_R  2                       // 列表选择框倒角

// 列表行尾选择选项参数
#define CHECK_BOX_L_S 95 // 选择框在每行的左边距
#define CHECK_BOX_U_S 2  // 选择框在每行的上边距
#define CHECK_BOX_F_W 12 // 选择框外框宽度
#define CHECK_BOX_F_H 12 // 选择框外框高度
#define CHECK_BOX_D_S 2  // 选择框里面的点距离外框的边距

// 弹窗参数
#define WIN_FNOT_H  16                           // 弹窗字体高度
#define WIN_H       32                           // 弹窗高度
#define WIN_W       102                          // 弹窗宽度
#define WIN_BAR_W   92                          // 弹窗进度条宽度
#define WIN_BAR_H   7                           // 弹窗进度条高度
#define WIN_Y       (-WIN_H - 2)              // 弹窗竖直方向出场起始位置
#define WIN_Y_TRG   (-WIN_H - 2)               // 弹窗竖直方向退场终止位置

// 确认弹窗变量
#define CON_WIN_FONT_H  16                    // 弹窗字体高度
#define CON_WIN_H       38                           // 弹窗高度
#define CON_WIN_W       102                          // 弹窗宽度
#define CON_WIN_BTN_H   20
#define CON_WIN_R       3
#define CON_WIN_Y       (-CON_WIN_H - 2)               // 弹窗竖直方向出场起始位置
#define CON_WIN_Y_TRG   (-CON_WIN_H - 2)           // 弹窗竖直方向退场终止位置

//曲线相关参数
#define   WAVE_SAMPLE         1                          //采集倍数
#define   WAVE_W              94                          //波形宽度
#define   WAVE_L              24                          //波形左边距
#define   WAVE_U              0                           //波形上边距
#define   WAVE_MAX            27                          //最大值
#define   WAVE_MIN            4                           //最小值
#define   WAVE_BOX_H          32                          //波形边框高度
#define   WAVE_BOX_W          94                          //波形边框宽度
#define   WAVE_BOX_L_S        24                          //波形边框左边距
//列表和文字背景框相关参数
#define   WAVE_FONT_H         24                          //电压数字字体
#define   WAVE_TEXT_BG_L_S    24                          //文字背景框左边距
#define   WAVE_TEXT_BG_W      94                          //文字背景框宽度
#define   WAVE_TEXT_BG_H      29  
//数字控件页面相关参数
#define   DIGITAL_RECT_X      8                           //外框的横坐标
#define   DIGITAL_RECT_Y      4                          //外框的纵坐标
#define   DIGITAL_RECT_H      40                          //外框的高度
#define   DIGITAL_RECT_W      112                         //外框的宽度
#define   DIGITAL_RECT_R      3                           //外框的倒角
#define   DIGITAL_NUM_T_S     8                           //数字到外框的边距
#define   DIGITAL_NUM_SIZE    24                          //数字的尺寸
#define   DIGITAL_Label_SIZE  16                          //数字的尺寸
#define DIGITAL_NUM_INDEX_MAX  6                          //digital页面共有6个数字

/*============================================类型声明=========================================*/
//--------页面类型枚举
typedef enum 
{
  //选择框类页面
  type_title = 0x00,  //磁贴类
  type_list,       //列表类
  //终端类页面
  type_radio,      //单选框类
  type_digital,  //数字控件类
  type_wave,       //波形显示类
  type_rader_pic,   //镭射图片类
} PageType; //页面类型，用于标志传入的每个页面类型，方便调用对应的proc函数

//-------页面成员类型
typedef char* String; // 定义字符串类型
typedef struct 
{
  uint8_t order;  //该选项在列表/磁贴中的排序(0-255)
  int16_t  item_max; //列表项对应变量可取的最大值(若是单选/多选框，该值无意义，可为0)
  int16_t  item_min; //列表项对应变量可取的最小值(若是单选/多选框，该值无意义，可为0)
  int16_t  step;//列表项对应变量的步长变化，只对数值弹窗有效(若是单选/多选框，该值无意义，可为0)
  int16_t  val;  //这个列表项关联的显示的值(可以用于设置初值) 
  String text;      
  //这个列表项显示的字符串
  //(通过选项的第一个字符判断为数值弹窗(~)/确认弹窗($)/其他项(-)/二值选项框(+))
  //其中二值选项框由于二值项只能在列表中展示，因此只在列表选择页面中有效，在磁帖页面中如果出现+开头的字串默认为其他项
  //其实单选列表项，需使用其他项在应用层关联跳转单选终端页面实现(单选列表项必须使用=做字符串开头)。
} Option; //通用选项类,其中item_max,item_min,entity与列表选项(单选/多选/弹窗项)相关，磁贴选项可不管
//选择类界面的回调函数类型定义,参数为确认选中项的指针。
typedef void (*CallBackFunc)(uint8_t self_page_id,Option* select_item);

typedef const uint8_t Icon[ICON_BUFFSIZE];  //定义图标类型
//页面地址类型，用于声明全局页面数组存放页面使用
#define PageAddr void*
//每个页面都有的三种方法
typedef void (*PageAniInit)(PageAddr);  //页面的动画初始化函数
typedef void (*PageShow)(PageAddr);    //页面的展示函数
typedef void (*PageReact)(PageAddr);  //页面的响应函数

typedef struct 
{
  PageType page_type; //页面类型，以便在处理时调用不同函数绘制
  uint8_t page_id; //页面的序号，每个页面唯一一个，用于指示在数组中的位置，方便跳转
  uint8_t last_page_id; //上一个页面的id，用于返回时使用
  CallBackFunc cb; //页面的回调函数
  PageAniInit ani_init;
  PageShow show;
  PageReact react;
} Page; //最基本的页面类型(所有页面类型的基类和结构体的**第一个成员**)

//----------5种页面类
typedef struct 
{
  Page page;   //基础页面信息
  uint8_t item_num ; //页面选项个数，option_array和icon_array个数需与此一致
  Option  *option_array; //选项类型的数组(由于数组大小不确定，使用指针代替)
  Icon    *icon_array ; //图标数组(由于数组大小不确定，使用指针代替)
  uint8_t select_item; //选中选项

  float   icon_x; //图标的x坐标距选中目标的间距的变量
  float   icon_x_trg;//图标的x坐标距选中目标的间距的变量目标
  float   icon_y;//图标的y坐标
  float   icon_y_trg;//图标的y坐标目标
  float   indi_x; //指示器的x坐标
  float   indi_x_trg;//指示器的x坐标目标值
  float   title_y;//标题的y坐标
  float   title_y_trg;//标题的y坐标目标值
} TitlePage; //磁帖页面类型(所有类型页面，类型成员为第一个，方便查看)

typedef struct
{
  Page page;   //基础页面信息
  uint8_t item_num ; //页面选项个数，title和icon个数需与此一致
  uint8_t select_item; //选中选项
  Option  *option_array; //选项类型的数组(由于数组大小不确定，使用指针代替)

  uint8_t slip_flag;   //切换动画是否完成的标志位
  uint8_t line_n; // = DISP_H / LIST_LINE_H; 屏幕内有多少行选项
  float y;            //列表中每个选项的间隔值
  float y_trg;        //列表中每个选项间隔的目标值
  float box_x;        //选中框x
  float box_x_trg;    //选中框x的目标值
  float box_y;        //选中框y
  float box_y_trg;    //选中框y的目标值
  float bar_y;        //进度条的y值
  float bar_y_trg;    //进度条的y目标值
} ListPage; //列表页面类型(所有类型页面，类型成员为第一个，方便查看)

#if PAGE_WAVE_ENABLE
typedef ListPage WavePage; //定义波形页面类型
#endif

#if PAGE_RADIO_ENABLE
typedef ListPage RadioPage; //定义单选项页面类型
#endif

#if PAGE_RADERPIC_ENABLE
typedef struct 
{
    const uint8_t * pic; //图片指针
    int16_t start_x;    //起始x
    int16_t start_y;    //起始y
    uint8_t w;          //width of pic
    uint8_t h;          //height of pic
    RaderDirection rd;  //射线方向选择，枚举类型
} RaderPic; //镭射图片对象
typedef struct 
{
  Page page; //基类
//镭射界面回调函数，传入已经绘制完成的pic序号
//为了方便在镭射图片界面，绘制自己想要的其他元素(如文字提示，在模式为Rader_Pic_Mode_Hold是，会不断绘制已完成的图片并不断调用回调函数)
  uint8_t pic_num ; //页面pic个数，pic_array数组大小需与此一致
  uint8_t rader_pic_mode:2; //结束之后的操作
  RaderPic* pic_array; //镭射图片数组

  uint8_t cur_pic_index; //当前正在绘制完成的pic序号(0~pic_num-1)
  float move_x;
  float move_y;
} RaderPicPage; //镭射图片页面对象

typedef enum
{
    // Rader_Pic_Mode_Clear = 0x00, //显示完所有图片后不操作，直接清空(这个模式暂时没有使用到)
    Rader_Pic_Mode_Loop = 0x01,    //显示完所有图片后不操作,再从第一张开始显示
    Rader_Pic_Mode_Hold = 0x02,    //显示完所有图片后不操作,保持原图
    Rader_Pic_Mode_Num,
} RaderPicMode;
#endif

#if PAGE_DIGITAL_ENABLE

typedef enum
{
    Digital_Direct_None = 0x00,
    Digital_Direct_Increase = 0x01,
    Digital_Direct_Decrease = 0x02,
} DigitalDirect;  //用于Digital Show和React函数间传递信号

typedef enum
{
    Digital_Mode_Observe = 0x00,   //观察模式没有光标
    Digital_Mode_Edit    = 0x01,   //对编辑位置的编辑
    Digital_Mode_Singlebit = 0x02, //对单位数字的编辑
} DigitalMode; //digital页面的模式



typedef enum //Digital页面从右往左
{
    Digital_Pos_IndexRight = 0x00, //用于指示当前光标或者编辑的位置
    Digital_Pos_IndexMid,
    Digital_Pos_IndexLeft,
    Digital_Pos_IndexLabel,  //在标签处
    Digital_Pos_Complete,   //编辑完成
}DigitalPosIndex; //用于在回调函数中检验选中项的op->order值,表示选中哪个数字位还是标签

typedef struct 
{
    Page page;            //页面的基本信息      
    char   gap_char;      //3个digital数字间的间隔字符
    uint8_t gap_shine_time;    //间隔字符闪烁的时间参数
    uint8_t uline_shine_time;    //下划线闪烁的时间参数
    Option * option_array; //选项数组，数组大小必须为3 ，用于显示三个数字
    uint8_t  label_array_num; // 标签数组的大小
    String * label_array;  //标签数组，方便标签滚动

    DigitalPosIndex select_index:4; //选中项的index
    uint8_t digital_num_pos; //0表示没有选中位置，1-7bit每一位置1表示该位被选中
    uint8_t temp_ripple_index; //用于在ripple时记录选中位置(选中最低位位置开始，慢慢往上增)
    DigitalDirect dir:2;  //用于Digital Show和React函数间传递信号
    DigitalMode mod:2;    //digital页面的模式
    uint8_t select_label_index;  //在标签数组中选中的标签的index
    window w_digtal[2]; //用限制数字时钟的窗口(1个数字窗口+1个label窗口) 2个窗口
    float rect_y; //移动外框的顶点坐标
    float rect_y_trg; //移动外框的顶点坐标目标
    float label_y; //移动外框的顶点坐标
    float label_y_trg; //移动外框的顶点坐标目标
    float num_y;   //移动数字的顶点坐标
    float num_y_trg;   //移动数字的顶点坐标目标
} DigitalPage; //时间页面

#endif



//---------输入消息枚举类型
typedef enum 
{
  msg_none = 0x00, //none表示没有操作
  msg_up,      //上，或者last消息，表上一个
  msg_down,    //下，或者next消息，表下一个
  msg_return,  //返回消息，表示返回，从一个页面退出
  msg_click,   //点击消息，表确认，确认某一选项，回调用一次回调
  msg_home,    //home消息，表回主界面(尚未设计，目前还没有设计对应的功能，默认以page_id为0的页面为主页面)
} InputMsg; //输入消息类型，UI设计只供输入5种消息



//------类别下标声明。用于UI参数数组中做索引
enum _ani_kind //动画速度类别(数组中的下标)
{
  TILE_ANI = 0x00,     //磁贴动画速度
  LIST_ANI,     //列表动画速度
  WIN_ANI,      //弹窗动画速度
  // SPOT_ANI,     //聚光动画速度
  // FADE_ANI,     //消失动画速度
  TAG_ANI,      //标签动画速度
  DIGI_ANI,     //数字动画滚动速度
  AIN_ALL_NUM,  //动画速度参数的数目，用于数组初始化
};
enum _ufd_kind //展开方式类别(数组中的下标)
{
  TILE_UFD = 0x00,   //磁贴图标从头展开开关
  LIST_UFD,       //菜单列表从头展开开关
  UFD_ALL_NUM,    //展开方式类别数目
} ;
enum _loop_kind //循环模式类别(数组中的下标)
{
  TILE_LOOP = 0x00,    //磁贴图标循环模式开关
  LIST_LOOP,    //菜单列表循环模式开关
  LOOP_ALL_NUM,    //循环模式类别数目
};

typedef enum
{
    ui_layer_in = 0x00, //ui层级深入时
    ui_page_proc   , //ui页面处理时
} UIState; //UI状态机


//============================================全局变量的外界声明================================

typedef struct  
{
  uint8_t valwin_broken:1;       //数值弹窗背景虚化开关
  uint8_t conwin_broken:1;       //数值弹窗背景虚化开关
  uint8_t digital_ripple:1;      //digital页面多位数值递增时是否使用ripple模式还是同时运动递增
  uint8_t raderpic_scan_mode:1;  //raderpic扫描方式的开关
  uint8_t raderpic_scan_rate;     //raderpic扫描的速度
  uint8_t raderpic_move_rate;     //raderpic过度动画的移动速度
  uint8_t ani_param[AIN_ALL_NUM]; //动画参数数组
  uint8_t ufd_param[UFD_ALL_NUM]; //展开参数数组
  uint8_t loop_param[LOOP_ALL_NUM];  //循环参数数组
} UiPara;  //UI参数集合类型
extern UiPara ui_para; //共外部使用的全局UI参数变量

extern window w_all; //这个窗口是ui绘制时的全局窗口，可供外界自由绘制

/*============================================接口函数=========================================*/
void OLED_TitlePageInit(TitlePage * title_page, uint8_t page_id,uint8_t item_num,Option* option_array,Icon *icon_array,CallBackFunc call_back);
void OLED_ListPageInit(ListPage * lp,uint8_t page_id,uint8_t item_num,Option *option_array,CallBackFunc call_back);
//用于向UI传递一个消息Msg(msg_click/msg_up/msg_down/msg_return)
void OLED_MsgQueSend(InputMsg msg);
//UI必要的一些参数和变量的初始化
void OLED_UiInit(void);
//UI运行任务，需要放在主循环中循环调用，而且尽量不要阻塞
void OLED_UIProc(void);
/* 
* 从一个页面跳转到另一个页面，常用于回调函数中调用，并确定页面的上下级关系(这样，在terminate_page页面收到return消息时，会返回self_page_id所代表的页面)
@param self_page_id 是当前页面的id（回调函数中有这个参数）
@param  terminate_page 要跳转的那个页面的地址(不需要理会是那种类型的页面，直接将页面地址作为参数传入即可)
*/
void OLED_UIJumpToPage(uint8_t self_page_id,PageAddr terminate_page);
/*
* 切换当前页面的函数，与Jump函数不同的时，这个函数不会绑定上下级页面关系，terminate_page 页面收到return 消息不会返回当前页面(常用于临时的画面切换)
@param terminate_page 要跳转的那个页面的地址(不需要理会是那种类型的页面，直接将页面地址作为参数传入即可)
*/
void OLED_UIChangeCurrentPage(PageAddr terminate_page);
/*获取当前页面的id*/
uint8_t OLED_UIGetCurrentPageID(void);


#if PAGE_WAVE_ENABLE
void OLED_WavePageInit(WavePage * wp, uint8_t page_id, uint8_t item_num, Option *option_array, CallBackFunc call_back);
void OLED_UIWaveUpdateVal(Option * op, int16_t val);
#endif

#if PAGE_RADIO_ENABLE 
void OLED_RadioPageInit(RadioPage * rp, uint8_t page_id, uint8_t item_num,Option *option_array,CallBackFunc call_back);
#endif

#if PAGE_RADERPIC_ENABLE
void OLED_RaderPicPageInit(RaderPicPage* rpp,uint8_t page_id,uint8_t pic_num,RaderPic * pic_array,RaderPicMode mode,CallBackFunc cb) ;
#endif

#if PAGE_DIGITAL_ENABLE
void OLED_DigitalPageInit(DigitalPage* dp, uint8_t page_id, Option * option_array, uint8_t  label_array_num, String * label_array, char gap_char, uint8_t gap_shine_time, uint8_t uline_shine_time,CallBackFunc cb);
void OLED_DigitalPage_UpdateDigitalNumAnimation(DigitalPage * dp, uint8_t leftval, uint8_t midval, uint8_t rightval, DigitalDirect dir);
void OLED_DigitalPage_UpdateLabelAnimation(DigitalPage * dp, uint8_t label_index, DigitalDirect dir);
#endif

#endif
