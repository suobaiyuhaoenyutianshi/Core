#include "app_encoder.h"
#include "main.h"
#include "delay.h"
#include "stm32_hal_legacy.h"
#include "stm32f1xx_hal_gpio.h"
#include <assert.h>
#include <stdint.h>
void App_Encoder_Init(void){
    Delay_Init();
}
float App_Encoder_GetPos_L(void);
float App_Encoder_GetPos_R(void);
float App_Encoder_GetSpeed_L(void);
float App_Encoder_GetSpeed_R(void);
static volatile uint64_t L_UPTIM = 0;
static volatile uint64_t L_DOWTIM =0;

static volatile uint32_t L_LOWtIM =0;
static volatile uint32_t L_HIGTIM =0;

static volatile uint64_t R_UPTIM = 0;
static volatile uint64_t R_DOWTIM =0;
static volatile uint32_t R_LOWtIM =0;
static volatile uint32_t R_HIGTIM =0;
//UP -DO 低的时间
//do-up  高的时间 脉冲时间
// 编码器占空比校准（如果你后面要做）
void App_Encoder_StartCalibration(void);
int  App_Encoder_EndCalibration(float *duty_l, float *duty_r);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    //左
    if(GPIO_Pin ==GPIO_PIN_14){
        //上升 //UP -DO 低的时间
        if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14) ==GPIO_PIN_SET) {
            L_UPTIM = GetUs();
            L_LOWtIM = L_UPTIM -L_DOWTIM;
            
        }else {
            L_DOWTIM = GetUs();
            L_HIGTIM = L_DOWTIM - L_UPTIM;
        }
        
    }
    //右
    if (GPIO_PIN_3 == GPIO_Pin) {
         if (HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3) ==GPIO_PIN_SET) {
          R_UPTIM = GetUs();
            R_LOWtIM = R_UPTIM -R_DOWTIM;
            
        }else {
            R_DOWTIM = GetUs();
            R_HIGTIM = R_DOWTIM - R_UPTIM;
        }
    }
        
}

   


    uint64_t L_H_time(){
        return L_HIGTIM;
    }
    uint64_t L_L_time(){
        return L_LOWtIM;
    }
    uint64_t R_H_time(){
        return R_HIGTIM;
    }
    uint64_t R_L_time(){
        return R_LOWtIM;
    }

