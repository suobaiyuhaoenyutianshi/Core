#include "mputest.h"
#include "app_usart2.h"

#include "app_mpu6050.h"
#include "app_encoder.h"
#include <stdint.h>
void mputest(uint64_t* a){
    while (1) {
        (*a)++;
        App_MPU6050_Proc();
    App_USART2_Printf("%.2f,%.2f,%.2f\n",App_Encoder_GetSpeed_R(),App_MPU6050_GetPitch(),App_MPU6050_GetGx());
    }
    App_MPU6050_Proc();
    App_USART2_Printf("%.2f,%.2f,%.2f\n",App_Encoder_GetSpeed_R(),App_MPU6050_GetPitch(),App_MPU6050_GetGx());
}