#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄
static TaskHandle_t LED_Task_Handler = NULL; // LED任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数
static void LED_Task(void* pvParameters); // LED任务函数
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
    printf("FreeRTOS SRAM Dynamic Create Single Task Example\r\n");
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
        return -1;
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // 进入临界区
    xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数
                         (const char*)"LED_Task",   // 任务名称
                         (uint16_t)512,             // 任务堆栈大小
                         (void*)NULL,              // 任务入口函数参数
                         (UBaseType_t)2,            // 任务优先级
                         (TaskHandle_t*)&LED_Task_Handler);// 任务控制块指针                           
    if(xReturn == pdPASS)
    {
        printf("Create LED_Task Success\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
}

// LED任务函数
static void LED_Task(void* pvParameters)
{
    while(1)
    {
        LED1_ON();
        printf("LED ON\r\n");
        vTaskDelay(500);
        LED1_OFF();
        printf("LED OFF\r\n");
        vTaskDelay(500);
    }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
}
