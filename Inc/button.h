#ifndef BUTTON_H
#define BUTTON_H
#include "main.h"
#include "stm32f103xb.h"
#include <stdint.h>
//点击的函数类型 快速
typedef void (*Button_clickback)(uint8_t clicks);
//长按的函数
typedef void (*Button_longProssCallback)(uint8_t clicks);
typedef struct {
    GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;
} Button_InitTypeDef;

typedef struct{
   GPIO_TypeDef *GPIOx;
    uint16_t      GPIO_Pin;
    uint8_t state;//状态
    uint32_t LastTick;//时间搓
    uint8_t clicks;//点击数
    Button_clickback clickcb;//短暂点击的回调函数
    Button_longProssCallback longPressCb;
} button_TypeDef;

void My_Button_Init(button_TypeDef * Bin,Button_InitTypeDef* inIt);
void My_Button_SetClickCb(button_TypeDef *btn, Button_clickback cb);
void My_Button_SetLongPressCb(button_TypeDef *btn, Button_longProssCallback cb);
void My_Button_Proc(button_TypeDef *btn);

#endif