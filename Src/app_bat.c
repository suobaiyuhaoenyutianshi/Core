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
static volatile float Volt;

void APP_batInit(){
    HAL_TIM_Base_Init(&htim3);
    HAL_ADC_Start_IT(&hadc1);
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadC){
    if (hadC->Instance == ADC1) {
        uint32_t val = HAL_ADC_GetValue(&hadc1);
        Volt = val /4095.0f *8.4f;
         HAL_ADC_Start_IT(&hadc1);
        App_USART2_Printf("%.3f\n",Volt);
    }
}
float App_batGet(){
    return Volt;
}
void App_Bat_Pro(){
    static uint8_t ledstate = 0;
    PERIODIC(20);
    if (Volt <6.0f) 
    {   
        PERIODIC_START(notelectricity,5)
        ledstate =!ledstate;
        PERIODIC_END(notelectricity)
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4|GPIO_PIN_5 | GPIO_PIN_6, ledstate? GPIO_PIN_RESET : GPIO_PIN_SET);
        
        return;
    }else if (Volt < 6.6f) {

        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4 | GPIO_PIN_5|GPIO_PIN_6,GPIO_PIN_RESET);
    
        return;
    } else if (Volt < 7.3f)
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


