/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "led.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*----------------队列配置区-----------------*/
static QueueSetHandle_t xQueueSet; // 队列集
QueueHandle_t xQueuel; // 队列1
SemaphoreHandle_t xSemaphore; // 二值信号量
#define QUEUE_LENGTH    1 // 队列长度
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // 每条信息大小
#define SEMAPHORE_BINARY_LENGTH  1 // 二值信号量有效长度
#define QUEUESET_LENGTH (QUEUE_LENGTH+SEMAPHORE_BINARY_LENGTH) // 队列集长度
/*---------------------------------------------*/

/*----------------任务配置区-----------------*/
// START_TASK配置
#define START_TASK_PRIO 1           // 任务优先级 
#define START_STK_SIZE  128         // 任务堆栈大小 
TaskHandle_t StartTask_Handler;     // 任务句柄 
void start_task(void *pvParameters);// 任务函数 

// TASK1配置
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Queue Set", RED);
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
    // 创建队列集
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    // 创建队列1
    xQueuel = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // 创建二值信号量
    xSemaphore = xSemaphoreCreateBinary();
    // 将队列1和二值信号量加入队列集
    xQueueAddToSet(xQueuel, xQueueSet);
    xQueueAddToSet(xSemaphore, xQueueSet);
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
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t key_value = 0;  // 改成 uint32_t
    while(1)
    {
        uint8_t temp = key_scan(0);  // 临时用 uint8_t 接收
        switch(temp)
        {
            case KEY0_PRES:
                key_value = temp;  // 赋值给 32 位变量
                xQueueSend(xQueuel, &key_value, portMAX_DELAY);
                break;
            case KEY1_PRES:
                xSemaphoreGive(xSemaphore);
                break;
            default:
                break;
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t queue_recv = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // 等待队列集中的队列接受到消息
        if(activate_member == xQueuel)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
					printf("recv from queue1:%d\r\n", queue_recv);
        }
        else if(activate_member == xSemaphore)
        {
            xSemaphoreTake(activate_member, portMAX_DELAY); // 等待二值信号量释放
            printf("recv from semaphore\r\n");
        }
    }
}
/*---------------------------------------------*/
