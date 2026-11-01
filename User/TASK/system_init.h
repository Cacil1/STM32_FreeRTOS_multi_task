#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__

#include "stm32f1xx_hal.h"   /* HAL �����뺯�� */
#include "main.h"

extern osEventFlagsId_t SystemReadyFlagsHandle;

#define SYSTEM_READY_FLAG  (1U << 0)

void System_Init_WaitReady(void);       // 所有任务阻塞等待init任务初始化
void System_Init_ReleaseTasks(void);    // 通知所有任务解除阻塞
void Init_Task_Entry(void *argument);

#endif
