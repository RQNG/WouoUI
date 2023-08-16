#include "ir_sw.h"

uint8_t mode = 0;
uint8_t save_flag = 0;
uint8_t trigger_flag = 0;
uint8_t ir_trigger_time = 0;
int16_t ir_trigger_now_time = 0;
uint32_t ir_tick = 0;
uint32_t ir_trigger_count = 0;

void ir_sw_tick(void)
{
	if(mode != IR_COUNT)
		return;
	if(ir_tick > 0)
	{
		ir_tick--;
		return;
	}
	ir_tick = 1000;

	if(trigger_flag)
	{
		trigger_flag = false;
		if(LL_GPIO_IsInputPinSet(IR_IN_GPIO_Port ,IR_IN_Pin) == RESET)
			ir_trigger_count++;
		save_flag++;
		if(save_flag > 10)
		{
			save_flag = 0;
			eeprom_notify_change();
		}	
	}
	
	ir_trigger_now_time--;
	if(ir_trigger_now_time <= 0)
	{
		ir_trigger_now_time = ir_trigger_time;
		LL_GPIO_TogglePin(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
	}
}

void ir_trigger(void)
{
	if(mode != IR_COUNT)
		return;
	trigger_flag = true;
}

void ir_sw_set_mode(IR_IN_MODE _mode)
{
	mode = _mode;
}

uint32_t ir_sw_get_count(void)
{
	return ir_trigger_count;
}

void ir_sw_set_count(uint32_t n)
{
	ir_trigger_count = n;
}

uint32_t ir_sw_get_now_time(void)
{
	return ir_trigger_now_time;
}

uint8_t ir_sw_get_time(void)
{
	return ir_trigger_time;
}

uint8_t *ir_sw_get_time_point(void)
{
	return &ir_trigger_time;
}

void ir_sw_set_time(uint32_t n)
{
	ir_trigger_time = n;
	ir_trigger_now_time = ir_trigger_time;
}

uint8_t ir_sw_get_out_state(void)
{
	return LL_GPIO_IsOutputPinSet(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
}

void ir_sw_init(void)
{
	LL_GPIO_SetOutputPin(VCC_IR_CTRL_GPIO_Port, VCC_IR_CTRL_Pin);
	LL_GPIO_SetOutputPin(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
	ir_trigger_now_time = ir_trigger_time;
	mode = IR_COUNT;
}

void ir_sw_deinit(void)
{
	mode = 0;
	ir_trigger_now_time = 0;
	ir_tick = 0;
	LL_GPIO_ResetOutputPin(VCC_IR_CTRL_GPIO_Port, VCC_IR_CTRL_Pin);
	LL_GPIO_ResetOutputPin(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
}