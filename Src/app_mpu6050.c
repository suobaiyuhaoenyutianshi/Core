#include "app_mpu6050.h"
#include "main.h"
#include "math.h"
#include "qmath.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_i2c.h"
#include <math.h>
#include <stdint.h>
#include "task.h"
/* MPU6050 的 I2C 从机地址：7 位地址 0x68 左移 1 位 = 0xD0 */
#define MPU6050_ADDR  0xD0

/* 寄存器地址定义（只列出我们用到的） */
#define MPU6050_REG_WHO_AM_I   0x75
#define MPU6050_REG_PWR_MGMT_1 0x6B
#define MPU6050_REG_GYRO_CONFIG 0x1B
#define MPU6050_REG_ACCEL_CONFIG 0x1C
#define MPU6050_REG_ACCEL_XOUT_H 0x3B  // 加速度计 X 轴高字节
#define MPU6050_REG_TEMP_OUT_H  0x41  // 温度高字节

/* 全局变量：存储当前传感器数据与欧拉角 */
static float ax, ay, az;        // 加速度，单位 g
static float temperature;       // 温度，单位 ℃
static float gx, gy, gz;        // 角速度，单位 °/s
static float yaw, pitch, roll;  // 欧拉角，单位 °

//内部辅助  向指定寄存器写一个字节 */
static void mpu6050_writer(uint8_t reg,uint8_t val){
     
    HAL_I2C_Mem_Write(&hi2c1,MPU6050_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&val,1,HAL_MAX_DELAY);
}
//内部辅助  向指定寄存器读
static uint8_t mpu6050_read(uint8_t reg){
    uint8_t val;
    HAL_I2C_Mem_Read(&hi2c1,MPU6050_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&val,1,HAL_MAX_DELAY);
    return val;
}

void App_MPU6050_Init(){
     /* 1. 复位设备（写入 0x80 到电源管理寄存器） */
     mpu6050_writer(MPU6050_REG_PWR_MGMT_1,0x80);
    HAL_Delay(100);   // 等待复位完成

    /* 2. 唤醒 MPU6050（写入 0x00，清除休眠位） */
    mpu6050_writer(MPU6050_REG_PWR_MGMT_1,0x00);
    /* 3. 设置陀螺仪满量程为 ±2000°/s（寄存器 0x1B 写入 0x18） */
    mpu6050_writer(MPU6050_REG_GYRO_CONFIG,0x18);
       /* 4. 设置加速度计满量程为 ±2g（寄存器 0x1C 写入 0x00） */
    mpu6050_writer(MPU6050_REG_ACCEL_CONFIG, 0x00);

}
void App_MPU6050_Update();
void App_MPU6050_Proc(){
    PERIODIC(5);
   App_MPU6050_Update();
    //陀螺仪
    float yaw_g = yaw + 0.005*gz;
    float pitch_g = pitch + 0.005*gx;
    float roll_g = roll - 0.005*gy;
    //加速度
    float pitch_a = qatan2(ay,az)/ 3.1415927f * 180.0f;;
    float roll_a = qatan2(ax,az)/ 3.1415927f * 180.0f;
    //互补
      yaw   = yaw_g;
    pitch = 0.95238f * pitch_g + 0.04762f * pitch_a;
    roll  = 0.95238f * roll_g  + 0.04762f * roll_a;

}   // 每 5ms 调用一次，进行姿态更新与融合
/**
 * @brief  从 MPU6050 读取所有原始数据并换算为物理量
 * @note   一次性连续读取 14 字节（从 0x3B 到 0x48），然后拼出 7 个 16 位值。
 *         这样比单独读取每个寄存器效率更高。
 */
void App_MPU6050_Update(){
    uint8_t RAW[14];
    HAL_I2C_Mem_Read(&hi2c1,MPU6050_ADDR,MPU6050_REG_ACCEL_XOUT_H,I2C_MEMADD_SIZE_8BIT,RAW,14,HAL_MAX_DELAY);
    int16_t ax_raw = (int16_t)((RAW[0]<<8) | RAW[1]);
     int16_t ay_raw = (int16_t)((RAW[2]<<8) | RAW[3]);
     int16_t az_raw = (int16_t)((RAW[4]<<8) | RAW[5]);
     int16_t temp_raw = (int16_t)((RAW[6]<<8) | RAW[7]);
     int16_t gx_raw = (int16_t)((RAW[8]<<8) | RAW[9]);
      int16_t gy_raw = (int16_t)((RAW[10]<<8) | RAW[11]);
       int16_t gz_raw = (int16_t)((RAW[12]<<8) | RAW[13]);
    
    /* 换算加速度：±2g 量程，16 位分辨率 → 1 LSB = 2/32768 = 0.000061035 g */
    ax = ax_raw * 6.1035e-5f;
    ay = ay_raw * 6.1035e-5f;
    az = az_raw * 6.1035e-5f;

    /* 换算温度（兼容 MPU6500 的公式，MPU6050 用注释掉的那行） */
    temperature = temp_raw / 333.87f + 21.0f;

    /* 换算角速度：±2000°/s 量程，1 LSB = 2000/32768 = 0.061035 °/s */
    gx = gx_raw * 6.1035e-2f;
    gy = gy_raw * 6.1035e-2f;
    gz = gz_raw * 6.1035e-2f;   

} // 读取传感器原始数据并换算

// 以下 getter 直接返回全局变量，单位见注释
float App_MPU6050_GetAx(){
    return ax;
}
float App_MPU6050_GetAy(){
    return ay;
}
float App_MPU6050_GetAz(){
    return az;
}
float App_MPU6050_GetTemperature(){
    return temperature;
}
float App_MPU6050_GetGx(){
    return gx;
}
float App_MPU6050_GetGy(){
    return gy;
}
float App_MPU6050_GetGz(){
    return gz;
}
float App_MPU6050_GetYaw(){
    return yaw;
}
float App_MPU6050_GetPitch(){
    return pitch;
}
float App_MPU6050_GetRoll(){
    return roll;
}
