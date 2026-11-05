#include "flash_task.h"
#include "watchdog_task.h"
#include "system_init.h"

void Flash_Task_Entry(void *argument)
{
    (void)argument;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();

    for (;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_FLASH);
    }
}
