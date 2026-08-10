#include "app_motor.h"
#include "delay.h"
#include "task.h"
#include "main.h"
#include "pid.h"
#include "app_encoder.h"
#include "app_pwm.h"
#include "app_bat.h"
#include <stdint.h>
static PID_TypeDef pid_mot_l;
static PID_TypeDef pid_mot_r;


void App_Motor_Init(){
    //初始
    PID_InitTypeDef init;
    init.Kp = 0.5f;
    init.Ki = 7.0f;
    init.Kp=0.0f;
    init.Setpoint =0.0f;// 初始目标轮速 0 rad/s
    init.OutputUpperLimit = +8.4f;
    init.OutputLowerLimit = -8.4f;
    init.DefaultOutput = 0.0f;
    PID_Init(&pid_mot_l,&init);
    PID_Init(&pid_mot_r,&init);

}

void App_Motor_SetOmega_L(float Omega){
    PID_ChangeSetpoint(&pid_mot_l,Omega);
}
void App_Motor_SetOmega_R(float Omega){
     PID_ChangeSetpoint(&pid_mot_r,Omega);
}
/**
 * @brief  电机使能/关闭
 * @note   关闭时复位 PID，防止下次启动飞车
 */
void App_Motor_Cmd(uint8_t On){
    App_PWM_Cmd(On);
    if (On ==0) {
        PID_Reset(&pid_mot_l);
        PID_Reset(&pid_mot_r);
    }
}
void App_Motor_Proc(void){
    PERIODIC(1);
    uint64_t now = GetUs();
    //当前转速
    float speed_l = App_Encoder_GetSpeed_L();
    float speed_r = App_Encoder_GetSpeed_R();
    //计算误差 需要的电压
    float vol_l= PID_Compute1(&pid_mot_l,speed_l,now);
    float vol_r = PID_Compute1(&pid_mot_r,speed_r,now);

    float totalBatVol = App_batGet();
    float duty_l = (totalBatVol> 0.1)?(vol_l/totalBatVol):0.0f;
    float duty_r = (totalBatVol> 0.1)?(vol_r/totalBatVol):0.0f;
    App_PWM_Set_L(duty_l);
    App_PWM_Set_R(duty_r);

}