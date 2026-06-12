#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include "stm32f1xx_hal_uart.h"

#define WORD_TIMEOUT      10

#define RX_LEN_UART1      1024

/*UART状态定义*/
enum __UART_STATUS_E
{
    /*端口空闲*/
    UART_STATE_IDLE = 0x01,
    /*接收*/
    UART_STATE_RX = 0x02,
    /*发送*/
    UART_STATE_TX = 0x04,
};

/*UART端口定义*/
enum __UART_PORT_E
{
    /* 串口1 */
    UART_PORT1 = 0,
    /* 串口2 */
    UART_PORT2 = 1,
    /* 串口3 */
    UART_PORT3 = 2,
    /*  */
    MAX_UART_PORT,
};

/*UART工作模式定义*/
enum __UART_MODE_E
{
    /*默认*/
    UART_MODE_DEFAULT,
    /*从模式*/
    UART_MODE_SALVE,
    /*主模式*/
    UART_MODE_MASTER,
};

/*uart 缓冲区定义*/
typedef struct __BSP_UART_STATUS_INFO_ST
{
    /*串口模式：*/
    unsigned char mcv_Mode;
    /*当前端口状态*/
    unsigned char mcv_Status;
    /*接收数据长度计数*/
    unsigned short msv_RxCount;
    /*接收数据长度*/
    unsigned short msv_RxLength;
    /*待发送数据长度*/
    unsigned short msv_TxTotalLength;
    /*已发送数据长度*/
    unsigned short msv_TxLength;
    /*接收缓存区*/
    unsigned char *mcp_RxBuff;
    /*发送缓存区*/
    unsigned char *mcp_TxBuff;
    /*自由口、Modbus master接收缓冲区地址*/
    unsigned short *msp_FreeRxBuff;
    /*自由口、Modbus master接收最大长度*/
    unsigned short msv_FreeRxMaxCnt;
    /*Modbus master重发次数*/
    unsigned char mcv_RetryCnt;

    unsigned char aFlag; // 0, modbus; 1, modlink
    unsigned char *aBuf;
} uart_status_info_st;


/* uart任务消息队列结构体 */
typedef struct __UART_MSG_ST
{
    /*设备UART端口号*/
    unsigned char mcv_UartPort;
    /*数据长度*/
    unsigned short msv_MsgLength;
    /*数据缓存区指针*/
    unsigned char *mcp_DataBuff;
} uart_msg_st;

extern volatile uart_status_info_st gtv_UartPortStatus[MAX_UART_PORT]; 

void osThreadNew_uartTask(void);

void UART_Init(void);
void uartReceive_IDLE_FromISR(UART_HandleTypeDef *huart);

void uart1_send_buffer(unsigned char *lcp_SendBuff, unsigned short lsv_Length);
void uart2_send_buffer(unsigned char *lcp_SendBuff, unsigned short lsv_Length);
void uart3_send_buffer(unsigned char *lcp_SendBuff, unsigned short lsv_Length);

void uart_addCRC_send_buffer(UART_HandleTypeDef *huart, unsigned char *pSendBuf, unsigned short len);

#endif /* __UART_H__ */
