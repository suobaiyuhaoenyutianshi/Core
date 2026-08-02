#ifndef APP_PWM_H
#define APP_PWM_H
#include "main.h"

void App_PWM_Init(void);
void App_PWM_Cmd(uint8_t State);         // 使能/休眠 TB6612
void App_PWM_Set_L(float Duty);          // 左电机占空比 -100~+100
void App_PWM_Set_R(float Duty);          // 右电机占空比 -100~+100

#endif