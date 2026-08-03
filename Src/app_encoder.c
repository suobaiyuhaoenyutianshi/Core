#include "app_encoder.h"
#include "main.h"
#include "delay.h"
#include "stm32f103xb.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>

// 全局变量（不再加 static）
static volatile uint64_t L_t1 =0;
static volatile uint64_t L_t0 =0;
static volatile uint64_t r_t1 =0;
static volatile uint64_t r_t0 =0;
static volatile int64_t L_encoder = 0;
static volatile int64_t r_encoder = 0;
static volatile int8_t L_statE1 =0;
static volatile int8_t r_statE1 =0;
static volatile int8_t L_statE0 =0;
static volatile int8_t r_statE0 =0;
void App_Encoder_Init(void)
{
    Delay_Init();
}

float App_Encoder_GetPos_L(void){
    return L_encoder* 0.013994022f;
}
float App_Encoder_GetPos_R(void){
    return r_encoder* 0.013994022f;
}
float App_Encoder_GetSpeed_L(void){
     __disable_irq();
    uint64_t t1 = L_t1;
    uint64_t t0 = L_t0;
    uint64_t T =0;
     __enable_irq();
     if (L_statE1*L_statE0 <0) {
     return 0.0f;
     }
    uint64_t NoW = GetUs();
    if ((NoW-t1) >(t1-t0)) {
        T = (float)(NoW - t1) * 1.0e-6f;
    }else {
        T= (float)(t1-t0) *1.0e-6f;
    }
    return 1/T* 0.013994022f;



}
float App_Encoder_GetSpeed_R(void){

}
//规定左的顺时 为正转  哪么右的逆时 为正 ++
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{   uint64_t now = GetUs();
    //左
   if (GPIO_Pin == GPIO_PIN_14) {
    L_statE0 =L_statE1;
    L_t0 = L_t1; L_t1 = now;
        int8_t L_A = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
        int8_t L_B = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
        if ((L_A==0&& L_B==0) || (L_A&&L_B)) {
            L_encoder--;
            L_statE1 = -1;
        }else if ((L_A==1&& L_B==0) || (L_A==0&&L_B==1)) {
            L_encoder++;
            L_statE1 =1;
        }
   }else if (GPIO_Pin == GPIO_PIN_3) {
        r_statE0 = r_statE1;
    r_t0 = r_t1; r_t1 = now;
         int8_t R_A = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
        int8_t R_B = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
        if ((R_A==0&& R_B==0) || (R_A&&R_B)) {
            r_encoder++;
            r_statE1 = 1;
        }else if ((R_A==1&& R_B==0) || (R_A==0&&R_B==1)) {
            r_encoder--;
            r_statE1=-1;
        }
   }


}