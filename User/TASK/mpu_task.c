#include "mpu_task.h"
#include "main.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "stdio.h"
#include "watchdog_task.h"
#include "system_init.h"
#include "usart.h"

float g_pitch = 0.0f;
float g_roll = 0.0f;
float g_yaw = 0.0f;

volatile uint32_t g_mpu_read_period = 200;

#define MPU_READ_PERIOD_MS 50 // DMP数据更新频率通常在50Hz左右，20ms采样一次比较合适

void MPU_Task_Entry(void *argument)
{
    float pitch, roll, yaw;

    (void)argument;

    System_Init_WaitReady();

    for (;;)
    {
        // 看门狗喂狗
        Watchdog_Checkin(WDG_TASK_MPU);
        mpu_read_who_am_i();
        int result = mpu_dmp_get_data(&pitch, &roll, &yaw);
        if (result == 0) // 读取成功
        {
            g_pitch = pitch;
            g_roll = roll;
            g_yaw = yaw;
            LOG("pitch:%f,roll:%f,yaw:%f", pitch, roll, yaw);
        }
        else
        {
            LOG("mpu read error");
        }

        osDelay(500);
    }
}
