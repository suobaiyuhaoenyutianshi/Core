#include "app_encoder.h"
#include "delay.h"
#include "math.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_gpio.h"
#include <math.h>
#include <stdint.h>

#define DUTY_L  0.500f
#define DUTY_R  0.490f

static volatile int64_t  encoder_l = 0;
static volatile int64_t  encoder_r = 0;

static volatile int8_t   dir_l = 0;
static volatile int8_t   last_dir_l = 0;
static volatile uint8_t  phase_l = 0;

static volatile int8_t   dir_r = 0;
static volatile int8_t   last_dir_r = 0;
static volatile uint8_t  phase_r = 0;

static volatile uint64_t t0_l = 0, t1_l = 0;
static volatile uint64_t t0_r = 0, t1_r = 0;

static float m_l[2] = { DUTY_L * 2.0f, 2.0f - DUTY_L * 2.0f };
static float m_r[2] = { DUTY_R * 2.0f, 2.0f - DUTY_R * 2.0f };

#define ENC_L_A_PIN        GPIO_PIN_14
#define ENC_L_B_PIN        GPIO_PIN_15
#define ENC_L_GPIO_PORT    GPIOB
#define ENC_L_EXTI_LINE    EXTI_LINE_14

#define ENC_R_A_PIN        GPIO_PIN_3
#define ENC_R_B_PIN        GPIO_PIN_4
#define ENC_R_GPIO_PORT    GPIOB
#define ENC_R_EXTI_LINE    EXTI_LINE_3




void App_Encoder_Init(void)
{
    Delay_Init();
}

float App_Encoder_GetPos_L(void)
{
    return encoder_l * 0.01399402208920360588844895090594f;
}

float App_Encoder_GetPos_R(void)
{
    return -encoder_r * 0.01399402208920360588844895090594f;
}

float App_Encoder_GetSpeed_L(void)
{
    __disable_irq();
    int8_t dir = dir_l;
    int8_t last = last_dir_l;
    uint8_t phase = phase_l;
    uint64_t t0 = t0_l;
    uint64_t t1 = t1_l;
    __enable_irq();
    uint64_t now = GetUs();
    if(dir ==0 || last ==0)return  0.00f;
    if (dir!= last) {
        return 0.00f;
    }
    uint8_t next_ph =phase ^1;
    float mo = dir * m_l[phase];
    float mNow = dir * m_l[next_ph];
    float M,T;
    if (fabs(mo) * (t0 - t1) >fabs(mNow)*(now - t0)) {
        M = mNow;
        T = (now - t0)*1.0e-6f;
    }else {
        M = mo;
        T = (t0 - t1) * 1.0e-6f;
    }
     return M / T * 0.01399402208920360588844895090594f;

    
}

float App_Encoder_GetSpeed_R(void)
{
    __disable_irq();
    int8_t   dir  = dir_r;
    int8_t   last = last_dir_r;
    uint8_t  ph   = phase_r;
    uint64_t t0   = t0_r;
    uint64_t t1   = t1_r;
    __enable_irq();

    if (dir == 0 || last == 0) return 0.0f;
    if (dir != last) return 0.0f;

    uint64_t now = GetUs();
    uint8_t next_ph = ph ^ 1;

    float M0   = dir * m_r[ph];
    float Mnow = dir * m_r[next_ph];

    float M, T;
    if (fabsf(Mnow) * (t0 - t1) < fabsf(M0) * (now - t0))
    {
        M = Mnow;
        T = (now - t0) * 1.0e-6f;
    }
    else
    {
        M = M0;
        T = (t0 - t1) * 1.0e-6f;
    }

    return -M / T * 0.01399402208920360588844895090594f;
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
   if (GPIO_Pin ==GPIO_PIN_14) {
        last_dir_l = dir_l;
        t1_l =t0_l;
        t1_l = GetUs();
        uint8_t A = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_14);
        uint8_t B = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_15);
        if (A) {
            phase_l =1;
            if (B) {
                dir_l = 1;  encoder_l++; 
            }else {
                dir_l = -1;  encoder_l--; 
            }    
        }else {
             phase_l = 0;
            if (B )  { dir_l = 1;  encoder_l++; }
            else                { dir_l = -1; encoder_l--; }
            
        }



   }else if (GPIO_Pin == GPIO_PIN_3) {
      last_dir_r = dir_r;
        t1_r =t0_r;
        t1_r = GetUs();
        uint8_t A = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_3);
        uint8_t B = HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_4);
        if (A) {
            phase_r =1;
            if (B) {
                dir_r = 1;  encoder_r++; 
            }else {
                dir_r = -1;  encoder_r--; 
            }    
        }else {
             phase_r = 0;
            if (B )  { dir_r = 1;  encoder_l++; }
            else                { dir_r = -1; encoder_r--; }
            
        }
   }
}

void App_Encoder_StartCalibration(void) { }
int App_Encoder_EndCalibration(float *duty_l, float *duty_r)
{
    *duty_l = DUTY_L;
    *duty_r = DUTY_R;
    return 0;
}