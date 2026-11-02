#include "oled_task.h"
#include "oled.h"
#include "usart.h"
#include "watchdog_task.h"
#include "system_init.h"

/* 显示一屏测试内容 */
static void OLED_ShowDemo(void)
{
    OLED_Clear();
    OLED_ShowString(16, 0,  "Hello STM32!", 16, 1);
    OLED_ShowString(0,  20, "OLED is working", 12, 1);
    OLED_ShowNum(0,  36, 12345678, 8, 12, 1);
    OLED_ShowFloatNum(0, 52, -3.14f, 2, 2, 12, 1);
    OLED_Refresh();
}


void OLED_Task_Entry(void *argument)
{
    (void)argument;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();

    OLED_ShowDemo();
    for (;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_OLED);

        // �����в�Ҫʹ��HAL_Delay������һֱռ��CPU��osDelay���ͷ�CPU
        osDelay(500);
    }
}
