#include "app_usart2.h"
#include "stm32f1xx_hal_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/_intsup.h>

void App_USART2_Printf(const char * fmt,...){
    char buf[128];
    va_list args;
    va_start(args,fmt);
    vsnprintf(buf,sizeof(buf),fmt,args);
    va_end(args);
//HAL_UART_Transmit(&huart2)
HAL_UART_Transmit(&huart2, (uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);


}