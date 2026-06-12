#ifndef __OPTS_H__
#define __OPTS_H__

/* IAP opts -------------------------------------------------------------------------------- */
/* If use this feature, In option [target] set IROM1 Start Address 0x8008000, Size 0x19000 */
#ifndef APP_IAP
#define APP_IAP                 0
#endif


/* Print log opts -------------------------------------------------------------------------- */
#ifndef PRINT_LOG_OPEN
#define PRINT_LOG_OPEN          1 // 1：打开print log功能（只有此为1，才能打印log）
#endif

#ifndef UART1_AS_LOG
#define UART1_AS_LOG            1 // 1: 通过UART1打印log, PA9,PA10
#endif

#ifndef UART2_AS_LOG
#define UART2_AS_LOG            0 // 1: 通过UART2打印log, PA2,PA3
#endif

#ifndef UART3_AS_LOG
#define UART3_AS_LOG            0 // 1: 通过UART3打印log
#endif




#endif /* __OPTS_H__ */ 
