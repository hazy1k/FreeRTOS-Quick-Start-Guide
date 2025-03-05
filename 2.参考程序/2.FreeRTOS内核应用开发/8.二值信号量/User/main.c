/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
#include "semphr.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t Receive_Task_Handle = NULL; // 接收任务句柄
static TaskHandle_t Send_Task_Handle = NULL; // 发送任务句柄

/* 内核对象句柄 */
SemaphoreHandle_t BinarySem_Handle = NULL; // 二值信号量句柄
/* 全局变量声明 */
/* 宏定义 */


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
    printf("按下KEY1或者KEY2进行任务与任务间的同步\r\n");
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

    BinarySem_Handle = xSemaphoreCreateBinary(); // 创建二值信号量
    if(BinarySem_Handle != NULL)
    {
        printf("二值信号量创建成功！\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Receive_Task,
                         (const char*)"Receive_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)2,
                         (TaskHandle_t*)&Receive_Task_Handle);
    if(xReturn == pdPASS)
    {
        printf("接收任务创建成功！\r\n");
    }
    xReturn = xTaskCreate((TaskFunction_t)Send_Task,
                         (const char*)"Send_Task",
                         (uint16_t)512,  
                         (void*)NULL,
                         (UBaseType_t)3,
                         (TaskHandle_t*)&Send_Task_Handle);
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
    BaseType_t xReturn = pdPASS;
    while(1)
    {
        // 获取二值信号量xSemaphore
        xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY);
        if(xReturn == pdTRUE)
        {
            printf("接收任务获取二值信号量成功！\r\n");
        }
        LED1_TOGGLE();
    }
}

// 发送任务函数
static void Send_Task(void* pvParameters)
{
    BaseType_t xReturn = pdPASS;
    while(1)
    {
        if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreGive(BinarySem_Handle); // 给出二值信号
            if(xReturn == pdTRUE)
            {
                printf("二值信号释放成功！\r\n");
            }
            else
            {
                printf("二值信号释放失败！\r\n");
            }
        }
        if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            xReturn = xSemaphoreGive(BinarySem_Handle);
            if(xReturn == pdTRUE)
            {
                printf("二值信号释放成功！\r\n");
            }
            else
            {
                printf("二值信号释放失败！\r\n");
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
