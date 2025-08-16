/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* LCD刷屏时使用的颜色 */
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};

/*----------------队列配置区-----------------*/
static QueueSetHandle_t xQueueSet; // 队列集
QueueHandle_t xQueue1; // 队列1
QueueHandle_t xQueue2; // 队列2
#define QUEUE_LENGTH 1 // 队列支持消息个数
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // 队列消息大小
#define QUEUESET_LENGTH (2*QUEUE_LENGTH) // 队列集支持队列个数
#define EVENTBIT_0 (1<<0) // 事件位
#define EVENTBIT_1 (1<<1) 
#define EVENTBIT_ALL (EVENTBIT_0 | EVENTBIT_1)
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
    lcd_show_string(10, 47, 220, 24, 24, "Queue Event Group", RED);
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
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
    xQueue1 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // 创建队列2
    xQueue2 = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // 将队列1和2加入队列集
    xQueueAddToSet(xQueue1, xQueueSet);
    xQueueAddToSet(xQueue2, xQueueSet);
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
    uint32_t key_value =0;
    uint32_t eventbit_0 = EVENTBIT_0;
    uint32_t eventbit_1 = EVENTBIT_1;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xQueueSend(xQueue1, &eventbit_0, portMAX_DELAY); // 发送事件到队列1
                break;
            }
            case KEY1_PRES:
            {
                xQueueSend(xQueue2, &eventbit_1, portMAX_DELAY);
                break;
            }
            default:break;
        }
        vTaskDelay(10); 
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t event_val = 0;
    uint32_t event_recv = 0;
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t task2_num = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // 选择队列集中的一个队列
        xQueueReceive(activate_member, &event_recv, portMAX_DELAY);
        event_val |= event_recv; // 合并事件值
        lcd_show_xnum(182,110,event_val,1,16,0,BLUE);
        if(event_val == EVENTBIT_ALL) // 事件集中包含所有事件
        {
            event_val = 0; // 清空事件集
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
        }
    }
}
/*---------------------------------------------*/
