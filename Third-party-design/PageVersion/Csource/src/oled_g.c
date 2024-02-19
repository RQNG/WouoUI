#include "oled_g.h"
#include "string.h"
#include "oledfont.h"
#include "math.h"

#define OLED_BUFF_SIZE    (OLED_HEIGHT/8*OLED_WIDTH)

uint8_t oled_buff[OLED_HEIGHT/8][OLED_WIDTH] = {0}; //全局buff，对应oled中的GRAM
uint8_t oled_color = '|'; //默认| 表示白色，'&'表示黑色

/**
* @brief : void OLED_ClearBuff(void)
* @param : Nonw
* @attention : 清空整个buff
* @author : Sheep
* @date : 23/10/04
*/
void OLED_ClearBuff(void)
{
	memset(oled_buff,0,1024);
}

/**
* @brief : void OLED_SetPointColor(uint8_t color)
* @param : 设置画笔颜色，color：1=亮，0=灭,2=反色
* @attention : None
* @author : Sheep
* @date : 23/10/04
*/
void OLED_SetPointColor(uint8_t color)
{
	switch (color)
	{
		case 0: oled_color = '&';break;
		case 1: oled_color = '|';break;
		case 2: oled_color = '^';break;
		default:break;
	}
	// oled_color = (color != 0 )?'|':'&';
} 


/**
* @brief : void OLED_RefreshBUff(void)
* @param : 更新整个buff到oledGRAM
* @attention : 返回oled_ok表示成功
* @author : Sheep
* @date : 23/10/04
*/
void OLED_RefreshBuff(void)
{
   OLED_SendBuff(oled_buff);
}


/*
函数：void OLED_BuffWriteByte(uint8_t x, uint8_t y , uint8_t val)
参数：x y 写入buff的位置，一整个字节的起始位置 coverORadd 对应的字节与\或还是直接覆盖
注意'='是直接将输入赋值给buff，'|'是将输入为1的位在buff中置1(写白点)，'&'内部会自动取反的，及将输入的val中为1的位在buff中置0（写黑点）
返回值：无
说明：主要是用于数组写入时防止越界
作者:Sheep
日期：23/09/27
*/
static void OLED_BuffWriteByte(int16_t x, int16_t y ,uint8_t coverORaddORclear, uint8_t val)
{
	if( x > (OLED_WIDTH-1) || y>(OLED_HEIGHT/8-1) || x<0 || y<0) return;
	switch (coverORaddORclear)
	{
		case '|':oled_buff[y][x] |= val;break;
		case '&':oled_buff[y][x] &= (~val);break;
		case '^':oled_buff[y][x] ^= val;break;
		default:
			break;
	}
}

/**
* @brief : static void OLED_WinWriteByte(window * win ,int16_t x, int16_t y, uint8_t val)
* @param : win 画图窗口，x相对于画图原点的横坐标，y相对于画图原点的纵坐标，写入的一个字节
* @attention : OLED_OK/OUT/ERR
* @author : Sheep
* @date : 23/10/03
*/
static void OLED_WinWriteByte(window * win ,int16_t x, int16_t y, uint8_t val)
{
    uint8_t n = 0,m = 0,temp1 = 0,temp2 = 0;
    if(x > win->w || y > win->h || x < 0 || y <= -7) return ; //超出窗口大小
    if(y+7 > (win->h -1) ) val &= (~(0xFF<<(win->h - y))); //超过窗口的部分不显示
	if(y > -7 && y < 0)
	{
		val >>=(-1*y);
		n = win->start_y/8;
		m = win->start_y%8;
	} 
	else
	{
    	n = (win->start_y + y)/8; m = (win->start_y + y)%8;
	}
	temp1 = val<<m; temp2 = (val>>(8-m)); 
    //在全局buff中第几个字节的第几位 ,取出低字节的位,取出高字节的位
    if(m==0) OLED_BuffWriteByte(win->start_x+x,n,oled_color,val); //恰好是整字节
    else if(m != 0)
    {
        OLED_BuffWriteByte(win->start_x+x,n,oled_color,temp1);        
        OLED_BuffWriteByte(win->start_x+x,n+1,oled_color,temp2);        
    }
}


/*
函数：void OLED_WinDrawASCII(window *win,int16_t x, int16_t y ,uint8_t size, char c)
参数：[in] win， x，y ,size(12,16,24) c
返回值：目前递增到的x的位置
说明：绘制单个字符
作者:Sheep
日期：24/01/30
*/
int16_t OLED_WinDrawASCII(window *win,int16_t x, int16_t y ,uint8_t size, char c)
{
	c = c - ' '; //得到偏移值
	switch (size)
	{
		case 16://16号字8x16
			for(uint8_t i=0;i<8;i++)
				{
					OLED_WinWriteByte(win,x,y,F8X16[c*2][i]);
					OLED_WinWriteByte(win,x,y+8,F8X16[c*2+1][i]);
					x++;if(x > win->w)break; //已经超出边框没必要再写了
				}
			break;        
		case 24://24号字12*24
			for(uint8_t i=0;i<12;i++)
				{
					OLED_WinWriteByte(win,x,y,F12X24[c*3][i]);
					OLED_WinWriteByte(win,x,y+8,F12X24[c*3+1][i]);
					OLED_WinWriteByte(win,x,y+16,F12X24[c*3+2][i]);
					x++;if(x > win->w)break; //已经超出边框没必要再写了
				}
			break;        
		case 8://12号字6*8
			for(uint8_t i=0;i<6;i++)
				{
					OLED_WinWriteByte(win,x,y,F6x8[c][i]);
					x++;if(x > win->w)break; //已经超出边框没必要再写了
				}
			break;
		default:
			break;
	}
	return x;
}


/**
* @brief : void OLED_WinDrawStr(window *win,int16_t x, int16_t y ,uint8_t size, uint8_t* str)
* @param : win指定窗口，x，y相对于窗口的恒重坐标，str 字符串
* @attention : None
* @author : Sheep
* @date : 23/10/03
*/
void OLED_WinDrawStr(window *win,int16_t x, int16_t y ,uint8_t size, uint8_t* str)
{
	int16_t cur_x = x; //由于绘制时x的递增在drawAscii中完成，所以利用返回值将X位置返回出来，下次传入
	while(*str != '\0')
	{
		cur_x = OLED_WinDrawASCII(win, cur_x, y, size, *str);
		str++;
		if(x > win->w) break; //已经到边了没必要再写了
	}
}



/**
* @brief : void OLED_WinDrawVLine(window *win,int16_t x, int16_t y_start, int16_t y_end)
* @param : 往指定窗口中画线，注意，y_end 必须大于y_start
* @attention : None
* @author : Sheep
* @date : 23/10/03
*/
void OLED_WinDrawVLine(window *win,int16_t x, int16_t y_start, int16_t y_end)
{	
	uint8_t n = 0, m = 0;
	if(y_start < 0 && y_end < 0)return;
	if(y_start < 0) y_start = 0;
	if(y_end > win->h )y_end = win->h;
	if(x > win->w || x < 0)return ;
	if((y_end - y_start) < 7) OLED_WinWriteByte(win,x,y_start, ~(0xFF<<(y_end - y_start)));
	else 
	{
		uint8_t i = 0;
		n = (y_end - y_start)/8;
		m = (y_end - y_start)%8;
		for(i = 0; i < n; i++)OLED_WinWriteByte(win,x,y_start+i*8,0xFF);
		OLED_WinWriteByte(win,x,y_start+i*8,~(0xFF<<m));
	}
}


/**
* @brief : void OLED_WinDrawHLine(window * win, int16_t x_start, int16_t x_end, int16_t y)
* @param : 注意x_end > x_start
* @attention : 错误代码
* @author : Sheep
* @date : 23/10/04
*/
void OLED_WinDrawHLine(window * win, int16_t x_start, int16_t x_end, int16_t y)
{
	if(x_start < 0) x_start = 0;
	if(x_end > win->w ) x_start = win->w;
	if(y > win->h || y < 0)return ;
	for(uint8_t i = 0; i < (x_end - x_start); i++)
		OLED_WinWriteByte(win,x_start+i,y,0x01);
}


/**
* @brief : void OLED_WinDrawRBox(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r)
* @param : r 倒角像素的大小
* @attention : 绘制倒角矩形,注意倒角不能大于宽或高的1/2
* @author : Sheep
* @date : 23/10/04
*/
void OLED_WinDrawRBox(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r)
{
	uint8_t cir_r = r; //
	if((2*r > width) || (2*r > height))return;
	for(uint8_t i = 0; i < width; i++)
	{
		OLED_WinDrawVLine(win,x_start+i,y_start+r,y_start+height-r);
		if(i < cir_r)r--;
		if(i >= (width - cir_r-1))r++;
	}
}

/**
* @brief : void OLED_WinDrawRBoxEmpty(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r)
* @param :  在窗口内绘制空心的倒角矩形,r 倒角像素的大小
* @attention : 绘制倒角矩形,注意倒角不能大于宽或高的1/2
* @author : Sheep
* @date : 23/10/04
*/
void OLED_WinDrawRBoxEmpty(window *win, int16_t x_start, int16_t y_start, int16_t width, int16_t height, uint8_t r)
{
	uint8_t cir_r = r; //默认倒角为2
	if((2*r > width) || (2*r > height))return;
	for(uint8_t i = 0; i < height ; i++)
	{
		if(i == 0 || i == (height-1))OLED_WinDrawHLine(win,x_start+r,x_start+width-r+1,y_start+i);
		else
		{
			OLED_WinWriteByte(win,x_start+r,y_start+i,0x01);			
			OLED_WinWriteByte(win,x_start+width-r,y_start+i,0x01);
		}
		if(i < cir_r)r--;
		if(i>= (height -cir_r -1) )r++;
	}
}

/**
* @brief : void OLED_WinDrawBMP(window * win, int16_t x, int16_t y, int16_t width, int16_t height,const uint8_t * BMP, uint8_t color)
* @param : heigh must be a total times of 8, the color is convenient for draw inverse-color BMP
* @attention : None
* @author : Sheep
* @date : 23/10/06
*/
void OLED_WinDrawBMP(window * win, int16_t x, int16_t y, int16_t width, int16_t height,const uint8_t * BMP, uint8_t color)
{
	if(BMP == NULL) return; //如果是NULL，直接返回
	uint8_t n = height/8, m = height%8;
    for(uint8_t i = 0; i < n; i++)
    {
        for(uint8_t j = 0; j < width; j++)
        {
            if(x+j > win->w)break;
            if(y+i*8 > win->h)return;
			if(color == 1) OLED_WinWriteByte(win,x+j,y+i*8,(BMP[i*width+j]));
			else OLED_WinWriteByte(win,x+j,y+i*8,~(BMP[i*width+j]));
        }   
    }
	if(m!=0)
	{
        for(uint8_t j = 0; j < width; j++)
        {
            if(x+j > win->w)return;
			if(color == 1) OLED_WinWriteByte(win,x+j,y+n*8,(BMP[n*width+j] & (~(0xFF<<m))));
			else OLED_WinWriteByte(win,x+j,y+n*8,~(BMP[n*width+j] & (~(0xFF<<m))) );
        }   
	}
}

/**
* @brief : void OLED_WinDrawPoint(window * win, int16_t x, int16_t y)
* @param : //画点函数尚未测试
* @attention : None
* @author : Sheep
* @date : 23/10/15
*/
void OLED_WinDrawPoint(window * win, int16_t x, int16_t y)
{
	OLED_WinWriteByte(win,x,y,0x01);
}



/**
* @brief : uint8_t OLED_GetStrWidth(const char * str, uint8_t size)
* @param : 得到字符串的宽度，size字号只有8，16，24三种选择
* @attention : len
* @author : Sheep
* @date : 23/10/08
*/
uint8_t OLED_GetStrWidth(const char * str, uint8_t size)
{
	uint8_t len = 0;
	if(size == 8) len = strlen(str)*6; //由于8号字体是8*6的
	else len = strlen(str)*size/2; //16-16x8；24-24x12.
	return len;
}


/**
* @brief : void OLED_Animation(float *a, float *a_trg, float n)
* @param : 非线性运动函数，n为运动步长(运动的距离占距离目标10倍的多少)
* @attention : None
* @author : Sheep
* @date : 23/10/04
*/
void OLED_Animation(float *a, float *a_trg, float n)
{
  if (*a != *a_trg)
  {
    if (fabs(*a - *a_trg) < 0.15) *a = *a_trg;
    else *a += (*a_trg - *a) / (n / 10.0); //慢慢接近，
  }
}


/**
* @brief : void OLED_AllSrcFade(uint8_t Odd0OrEven1,uint8_t byte)
* @param : 全屏的逐字节虚化函数，OddorEven 表示要虚化奇数字节1还是偶数字节0，
			byte为填充的内容(虚化时推荐有0xAA,0x55,0xFF)
* @attention : 注意，该字节的对应位置0还是置1由OLED_SetPointColor设置的颜色决定
* @author : Sheep
* @date : 23/10/10
*/
void OLED_AllSrcFade(uint8_t Odd0OrEven1,uint8_t byte)
{

	for (uint8_t i = 0; i < OLED_HEIGHT/8; i++)
		for (uint8_t j = 0; j < OLED_WIDTH; j++)
			if(j%2 == Odd0OrEven1) OLED_BuffWriteByte(j,i,oled_color,byte);
}


/**
* @brief : void OLED_WinDrawLine(window* win,int16_t x1, int16_t y1, int16_t x2, int16_t y2)
* @param : (x1,y1)为起点，(x2,y2)为终点
* @attention : 使用Bresenham算法进行画直线,注意，x1 != x2
* @author : Sheep
* @date : 23/10/31
*/
void OLED_WinDrawLine(window* win,int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	int16_t x_small = 0, x_big = 0, temp = 0, y_small = 0, y_big = 0;
	int32_t dx = 0, dy = 0, p = 0; // deltax 和deltay
	if(x2 == x1)return;
	else p = (y2-y1)/(x2-x1);
	p = (p>0)?p:(-p); //求斜率的绝对值,后面作误差变量使用
	if(p < 1) //p>0 && p<1
	{
		if(x2 > x1){x_small = x1; x_big = x2; }
		else{x_small = x2; x_big = x1; temp = y1; y1 = y2; y2 = temp;} //默认与 xsmall对应的y是y1
		dx = x_big - x_small;
		dy = ((y1 - y2)>0)?(y1-y2):(y2-y1);
		p = (dy<<1) - dx;
		while (x_small <= x_big)
		{
			OLED_WinWriteByte(win, x_small, y1, 0x01);
			if(p > 0)
			{
				if(y1 < y2)y1++;
				else y1--;
				p = p+(dy<<1)-(dx<<1);
			}
			else {p = p+(dy<<1);}
			x_small++;
		}
	}
	else //p>=1
	{
		if(y2 > y1){y_small = y1; y_big = y2;}
		else {y_small = y2; y_big = y1; temp = x1; x1 = x2; x2 = temp;} //默认与y_small对应的是x1;
		dx = (x1>x2)?(x1-x2):(x2-x1);
		dy = y_big - y_small;
		p = (dx<<1) - dy;
		while (y_small <= y_big)
		{
			OLED_WinWriteByte(win, x1, y_small, 0x01);
			if(p > 0)
			{
				if(x1 < x2)x1++; 
				else x1--;
				p = p+(dx<<1)-(dy<<1);
			}
			else {p = p+(dx<<1);}
			y_small++;
		}
	}
}

/**
* @brief : uint8_t OLED_WinDrawRaderPic(window * win,const uint8_t* pic,int16_t start_x, uint16_t start_y, uint8_t width, uint8_t height, RaderDirection direct, uint8_t enable_all_point_scan)
* @param : [in] win 显示的窗口对象
* @param : [in] pic 显示的图片对象
* @param : [in] start_x，start_y,width,height,图片的左上角(x,y) 和宽高
* @param : [in] direct 射线方向，枚举类型RaderDirection
* @param : [in] enable_all_point_scan true/Fasle，Fasle的话，只有图片亮的点才会生成射线扫描
* @param : [in] scan_rate 扫描的速度，值越大越快
* @return : 若整体图像绘制完成时，返回1，否则返回0
* @attention : None
* @author : Sheep
* @date : 23/11/15
*/
uint8_t OLED_WinDrawRaderPic(window * win,const uint8_t* pic,int16_t start_x, uint16_t start_y, uint8_t width, uint8_t height, RaderDirection direct, uint8_t enable_all_point_scan, uint8_t scan_rate)
{
	static uint16_t now_end_point = 1; //静态计数变量（允许在描文字的过程中被打断）
	uint8_t mask = 1, horizon_line_flag = 0, ret = 0;
	int16_t x = 0, y = 0;
	uint16_t i = 0,array_index = 0;
	if(pic == NULL) return 0; //如果图像为null，直接返回0，表示未完成
	//绘制前面绘制好的图形
	for(i = 0; i < now_end_point; i++)
	{
		if((i % width == 0) && (i!=0))
			if(mask == 0x80){mask = 1;}
			else {mask <<= 1;array_index-=width;}
		if(pic[array_index]&mask)
			OLED_WinDrawPoint(win,start_x + i%width, start_y + i/width);
		array_index++;
	}
	//绘制镭射那根线
	if((pic[array_index]&mask) || enable_all_point_scan)
	{
		switch (direct) //确定端点
		{
			case RD_LEFT_UP: x = win->start_x; y = win->start_y; break;
			case RD_LEFT_DOWN: x = win->start_x; y = win->start_y+win->h-1; break;
			case RD_RIGHT_UP: x = win->start_x + win->w-1; y = win->start_y; break;
			case RD_RIGHT_DOWN: x = win->start_x+win->w-1; y = win->start_y+win->h-1; break;
			case RD_LEFT: horizon_line_flag = 1;x = win->start_x;break;
			case RD_RIGHT: horizon_line_flag = 1;x = win->start_x+win->w;break;
			default:
				break;
		}
		if(horizon_line_flag)
			if(start_x + i%width < x)OLED_WinDrawHLine(win,start_x + i%width,x,start_y + i/width);
			else OLED_WinDrawHLine(win,x , start_x + i%width,start_y + i/width);
		else
			if(i%width != x){OLED_WinDrawLine(win,start_x + i%width,start_y + i/width,x,y);}
			else 
				if(start_y + i/width < y )OLED_WinDrawVLine(win,x,start_y + i/width,y);
				else OLED_WinDrawVLine(win,x,y,start_y+i/width);
	}
	//终点自增
	if(now_end_point < (width*height-1)){now_end_point+=scan_rate;ret = 0;}
	else {now_end_point = 1;ret= 1;} //图像绘制结束
	return ret;
}

