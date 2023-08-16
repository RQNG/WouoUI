#include "buzzer.h"

static uint16_t time_on = 0;

void buzzer_tick(void)
{
	if(time_on)
	{
		LL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
		time_on--;
		if(time_on == 0)
		{
			LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
		}
	}
}

void buzzer_on(uint16_t time)
{
	time_on = time;
}

void buzzer_init(void)
{
	LL_GPIO_ResetOutputPin(BUZZER_GPIO_Port, BUZZER_Pin);
}

void buzzer_deinit(void)
{

}