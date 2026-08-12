#include "button.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_gpio.h"
#include <stdint.h>
void My_Button_Init(button_TypeDef * Bin,Button_InitTypeDef* inIt){
    Bin->GPIOx = inIt->GPIOx;
    Bin->GPIO_Pin = inIt->GPIO_Pin;
    Bin->clicks = 0;
    Bin->LastTick = 0;
    Bin->state = 0;
    Bin->clickcb =NULL;
    Bin->longPressCb =NULL;
}//
void My_Button_SetClickCb(button_TypeDef *btn, Button_clickback cb){
    btn->clickcb =cb;
}
void My_Button_SetLongPressCb(button_TypeDef *btn, Button_longProssCallback cb){
    btn->longPressCb = cb;
}
void My_Button_Proc(button_TypeDef *btn){
//按下低  为1
    uint8_t press = (HAL_GPIO_ReadPin(btn->GPIOx,btn->GPIO_Pin) == GPIO_PIN_RESET);
    uint32_t Now = HAL_GetTick();
    switch (btn->state) {
        case 0:
            if (press ==1) {
                btn->state = 1;
                btn->LastTick = Now;
            }
            break;
    //消抖
        case 1:
            if ((HAL_GetTick() - btn->LastTick) >=20) {
                //还在按 消抖
                if (press) {
                    btn->state =2;
                    btn->LastTick = Now;
                }
                else {
                //否则 在20ms内松开  说明是抖动  无效
                btn->state = 0;
                }
            }
            break;
        case 2:
            //松开了 加1
            if (!press) {
                btn->clicks++;
                btn->state = 3;
                btn->LastTick = Now;
            }
            //一直不松开长按 
            else if ((Now - btn->LastTick) >=1000) {
                if(btn->longPressCb) btn->longPressCb(1);
                btn->state =4;

            }
            break;
        case 3:
            //等2击
            if(press){
                btn->state =2;
                btn->LastTick = Now;
            }else if (Now - btn->LastTick >= 300){
                if (btn->clickcb) btn->clickcb(btn->clicks);
                btn->state = 0;
                btn->clicks = 0;
            }
            break;

            case 4:  // 长按后等松开
        if (!press)
        {
            btn->state = 0;
            btn->clicks = 0;
        }
        break;
    }





    
}






