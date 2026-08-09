#ifndef APP_MPU6050_H
#define APP_MPU6050_H
#include "main.h"


void App_MPU6050_Init(void);
void App_MPU6050_Proc(void);   // 每 5ms 调用一次，进行姿态更新与融合

void App_MPU6050_Update(void); // 读取传感器原始数据并换算

// 以下 getter 直接返回全局变量，单位见注释
float App_MPU6050_GetAx(void);
float App_MPU6050_GetAy(void);
float App_MPU6050_GetAz(void);
float App_MPU6050_GetTemperature(void);
float App_MPU6050_GetGx(void);
float App_MPU6050_GetGy(void);
float App_MPU6050_GetGz(void);
float App_MPU6050_GetYaw(void);
float App_MPU6050_GetPitch(void);
float App_MPU6050_GetRoll(void);


#endif