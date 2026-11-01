#include "led_board.h"
#include "usart.h"
#include "watchdog_task.h"

void LED_Board_Task_Entry(void *argument)
{
    (void)argument;
	
	for(;;)
	{		
		// 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_LED_BOARD);
		
		// �����в�Ҫʹ��HAL_Delay������һֱռ��CPU��osDelay���ͷ�CPU
		osDelay(500);
		HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
	}
}
