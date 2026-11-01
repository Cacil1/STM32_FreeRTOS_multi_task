#include "system_init.h"
#include "usart.h"
#include "watchdog_task.h"

void Init_Task_Entry(void *argument)
{
    (void)argument;

	for(;;)
	{
		osDelay(500);

		Watchdog_ResetCheckins();

		// 任务退出
		osThreadExit(); 
	}
}
