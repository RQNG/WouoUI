#include "ranging.h"

#define TRIG1_GPIO_Port 		SPI1_MOSI_GPIO_Port
#define TRIG1_Pin						SPI1_MOSI_Pin
#define ECHO_RX1_GPIO_Port	SPI1_SCK_GPIO_Port
#define ECHO_RX1_Pin				SPI1_SCK_Pin

float ranging_calc = 0;

void ranging_delay_us(uint16_t us)
{
	LL_TIM_SetCounter(TIM4, 0);
	LL_TIM_EnableCounter(TIM4);
	while(LL_TIM_GetCounter(TIM4) < us);
	LL_TIM_DisableCounter(TIM4);
}

uint8_t ranging_start(void)
{
	LL_GPIO_SetOutputPin(TRIG1_GPIO_Port,TRIG1_Pin);
	ranging_delay_us(20);
	LL_GPIO_ResetOutputPin(TRIG1_GPIO_Port,TRIG1_Pin);

	LL_TIM_SetCounter(TIM4, 0);
	LL_TIM_EnableCounter(TIM4);
	while(LL_GPIO_IsInputPinSet(ECHO_RX1_GPIO_Port,ECHO_RX1_Pin) == RESET)
	{
		if(LL_TIM_GetCounter(TIM4) > 65530)
		{
			LL_TIM_DisableCounter(TIM4);
			goto timer_out;
		}
	}
	LL_TIM_DisableCounter(TIM4);
	LL_TIM_SetCounter(TIM4, 0);
	LL_TIM_EnableCounter(TIM4);
	while(LL_GPIO_IsInputPinSet(ECHO_RX1_GPIO_Port,ECHO_RX1_Pin) == SET)
	{
		if(LL_TIM_GetCounter(TIM4) > 65530)
		{
			LL_TIM_DisableCounter(TIM4);
			goto timer_out;
		}
	}
	LL_TIM_DisableCounter(TIM4);
	
	ranging_calc = (LL_TIM_GetCounter(TIM4)/1000.0)*342.62/2;
	if(ranging_calc < 0)
	{
		goto wait_data;
	}	
	else if(ranging_calc > 7500)
	{
		goto error_data;
	}
	else if(ranging_calc > 300 && ranging_calc < 400)
	{
		buzzer_on(200);
		return RANG_PASS;
	}
	return RANG_NORMAL;
wait_data:
	return RANG_WAIT;
error_data:
	return RANG_ERROR;
timer_out:
	return RANG_FAIL;
}

float ranging_get_length(void)
{
	return ranging_calc;
}

void ranging_time_init(void)
{
  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM4);

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  TIM_InitStruct.Prescaler = 71;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM4, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM4);
  LL_TIM_SetClockSource(TIM4, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM4, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM4);
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
}

void ranging_init(void)
{
  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = TRIG1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  LL_GPIO_Init(TRIG1_GPIO_Port, &GPIO_InitStruct);
	
  GPIO_InitStruct.Pin = ECHO_RX1_Pin;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  LL_GPIO_Init(ECHO_RX1_GPIO_Port, &GPIO_InitStruct);
	
  LL_GPIO_ResetOutputPin(TRIG1_GPIO_Port, TRIG1_Pin);  
	LL_GPIO_ResetOutputPin(ECHO_RX1_GPIO_Port, ECHO_RX1_Pin);
	ranging_time_init();
	LL_GPIO_ResetOutputPin(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
	LL_GPIO_SetOutputPin(VCC_3V3_CRTL_GPIO_Port, VCC_3V3_CRTL_Pin);
}

void ranging_deinit(void)
{
	LL_GPIO_ResetOutputPin(VCC_5V_CTRL_GPIO_Port, VCC_5V_CTRL_Pin);
	LL_GPIO_ResetOutputPin(VCC_3V3_CRTL_GPIO_Port, VCC_3V3_CRTL_Pin);
	ranging_calc = -1;
}