#include "app_bat.h"
#include "main.h"
#include "stm32_hal_legacy.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_adc.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>
#include "task.h"
#include "app_usart2.h"
#include "app_bat.h"
#include "main.h"
#include "task.h"

static volatile float Volt;

void APP_batInit(void)
{
    HAL_TIM_Base_Start(&htim3);   // 启动 TIM3，TRGO 自动触发 ADC
    HAL_ADC_Start_IT(&hadc1);     // 使能 ADC 的 EOC 中断
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadC)
{
    if (hadC->Instance == ADC1)
    {
        uint32_t val = HAL_ADC_GetValue(&hadc1);
        Volt = val / 4095.0f * 8.4f;
    }
}

float App_batGet(void)
{
    return Volt;
}

void App_Bat_Pro(void)
{
    static uint8_t ledState = 0;
    PERIODIC(20);
    
    if (Volt < 6.0f)
    {
        static uint32_t lastBlink = 0;
        if (HAL_GetTick() - lastBlink >= 100)
        {
            lastBlink = HAL_GetTick();
            ledState = !ledState;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6,
                              ledState ? GPIO_PIN_RESET : GPIO_PIN_SET);
        }
    }
    else if (Volt < 6.6f)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_RESET);
    }
    else if (Volt < 7.3f)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_RESET);
    }
    else if (Volt < 8.0f)
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
    }
}