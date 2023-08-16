#ifndef __RANGING_H
#define __RANGING_H

#include "main.h"
#include "stdlib.h"

enum 
{
	RANG_WAIT,
  RANG_FAIL,
  RANG_PASS,
  RANG_ERROR,
  RANG_NORMAL,
};


uint8_t ranging_start(void);
float ranging_get_length(void);
void ranging_init(void);
void ranging_deinit(void);


#endif  
	 



