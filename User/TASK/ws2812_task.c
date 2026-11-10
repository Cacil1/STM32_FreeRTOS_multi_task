#include "ws2812_task.h"
#include "system_init.h"
#include "watchdog_task.h"
#include "ws2812.h"
#include "usart.h"

void WS2812_Task_Entry(void *argument)
{
    (void)argument;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();

    WS2812_Init();

    for (;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_WS2812);
        WS2812_Tick();  // 触发DMA发送
        osDelay(20);
    }
}
