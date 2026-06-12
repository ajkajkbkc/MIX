#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>
#include "opts.h"

#if PRINT_LOG_OPEN == 1
    #if UART1_AS_LOG
        #define LOGV(tag, fmt, ...) printf("[V][%s] " fmt, tag, ##__VA_ARGS__) //最详细的调试信息
    #else
        #define LOGV(tag, fmt, ...)
    #endif

    #if UART1_AS_LOG
        #define LOGD(tag, fmt, ...) printf("[D][%s] " fmt, tag, ##__VA_ARGS__) //调试信息
    #else
        #define LOGD(tag, fmt, ...)
    #endif

    #if UART1_AS_LOG
        #define LOGI(tag, fmt, ...) printf("[I][%s] " fmt, tag, ##__VA_ARGS__) //常规运行信息
    #else
        #define LOGI(tag, fmt, ...)
    #endif

    #if UART1_AS_LOG
        #define LOGW(tag, fmt, ...) printf("[W][%s] " fmt, tag, ##__VA_ARGS__) //警告信息
    #else
        #define LOGW(tag, fmt, ...)
    #endif

    #if UART1_AS_LOG
        #define LOGE(tag, fmt, ...) printf("[E][%s] " fmt, tag, ##__VA_ARGS__) //错误信息
    #else
        #define LOGE(tag, fmt, ...)
    #endif
#endif

#endif /* __LOG_H__ */
