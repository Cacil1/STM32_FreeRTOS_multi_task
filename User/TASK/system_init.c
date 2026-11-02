#include "system_init.h"
#include "usart.h"
#include "iic.h"
#include "oled.h"
#include "watchdog_task.h"

void System_Init_WaitReady(void)
{
    (void)osEventFlagsWait(SystemReadyFlagsHandle, SYSTEM_READY_FLAG,
                            osFlagsWaitAny | osFlagsNoClear, osWaitForever);
}

void System_Init_ReleaseTasks(void)
{
    (void)osEventFlagsSet(SystemReadyFlagsHandle, SYSTEM_READY_FLAG);
}

void Init_Task_Entry(void *argument)
{
    (void)argument;

    iic_init();

    OLED_Init();

	for(;;)
	{
		osDelay(500);

		// 初始化软件看门狗
		Watchdog_ResetCheckins();

		// 唤醒所有等待的任务
		System_Init_ReleaseTasks();

		// 任务退出
		osThreadExit(); 
	}
}
