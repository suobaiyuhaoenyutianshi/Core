#include "app_control.h"
#include "pid.h"
#include "app_mpu6050.h"
#include "qmath.h"
#include "app_motor.h"
#include "app_encoder.h"
#include "delay.h"
#include "task.h"
#include <stdint.h>
static const float g  = 9.8f;       // 重力加速度 m/s²
static const float lp = 0.062f;     // 质心到轮轴距离 m
static const float rw = 0.032f;     // 车轮半径 m

static volatile uint64_t lasttime =0;
static float omega_ref = 0.0f;      // 积分得到的目标轮速 (rad/s)
//PID 控制器定义
static PID_TypeDef pid_vel ;//速度环
static PID_TypeDef pid_theta;//角度环
static PID_TypeDef pid_theta_dot;//角速度环
static PID_TypeDef pid_turn;



void App_Control_Init(void){
    PID_InitTypeDef init;
    //速度环 PI 控制器
    init.Kp = 10.0f;init.Ki =1.0f;init.Kd = 0.0f;
    init.Setpoint =0.0f;
   init.Setpoint = 0.0f;                  // 初始目标速度 0 m/s
    init.OutputUpperLimit = +0.5f * g;     // 加速度上限 +0.5g ≈ +4.9 m/s²
    init.OutputLowerLimit = -0.5f * g;     // 加速度下限 -0.5g
    init.DefaultOutput = 0.0f;             // 默认输出（积分初值）
    PID_Init(&pid_vel, &init);
    //角速度环
    init.Kp = 4.0f;init.Ki = 0.0f;init.Kd = 0.0f;
    init.OutputUpperLimit = +12.57f;       // 角速度上限 ±12.57 rad/s (≈720°/s)
    init.OutputLowerLimit = -12.57f;
    init.DefaultOutput = 0.0f;
    init.Setpoint = 0.0f;                  // 初始目标倾角 0 rad
    PID_Init(&pid_theta, &init);
    //角加速度环
    init.Kp =10.0f;init.Ki = 10.0f;init.Kd = 0.0f;
    init.Setpoint = 0.0f;                  // 初始目标角速度 0 rad/s
    init.OutputUpperLimit = +125.7f;       // 角加速度上限 ±125.7 rad/s²
    init.OutputLowerLimit = -125.7f;
    init.DefaultOutput = 0.0f;
    PID_Init(&pid_theta_dot, &init);
    init.Kp = 1.0f;init.Ki =0.0f;init.Kd =0;init.Setpoint = 0.0f;init.DefaultOutput =0.0f;init.OutputUpperLimit = 15.0f;init.OutputLowerLimit =-15.0f;
    PID_Init(&pid_turn,&init);
}
/**
 * @brief  平衡车控制主进程（串级 PID 核心）
 * @note   内部 PERIODIC(5) 保证每 5ms 执行一次。
 *         调用顺序：速度环 → 加速度转倾角 → 角度环 → 角速度环 → 逆解算 → 积分 → 电机命令
 */
void App_Control_Proc(void){
    PERIODIC(5);
    uint64_t now = GetUs();
    float deltaT = (now - lasttime)*1.0e-6f;//真正的时间间隔
    float speed = (App_Encoder_GetSpeed_R()+App_Encoder_GetSpeed_L())/2;
    float theta     = App_MPU6050_GetPitch() * 0.0174533f;  // 倾角 ° →rad
    float theta_dot = App_MPU6050_GetGx()*0.0174533f;// 角速度 °/s → rad/s
// ===== 3. 融合编码器与陀螺仪，计算真实水平速度 x_dot (m/s) =====
    // 车轮转动速度包含两部分：车身转动导致轮子跟着转 + 真正水平移动
    // 车身以 theta_dot 角速度旋转时，轮子中心线速度 = theta_dot * (lp + rw)
    // 换算成轮子角速度 = theta_dot * (lp + rw) / rw，方向相反
//     后撤让编码器读数变负（假速度）。

// 前倾让陀螺仪读数变正。

// 合成公式（编码器 + 陀螺仪补偿）把假速度抵消，得出真速度 = 0。

// 控制器看到真速度是0，目标速度是正，误差很大，于是坚定地继续命令前倾。

// 重力开始真正把车往前推，真速度开始变正，系统进入稳定加速
    float omeg2 = -(lp + rw)* theta_dot / rw;// 由车身旋转引起的等效轮速
                
    float omeg1 = speed - omeg2;// 纯水平移动的轮速
     float x_dot  = omeg1 * rw;                   // 水平速度 (m/s)
     // ===== 4. 速度环 PID：速度误差 → 所需加速度 =====
    // PID 输出直接代表目标水平加速度 accel_ref (m/s²)
    // 物理原理：Kp*error 产生与误差成正比的加速度，积分项消除稳态误差
    float acc_ref = PID_Compute1(&pid_vel,x_dot,now);
    // 加速度 → 目标倾角：重力分量提供加速度，a = g*tanθ → θ = atan(a/g)
    float the_ref = qatan(acc_ref/g);
    // ===== 5. 角度环 PID：倾角误差 → 目标角速度 =====
    PID_ChangeSetpoint(&pid_theta,the_ref);
    float the_dot_tef = PID_Compute1(&pid_theta,theta,now);
    // ===== 6. 角速度环 PID：角速度误差 → 目标角加速度 =====
    PID_ChangeSetpoint(&pid_theta_dot,the_dot_tef);
    float the_dot_dot_ref = PID_Compute1(&pid_theta_dot,theta_dot,now);
    // ===== 7. 逆解算：角加速度 → 车轮线加速度 =====
    // 根据倒立摆动力学：车轮加速度 x_dot_dot = (g*sinθ - θ_dot_dot * lp) / cosθ
    float x_dot_dot_ref = (g * qsin(theta) - the_dot_dot_ref * lp) / qcos(theta);
    if (lasttime!=0) {
        omega_ref += (1.0f / rw) * x_dot_dot_ref * deltaT;
    }
    float gz = App_MPU6050_GetGz()*0.0174533f;
    float om_dif    =  PID_Compute1(&pid_turn,gz,now) ;
     // ===== 9. 将目标轮速发给电机速度环 =====
    App_Motor_SetOmega_L(omega_ref-om_dif);
    App_Motor_SetOmega_R(omega_ref+om_dif);

    lasttime = now; // 保存本次时间戳

}
/**
 * @brief  复位控制系统（清除所有积分和状态）
 */
void App_Control_Reset(void)
{
    lasttime  = 0;
    omega_ref = 0.0f;

    PID_Reset(&pid_vel);
    PID_Reset(&pid_theta);
    PID_Reset(&pid_theta_dot);
}

void App_Contro_setMoveSpeed(float speed){
    PID_ChangeSetpoint(&pid_vel,speed);
}
void App_Contro_setTurnSpeed(float speed){
    PID_ChangeSetpoint(&pid_turn,speed);
}