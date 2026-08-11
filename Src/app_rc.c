#include "app_rc.h"
#include "main.h"
#include "stm32f1xx_hal_uart.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>
#include "app_control.h"
#define MAX_leng 64
static volatile char directIntBuf[MAX_leng];
static volatile char tranBuf[MAX_leng];//中转站
static volatile char proBuf[MAX_leng];//目的
/* 用于 HAL_UART_Receive_IT 的接收字节变量 */
static uint8_t rcRxByte;

static volatile uint8_t lineReceivedFlag = 0;
static uint16_t intBufCursor = 0;

static void str_to_lower(char *str) {
    while (*str) {
        if (*str >= 'A' && *str <= 'Z') {
            *str += 32;  // ASCII 中 'a' - 'A' = 32
        }
        str++;
    }
}


void App_RC_Init(void){
    HAL_UART_Receive_IT(&huart3,&rcRxByte,1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart == &huart3) {
        uint8_t DATA = rcRxByte;
        if (DATA !='\n') {
            if (intBufCursor < (MAX_leng-1)) {
                directIntBuf[intBufCursor] = DATA;
            }else {
                intBufCursor = 0;
            }
            

        }else {
            directIntBuf[intBufCursor] = '\0';
            intBufCursor = 0;

            /* 转移到转运缓冲区，并通知主循环 */
           strcpy(tranBuf, directIntBuf);
            lineReceivedFlag = 1;

        }


    HAL_UART_Receive_IT(&huart3,&rcRxByte,1);

    }
}


void App_RC_Proc(void){
    if (lineReceivedFlag) {
        lineReceivedFlag =0;
        strcpy(proBuf,tranBuf);
        str_to_lower(proBuf);
        if(strncmp(proBuf, "move ", 5)==0) {
            int turnSpeed, moveSpeed;
            if (sscanf(proBuf, "move %d %d", &turnSpeed, &moveSpeed) == 2) {
                App_Contro_setMoveSpeed(-moveSpeed *0.01f*0.7f);
                App_Contro_setTurnSpeed(-turnSpeed*0.01f*15.0f);
            }
        }






    }



}

