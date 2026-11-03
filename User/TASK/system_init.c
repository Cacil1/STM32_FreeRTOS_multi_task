#include "system_init.h"
#include "usart.h"
#include "iic.h"
#include "oled.h"
#include "inv_mpu.h"
#include "soft_spi.h"
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

	soft_spi_init();

    OLED_Init();

	uint8_t res = mpu_dmp_init();
LOG("mpu_dmp_init result = %u", res);

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
