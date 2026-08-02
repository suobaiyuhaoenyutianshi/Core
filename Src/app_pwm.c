#include "app_pwm.h"
#include "main.h"
#include "math.h"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_tim.h"
#define PWM_ARR 99  // 和 CubeMX 里 TIM1/TIM4 的 ARR 一致





void App_PWM_Init(void){
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);



}
void App_PWM_Cmd(uint8_t State){
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,State?GPIO_PIN_SET:GPIO_PIN_RESET);

}       // 使能/休眠 TB6612
void App_PWM_Set_L(float Duty){
    if (Duty > 100.0f)  Duty = 100.0f;
    if (Duty < -100.0f) Duty = -100.0f;
    //正转
    if(Duty>=0){
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_SET);// AIN2
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);// AIN1
    }else {
    //反转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_9,GPIO_PIN_RESET);// AIN2
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);// AIN1
    }
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,(uint16_t)(fabsf(Duty) / 100.0f * (PWM_ARR + 1)));



}          // 左电机占空比 -100~+100
void App_PWM_Set_R(float Duty){
     if (Duty > 100.0f)  Duty = 100.0f;
    if (Duty < -100.0f) Duty = -100.0f;
    //正转
    if(Duty>=0){
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);// BIN1
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);// BIN1
    }else {
    //反转
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);// 
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);// 
    }
    __HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,(uint16_t)(fabsf(Duty) / 100.0f * (PWM_ARR + 1)));





}          // 右电机占空比 -100~+100

