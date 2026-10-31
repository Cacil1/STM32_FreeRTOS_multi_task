#include "led.h"

void LED_All_Toggle(void)
{
	HAL_GPIO_TogglePin(RED_GPIO_Port, RED_Pin);
	HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
	
	return;
}
