#include "mpu_task.h"
#include "main.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "math.h"
#include "stdio.h"
#include "watchdog_task.h"
#include "system_init.h"
#include "usart.h"

#define Q30     1073741824.0f
#define MPU_LOG_DIVIDER   10          /* 每10次循环才打一条日志，别刷屏 */

float g_pitch = 0.0f;
float g_roll  = 0.0f;
float g_yaw   = 0.0f;

void MPU_Task_Entry(void *argument)
{
    short        gyro[3], accel[3];
    long         quat[4];
    unsigned long timestamp;
    short        sensors;
    unsigned char more;
    uint8_t      log_cnt = 0;

    (void)argument;

    System_Init_WaitReady();

    for (;;)
    {
        Watchdog_Checkin(WDG_TASK_MPU);

        /* ---- 排空式读取：醒来一次，把FIFO里积压的整包全部读完 ---- */
        do {
            if (dmp_read_fifo(gyro, accel, quat, &timestamp, &sensors, &more))
                break;                          /* 无整包/出错，下轮再战 */
        } while (more);                         /* more>0: FIFO里还有整包 */

        /* ---- 只有拿到四元数才解算（用最后一包 = 最新数据） ---- */
        if (sensors & INV_WXYZ_QUAT)
        {
            float q0 = quat[0] / Q30;
            float q1 = quat[1] / Q30;
            float q2 = quat[2] / Q30;
            float q3 = quat[3] / Q30;

            g_pitch = asin(-2.0f * q1 * q3 + 2.0f * q0 * q2) * 57.2957795f;
            g_roll  = atan2(2.0f * q2 * q3 + 2.0f * q0 * q1,
                            -2.0f * q1 * q1 - 2.0f * q2 * q2 + 1.0f) * 57.2957795f;
            g_yaw   = atan2(2.0f * (q1 * q2 + q0 * q3),
                            q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3) * 57.2957795f;

            if (++log_cnt >= MPU_LOG_DIVIDER)  /* 日志限流，5秒一条 */
            {
                log_cnt = 0;
                LOG("pitch:%.2f, roll:%.2f, yaw:%.2f", g_pitch, g_roll, g_yaw);
            }
        }

        osDelay(50);   /* 周期随意：50ms/100ms/200ms都行，排空模式下不再敏感 */
    }
}
