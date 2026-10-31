#include "led_board.h"
#include "usart.h"

void LED_Board_Task_Entry(void *argument)
{
    (void)argument;
	
	for(;;)
	{
		// LOG("LED_BOARD_Task RUNNING!");
		
		// �����в�Ҫʹ��HAL_Delay������һֱռ��CPU��osDelay���ͷ�CPU
		osDelay(500);
		HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
	}
}
