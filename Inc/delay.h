#ifndef DELAY_H
#define DELAY_H
#include"main.h"
#include <stdint.h>
void Delay_Init(void);
void DelayUs(uint32_t us);
uint64_t GetUs(void);
#endif // DELAY_H