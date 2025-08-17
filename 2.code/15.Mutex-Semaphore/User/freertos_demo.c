/* 用户头文件 */
#include "freertos_demo.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* LCD刷屏时使用的颜色 */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/*----------------任务配置区-----------------*/
// START_TASK配置
#define START_TASK_PRIO 1           // 任务优先级 
#define START_STK_SIZE  128         // 任务堆栈大小 
TaskHandle_t StartTask_Handler;     // 任务句柄 
void start_task(void *pvParameters);// 任务函数 

// TASK1配置
#define TASK1_PRIO      4                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      2                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

SemaphoreHandle_t MutexSemaphore; // 互斥信号量
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"MultiTask Demo",RED);
    // 创建START_TASK任务
    xTaskCreate((TaskFunction_t)start_task,        // 任务函数
                (const char*)"start_task",         // 任务名称
                (uint16_t)START_STK_SIZE,          // 任务堆栈大小
                (void*)NULL,                       // 传递给任务函数的参数
                (UBaseType_t)START_TASK_PRIO,      // 任务优先级
                (TaskHandle_t*)&StartTask_Handler);// 任务句柄
    // 开始任务调度
    vTaskStartScheduler();
}

// start_task函数实现
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // 进入临界区
    // 创建互斥信号量
    MutexSemaphore = xSemaphoreCreateMutex();  
    // 创建TASK1任务
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    // 创建TASK3任务
    xTaskCreate((TaskFunction_t)task3,  
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现-优先级低
void task1(void *pvParameters)
{
    vTaskDelay(500);
    while(1)
    {
        printf("task1 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // 获取互斥信号量
        printf("task1 has taked mutex\r\n");
        printf("task1 running\r\n");
        printf("task1 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // 释放互斥信号量
        vTaskDelay(100);

    }
}

// task2函数实现-优先级中
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    vTaskDelay(200);
    while(1)
    {
        for(task2_num = 0; task2_num < 5; task2_num++)
        {
            printf("task2 running\r\n");
            delay_ms(100); // 模拟运行，不触发任务调度
        }
        vTaskDelay(1000);
    }
}

// task3函数实现-优先级搞
void task3(void *pvParameters)
{
    uint32_t task3_num = 0;
    while(1)
    {
        printf("task3 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // 获取互斥信号量
        printf("task3 has taked mutex\r\n");
        for(task3_num = 0; task3_num < 5; task3_num++)
        {
            printf("task3 running\r\n");
            delay_ms(100); // 模拟运行，不触发任务调度
        }
        printf("task3 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // 释放互斥信号量
        vTaskDelay(1000);
    }
}
// 程序运行流程：task3获取互斥信号量（因为优先级最高）
// task3运行5次后释放互斥信号量
// task2同时运行5次
// task1获取互斥信号量（因为优先级最低）
/*---------------------------------------------*/
