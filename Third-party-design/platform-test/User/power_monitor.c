#include "power_monitor.h"

#define CHANNEL_SIZE	2

uint16_t ADC_ConvertedValue[CHANNEL_SIZE];
float offset_channela[CHANNEL_SIZE];

uint16_t power_monitor_read_value(uint8_t channel)
{
	if(channel > (CHANNEL_SIZE - 1))
		channel = (CHANNEL_SIZE - 1);
	return ADC_ConvertedValue[channel];
}

void calibrate_offsets(void)
{
	int i;
	for(i=0; i<1000; i++)
	{
		offset_channela[0] += ADC_ConvertedValue[0];
		offset_channela[1] += ADC_ConvertedValue[1];
		LL_mDelay(1);
	}
	// calculate the mean offsets
	offset_channela[0] = offset_channela[0]/1000;
	offset_channela[1] = offset_channela[1]/1000;
	if(offset_channela[1] < 2044 || offset_channela[1] > 2048)
	{
		offset_channela[1] = 2046;
	}
}


void power_monitor_init(void)
{
	LL_DMA_SetDataLength(DMA1,LL_DMA_CHANNEL_1,CHANNEL_SIZE);
	LL_DMA_SetPeriphAddress(DMA1,LL_DMA_CHANNEL_1,LL_ADC_DMA_GetRegAddr(ADC1,     LL_ADC_DMA_REG_REGULAR_DATA));
	LL_DMA_SetMemoryAddress(DMA1,LL_DMA_CHANNEL_1,(uint32_t)&ADC_ConvertedValue);
	LL_DMA_EnableChannel(DMA1,LL_DMA_CHANNEL_1);
	
	LL_ADC_Enable(ADC1);
	// ADC开始校准
	LL_ADC_StartCalibration(ADC1);
	// 等待校准完成
	while(LL_ADC_IsCalibrationOnGoing(ADC1));
	//设置ADC组通过DMA定期转换数据传输
	LL_ADC_REG_SetDMATransfer(ADC1,LL_ADC_REG_DMA_TRANSFER_UNLIMITED);
 
	//LL_ADC_REG_StartConversion(ADC1);        //对于F0系列的芯片需要加上这句代码
	
	LL_mDelay(10);
	calibrate_offsets();
}


void power_monitor_deinit(void)
{


}