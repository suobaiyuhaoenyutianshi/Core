#include "app_encoder.h"
#include "main.h"
#include "delay.h"

// 全局变量（不再加 static）
volatile uint64_t L_UPTIM = 0;
volatile uint64_t L_DOWTIM = 0;
volatile uint32_t L_LOWTIM = 0;
volatile uint32_t L_HIGTIM = 0;

volatile uint64_t R_UPTIM = 0;
volatile uint64_t R_DOWTIM = 0;
volatile uint32_t R_LOWTIM = 0;
volatile uint32_t R_HIGTIM = 0;

void App_Encoder_Init(void)
{
    Delay_Init();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint64_t now = GetUs();

    if (GPIO_Pin == GPIO_PIN_14) // 左轮 A 相
    {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == GPIO_PIN_SET) // 上升沿
        {
            L_UPTIM = now;
            if (L_DOWTIM != 0)
                L_LOWTIM = (uint32_t)(L_UPTIM - L_DOWTIM);
        }
        else // 下降沿
        {
            L_DOWTIM = now;
            if (L_UPTIM != 0)
                L_HIGTIM = (uint32_t)(L_DOWTIM - L_UPTIM);
        }
    }

    if (GPIO_Pin == GPIO_PIN_3) // 右轮 A 相
    {
        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_SET)
        {
            R_UPTIM = now;
            if (R_DOWTIM != 0)
                R_LOWTIM = (uint32_t)(R_UPTIM - R_DOWTIM);
        }
        else
        {
            R_DOWTIM = now;
            if (R_UPTIM != 0)
                R_HIGTIM = (uint32_t)(R_DOWTIM - R_UPTIM);
        }
    }
}