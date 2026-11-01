#include "watchdog_task.h"
#include "system_init.h"
#include "iwdg.h"
#include "stdio.h"
#include "usart.h"

#define WDG_CHECK_PERIOD_MS     500     //每500毫秒检查一次各任务状态
#define WDG_STALE_THRESHOLD_MS  1500    //1500毫秒任务还没报道，就判定挂了

static volatile uint32_t g_task_alive_tick[WDG_TASK_COUNT];

static const char *g_task_name[WDG_TASK_COUNT] = {
    "Key", "WDG_TASK_LED_BOARD"
};

void Watchdog_Checkin(WatchdogTaskId_t id)
{
    g_task_alive_tick[id] = osKernelGetTickCount();
}

void Watchdog_ResetCheckins(void)
{
    uint32_t now = osKernelGetTickCount();
    for (uint8_t i = 0U; i < WDG_TASK_COUNT; ++i)
    {
        g_task_alive_tick[i] = now;
    }
}

// 每 500ms 醒来检查一次，如果有任务超过3次没喂狗，则认为任务挂了，那么就不喂硬件看门狗，让硬件看门狗重启系统
void Watchdog_Task_Entry(void *argument)
{
    (void)argument;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();
    
    for(;;)
    {
        // 看门狗每隔500ms检查一下是否喂狗
        osDelay(WDG_CHECK_PERIOD_MS);

        uint32_t now = osKernelGetTickCount();
        uint8_t all_alive = 1;

        for(int i = 0; i < WDG_TASK_COUNT; i++)
        {
            if((now - g_task_alive_tick[i]) > WDG_STALE_THRESHOLD_MS)
            {
                LOG("[Watchdog] task %s stale, skip feed", g_task_name[i]);
                all_alive = 0;
            }
        }

        if(all_alive)
        {
            // 喂硬件看门狗
            HAL_IWDG_Refresh(&hiwdg);
        }
    }
}
