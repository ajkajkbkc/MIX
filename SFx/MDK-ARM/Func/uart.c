#include "uart.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"

#include "log.h"
#include "string.h"
/* Private variables ---------------------------------------------------------*/
/* Definitions for defaultTask */
osThreadId_t uartTaskHandle;

/* UART端口状态数组定义（对应 uart.h 中的 extern 声明） */
volatile uart_status_info_st gtv_UartPortStatus[MAX_UART_PORT];
const osThreadAttr_t uartTask_attributes = {
  .name = "UartTask",
  .stack_size = 2048,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Private define ------------------------------------------------------------*/
#if PRINT_LOG_OPEN == 1
static const char *TAG = "UART";
#endif

#define UART1_RX_TIMER_ID   1

//队列句柄
QueueHandle_t gtv_UartTaskMsgQueueHandle;

/*定时器句柄*/
TimerHandle_t ltv_Uart1RxTime;

//串口结构体
extern UART_HandleTypeDef huart1;

uint8_t gcv_Uart1RecvBuf[RX_LEN_UART1];

/* Private function prototypes -----------------------------------------------*/
void UartTask(void *argument);


/* Private user code ---------------------------------------------------------*/
//定时器回掉函数
static void uart_timer_callback_func(TimerHandle_t ltv_TimeHandle)
{
    unsigned int liv_TimerId;
    uart_msg_st ltv_UartMsg;
    BaseType_t ltv_Err;

    liv_TimerId = (unsigned int)pvTimerGetTimerID(ltv_TimeHandle);
    LOGV(TAG, "Enter %s, liv_TimerId = %d\r\n", __func__, liv_TimerId);

    if(liv_TimerId == UART1_RX_TIMER_ID)
    {
        if(gtv_UartPortStatus[UART_PORT1].mcv_Status == UART_STATE_RX)
        {
            gtv_UartPortStatus[UART_PORT1].mcv_Status = UART_STATE_IDLE;
            
            ltv_UartMsg.mcp_DataBuff = gcv_Uart1RecvBuf;
            ltv_UartMsg.msv_MsgLength = gtv_UartPortStatus[UART_PORT1].msv_RxCount;
            ltv_UartMsg.mcv_UartPort = UART_PORT1;
            if(ltv_UartMsg.msv_MsgLength != 0)
            {
                ltv_Err = xQueueSend(gtv_UartTaskMsgQueueHandle, &ltv_UartMsg, 10);
                if(ltv_Err != pdPASS)
                {
                    /*错误处理*/
                    LOGE(TAG, "xQueueSend to gtv_UartTaskMsgQueueHandle ERROR!\r\n");
                }
                else
                {
                    LOGI(TAG, "xQueueSend : msv_MsgLength = %u", ltv_UartMsg.msv_MsgLength);
                }
            }
        }
    }
}
//串口1定时器
static void uart1_time_init(unsigned short lsv_RxPeriod)
{
    LOGV(TAG, "Enter %s, lsv_RxPeriod = %d\r\n", __func__, lsv_RxPeriod);
    if (lsv_RxPeriod == 0)
    {
        lsv_RxPeriod = 100;
    }
    if((ltv_Uart1RxTime == NULL) && (lsv_RxPeriod > 0))
    {
        ltv_Uart1RxTime = xTimerCreate((const char *)"Uart1 RxTime",
                                       (TickType_t  )lsv_RxPeriod / portTICK_PERIOD_MS,
                                       (UBaseType_t )pdFALSE,
                                       (void *      )UART1_RX_TIMER_ID,
                                       (TimerCallbackFunction_t)uart_timer_callback_func);
    }
}
//串口1初始化
void bsp_uart1_init(unsigned long llv_BaudRate, unsigned char lcv_Parity, unsigned char lcv_WordLength, unsigned char lcv_StopBits)
{
    LOGV(TAG, "Enter %s, llv_BaudRate=%ld, lcv_Parity=%d, lcv_WordLength=%d, lcv_StopBits=%d.\r\n", __func__, llv_BaudRate, lcv_Parity, lcv_WordLength, lcv_StopBits);

    huart1.Instance = USART1;
    huart1.Init.BaudRate = llv_BaudRate;
    huart1.Init.WordLength = lcv_WordLength;
    huart1.Init.StopBits = lcv_StopBits;
    huart1.Init.Parity = lcv_Parity;
    huart1.Init.Mode = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart1) != HAL_OK)
    {
        Error_Handler();
    }

    gtv_UartPortStatus[UART_PORT1].mcv_Status = UART_STATE_IDLE;
    gtv_UartPortStatus[UART_PORT1].mcv_Mode = UART_MODE_DEFAULT;
    uart1_time_init(WORD_TIMEOUT);

    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE); //使能 USART1 的接收数据寄存器非空（RXNE）中断。
    HAL_NVIC_EnableIRQ(USART1_IRQn);  //使能串口中断
}

// 串口1接收数据处理函数
static void HandleUART1RecvData(unsigned char *lcp_Buff, unsigned short lsv_Length)
{   
    if(gtv_UartPortStatus[UART_PORT1].mcv_Mode == UART_MODE_DEFAULT)
    {
#if (APP_IAP == 1)
        if(is_iap_update(lcp_Buff, lsv_Length))
        {
            HAL_NVIC_SystemReset();
        }
#endif

        // if(is_mb_protocol(lcp_Buff, lsv_Length))
        // {
        //     md_slave_msg_pack smsg = {0,};

        //     smsg.mcv_IsBroadcastInfo = lcp_Buff[0] == 0 ? 1 : 0;
        //     smsg.mcp_ReceiveBuff = lcp_Buff;
        //     smsg.msv_ReceiveLen = lsv_Length;
        //     smsg.mcv_Sender = MB_SENDER_UART1;
        //     smsg.uart_resp_func = bsp_uart1_send_buffer;
        //     smsg.mcp_RespBuff = (unsigned char *)pvPortMalloc(1024);
        //     LOGI(TAG, "smsg.mcp_RespBuff = 0x%08X, Free Heap Size = %d", (uint32_t)smsg.mcp_RespBuff, xPortGetFreeHeapSize());
        //     mb_slave_msg_handler(&smsg);
        //     vPortFree(smsg.mcp_RespBuff);
        // }
    }
    else if(gtv_UartPortStatus[UART_PORT1].mcv_Mode == UART_MODE_MASTER)
    {                   
        for(int a=0; a < lsv_Length; a++)
        {
            if (gtv_UartPortStatus[1].aFlag == 0)
            {
                gtv_UartPortStatus[1].msp_FreeRxBuff[a] = lcp_Buff[a];
            }   
            else
            {
                gtv_UartPortStatus[1].aBuf[a] = lcp_Buff[a];
            }
            
        }    
        // modlink_copy(1);
        //gtv_UartPortStatus[UART_PORT1].mcv_Status = UART_STATE_IDLE;
    }
}

void UART_Init(void)
{
    bsp_uart1_init(9600, UART_PARITY_NONE, UART_WORDLENGTH_8B, UART_STOPBITS_1);
}

void osThreadNew_uartTask(void)
{
    uartTaskHandle = osThreadNew(UartTask, NULL, &uartTask_attributes);
}

void UartTask(void *argument)
{
    uart_msg_st ltv_UartMsg;
    /*创建消息队列*/
    gtv_UartTaskMsgQueueHandle = xQueueCreate(10, sizeof(uart_msg_st));

    for(;;)
    {
        // 1. 执行所有主要的功能逻辑
        xQueueReceive(gtv_UartTaskMsgQueueHandle, &ltv_UartMsg, portMAX_DELAY);

        switch(ltv_UartMsg.mcv_UartPort)
        {
        case UART_PORT1:
            HandleUART1RecvData(ltv_UartMsg.mcp_DataBuff, ltv_UartMsg.msv_MsgLength);
            memset(gcv_Uart1RecvBuf, 0, RX_LEN_UART1);
            break;

        default:
            break;
        }
        // 2. 逻辑执行完后，在进入阻塞前调用
        //    此时栈使用量通常达到峰值，测量结果最准确[reference:4]
        uint32_t stack_space = osThreadGetStackSpace(uartTaskHandle);
        // 3. 打印或通过其他方式记录，单位是字节[reference:5]
        LOGD(TAG, "%s RUN. UartTask minimum free stack space: %lu bytes\n", __func__, stack_space);
    }
}
