#ifndef TASK_H
#define TASK_H
#include "main.h"

// 基础版：一个模块只有一个周期性任务
#define PERIODIC(ms) \
    static uint32_t __last = 0; \
    uint32_t __now = HAL_GetTick(); \
    if ((__now - __last) < (ms)) return; \
    __last = __now;

// 高级版：一个模块有多个任务，给每个任务起名字
#define PERIODIC_START(name, ms) \
    static uint32_t __last_##name = 0; \
    uint32_t __now_##name = HAL_GetTick(); \
    if ((__now_##name - __last_##name) >= (ms)) {

#define PERIODIC_END(name)  \
    __last_##name = __now_##name; \
    }

#endif