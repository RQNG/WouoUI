#ifndef __IR_SW_H
#define __IR_SW_H

#include "main.h"
#include "stdlib.h"

typedef enum
{
	IR_COUNT = 1,
}IR_IN_MODE;


void ir_sw_tick(void);
void ir_trigger(void);
void ir_sw_set_mode(uint8_t mode);
uint32_t ir_sw_get_count(void);
void ir_sw_set_count(uint32_t n);
uint32_t ir_sw_get_now_time(void);
uint8_t ir_sw_get_time(void);
uint8_t *ir_sw_get_time_point(void);
void ir_sw_set_time(uint32_t n);
uint8_t ir_sw_get_out_state(void);
void ir_sw_init(void);
void ir_sw_deinit(void);


#endif  
	 



