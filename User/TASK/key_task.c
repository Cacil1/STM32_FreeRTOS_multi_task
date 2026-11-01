#include "key_task.h"
#include "cmsis_os.h"
#include "usart.h"
#include "watchdog_task.h"
#include "system_init.h"

#define LONG_PRESS_TICKS   pdMS_TO_TICKS(200)
#define DEBOUNCE_MS        20
#define SCAN_PERIOD_MS     10

static uint8_t Read_Key_Pin(uint8_t key_index)
{
    switch(key_index)
    {
        case 0: return HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
        case 1: return HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
        case 2: return !HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
        default: return 1;
    }
}

static char *index2KeyName(uint8_t index)
{
    if(index == 0)
    {
        return "KEY0";
    }
    else if(index == 1)
    {
        return "KEY1";
    }
    else if(index == 2)
    {
        return "KEY_UP";
    }

    return "ERROR";
}

void Key_Scan_Task_Entry(void *argument)
{
    uint8_t key_last_state[3] = {1, 1, 1};   //KEY0, KEY1, KEY_UP
    uint32_t press_start_tick[3] = {0};     // 按下开始的时间戳

    (void)argument;

    // 阻塞等待init任务初始化完成
    System_Init_WaitReady();

    for(;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_KEY);

        for(int i = 0; i < 3; i++)
        {
            uint8_t cur = Read_Key_Pin(i);

            // ---- 检测到按下（下降沿）----
            if(cur == 0 && key_last_state[i] == 1)
            {
                osDelay(DEBOUNCE_MS); // 延时去抖动
                if(Read_Key_Pin(i) == 0) // 二次确认，还是低电平，认为是真按下
                {
                    press_start_tick[i] = osKernelGetTickCount(); // 记录按下时间戳
                    key_last_state[i] = 0; // 更新状态
                    LOG("press %s",index2KeyName(i));
                }
            }
            // ---- 检测到松开（上升沿）----
            else if(cur == 1 && key_last_state[i] == 0)
            {
                osDelay(DEBOUNCE_MS); // 延时去抖动
                if(Read_Key_Pin(i) == 1) // 二次确认，还是高电平，认为是真松开（这里实际上松手才算长按，有些需求是长按指定时间后就触发）
                {
                    uint32_t duration = osKernelGetTickCount() - press_start_tick[i];

                    // 判断是长按还是短按
                    if(duration >= LONG_PRESS_TICKS)
                    {
                        LOG("release %s long",index2KeyName(i));
                    }
                    else
                    {
                        LOG("release %s short",index2KeyName(i));
                    }

                    key_last_state[i] = 1;
                }
            }
        }

        osDelay(SCAN_PERIOD_MS);
    }
}
