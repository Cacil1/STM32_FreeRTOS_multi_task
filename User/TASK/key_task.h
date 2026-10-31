
#ifndef __KEY_TASK_H
#define __KEY_TASK_H

// ---------- 按键事件定义 ----------
typedef enum {
    KEY_NONE = 0,
    KEY0_SHORT,
    KEY0_LONG,
    KEY1_SHORT,
    KEY1_LONG,
    KEY_UP_SHORT,
    KEY_UP_LONG,
} KeyEvent_t;

void Key_Scan_Task_Entry(void *argument);

#endif
