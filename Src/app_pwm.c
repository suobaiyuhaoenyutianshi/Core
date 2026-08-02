#include "app_pwm.h"
#include "math.h"
#define PWM_ARR 99  // 和 CubeMX 里 TIM1/TIM4 的 ARR 一致

void App_PWM_Init(void)
{
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);   // 左电机 PA8
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);   // 右电机 PB6
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET); // STBY 使能
}

void App_PWM_Cmd(uint8_t State)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, State ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void App_PWM_Set_L(float Duty)
{
    if (Duty > 100.0f)  Duty = 100.0f;
    if (Duty < -100.0f) Duty = -100.0f;

    if (Duty >= 0)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);   // AIN2
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,  GPIO_PIN_RESET); // AIN1
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9,  GPIO_PIN_SET);
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)(fabsf(Duty) / 100.0f * (PWM_ARR + 1)));
}

void App_PWM_Set_R(float Duty)
{
    if (Duty > 100.0f)  Duty = 100.0f;
    if (Duty < -100.0f) Duty = -100.0f;

    if (Duty >= 0)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);   // BIN1
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET); // BIN2
    }
    else
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
    }
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, (uint16_t)(fabsf(Duty) / 100.0f * (PWM_ARR + 1)));
}