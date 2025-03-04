/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t Receive_Task_Handle = NULL; // 接收任务句柄
static TaskHandle_t Send_Task_Handle = NULL; // 发送任务句柄

/* 内核对象句柄 */
QueueHandle_t Test_Queue = NULL; // 消息队列句柄
/* 全局变量声明 */

/* 宏定义 */
#define QUEUE_LEN 4  // 消息队列长度，最大可包含多少消息
#define QUEUE_SIZE 4 // 队列中每个消息的大小

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void Receive_Task(void* pvParameters); // 接收任务函数
static void Send_Task(void* pvParameters); // 发送任务函数

static void BSP_Init(void); // 板级初始化函数

// 主函数启动流程
/*
    1.BSP初始化
    2.创建APP任务
    3.启动FreeRTOS,开启调度
*/
int main(void)
{
    BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
    BSP_Init(); // 板级初始化
    printf("按下KEY1或者KEY2发送队列消息\r\n");
    printf("Receive Task 接收到消息会提供串口回显\r\n");
    // 创建AppTaskCreate任务   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
        printf("FreeRTOS Start Success\r\n");
    }
    else
    {
        printf("AppTaskCreate 创建失败！\r\n");
        return -1;
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // 进入临界区
    Test_Queue = xQueueCreate((UBaseType_t)QUEUE_LEN, (UBaseType_t)QUEUE_SIZE); // 创建消息队列
    if(Test_Queue != NULL)
    {
        printf("消息队列创建成功！\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Receive_Task, // 任务入口函数
                         (const char*)"Receive_Task", // 任务名字
                         (uint16_t)512, // 任务堆栈大小
                         (void*)NULL, // 任务参数
                         (UBaseType_t)2, // 任务优先级
                         (TaskHandle_t*)&Receive_Task_Handle); // 任务控制块
    if(xReturn == pdPASS)
    {
        printf("接收任务创建成功！\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t )Send_Task, // 任务入口函数
                         (const char*)"Send_Task", // 任务名字
                         (uint16_t)1024, // 任务堆栈大小
                         (void*)NULL, // 任务参数
                         (UBaseType_t)3, // 任务优先级
                         (TaskHandle_t*)&Send_Task_Handle); // 任务控制块
    if(xReturn == pdPASS)
    {
        printf("发送任务创建成功！\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle);
    taskEXIT_CRITICAL(); // 退出临界区
}

// 接收任务函数
static void Receive_Task(void* pvParameters)
{
    BaseType_t xReturn = pdTRUE;
    uint32_t r_queue; // 接收信息临时变量
    while(1)
    {
        xReturn = xQueueReceive(Test_Queue, &r_queue, portMAX_DELAY); // 接收消息
        if(xReturn == pdTRUE)
        {
            printf("接收到消息：%d\r\n", r_queue); // 打印接收到的消息
        }
        else
        {
            printf("接收出错，错误代码：%d\n", xReturn);
        }
    }
}

// 发送任务函数
static void Send_Task(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    uint32_t send_data1 = 1; // 要发送的信息
    uint32_t send_data2 = 2;
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            printf("KEY1按下，发送send_data1");
            xReturn = xQueueSend(Test_Queue, &send_data1, 0); // 发送消息
            if(xReturn == pdPASS)
            {
                printf("send_data1发送成功！\r\n");
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            printf("KEY2按下，发送send_data2");
            xReturn = xQueueSend(Test_Queue, &send_data2, 0); // 发送消息
            if(xReturn == pdPASS)
            {
                printf("send_data2发送成功！\r\n");
            }
        }
        vTaskDelay(20);
    }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
    Key_GPIO_Config();
}
