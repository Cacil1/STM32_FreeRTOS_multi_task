#include "system_init.h"
#include "usart.h"

void LED_Board_Task_Entry(void *argument)
{
    (void)argument;
	
	for(;;)
	{
		LOG("LED_BOARD_Task RUNNING!");
		
		// 任务中不要使用HAL_Delay，他会一直占用CPU；osDelay会释放CPU
		osDelay(500);
		HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
	}
}
