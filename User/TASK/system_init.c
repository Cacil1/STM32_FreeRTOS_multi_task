#include "system_init.h"
#include "usart.h"


void Init_Task_Entry(void *argument)
{
    (void)argument;

	for(;;)
	{
		osDelay(500);
		LOG("Init_Task RUNNING!");
	}
}
