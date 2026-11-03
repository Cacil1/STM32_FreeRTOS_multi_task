#ifndef __WATCHDOG_TASK_H
#define __WATCHDOG_TASK_H

#include "cmsis_os.h"

typedef enum
{
    WDG_TASK_KEY = 0,
    WDG_TASK_LED_BOARD,
    WDG_TASK_OLED,
    WDG_TASK_MPU,
    WDG_TASK_COUNT
} WatchdogTaskId_t;

// 各任务定时报到
void Watchdog_Checkin(WatchdogTaskId_t id);
void Watchdog_ResetCheckins(void);

void Watchdog_Task_Entry(void *argument);

#endif
