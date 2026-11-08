#include "oled_task.h"
#include "oled.h"
#include "usart.h"
#include "watchdog_task.h"
#include "system_init.h"
#include "key_task.h"

extern osMessageQueueId_t KeyQueueHandle;

/* 显示一屏测试内容 */
static void OLED_ShowDemo(void)
{
    OLED_Clear();
    OLED_ShowString(16, 0, "Hello STM32!", 16, 1);
    OLED_ShowString(0, 20, "OLED is working", 12, 1);
    OLED_ShowNum(0, 36, 12345678, 8, 12, 1);
    OLED_ShowFloatNum(0, 52, -3.14f, 2, 2, 12, 1);
    OLED_Refresh();
}

static char *keyEvt2String(uint16_t evt)
{
    switch (evt)
    {
    case 0:
        return "KEY_NONE";
    case 1:
        return "KEY0_SHORT";
    case 2:
        return "KEY0_LONG";
    case 3:
        return "KEY1_SHORT";
    case 4:
        return "KEY1_LONG";
    case 5:
        return "KEY_UP_SHORT";
    case 6:
        return "KEY_UP_LONG";
    default:
        return "KEY_NONE";
    }
}

void OLED_Task_Entry(void *argument)
{
    (void)argument;
    uint16_t evt;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();

    OLED_ShowDemo();
    for (;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_OLED);

        osStatus_t status = osMessageQueueGet(KeyQueueHandle, &evt, NULL, 1000);
        if (status == osOK)
        {

            OLED_Clear();
            OLED_ShowString(16, 0, (uint8_t *)keyEvt2String(evt), 16, 1);
            OLED_Refresh();
        }

        // �����в�Ҫʹ��HAL_Delay������һֱռ��CPU��osDelay���ͷ�CPU
        osDelay(400);
    }
}
