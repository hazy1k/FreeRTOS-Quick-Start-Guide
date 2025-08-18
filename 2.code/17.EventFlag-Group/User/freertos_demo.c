/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

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
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      4                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      3                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

EventGroupHandle_t Event_handle;
#define EVENT0_BIT (1 << 0)     // 事件位
#define EVENT1_BIT (1 << 1)
#define EVENTALL_BIT (EVENT0_BIT | EVENT1_BIT) // 等待所有事件位的宏
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Event Group", RED);
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

    // 创建事件标志组
    Event_handle = xEventGroupCreate(); // 句柄名称恢复

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

void task1(void *pvParameters)
{
    uint8_t key_value; // 变量名恢复
    while(1)
    {
        key_value = key_scan(0); // 函数名和参数恢复
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT0_BIT); // 句柄和事件位宏恢复
                break;
            }
            case KEY1_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT1_BIT); // 句柄和事件位宏恢复
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
    uint32_t task2_value = 0; // 变量名和初始化恢复
    while(1)
    {
        xEventGroupWaitBits((EventGroupHandle_t)Event_handle,// 等待的事件标志组句柄恢复
                            (EventBits_t)EVENTALL_BIT,       // 等待的事件标志宏恢复
                            (BaseType_t)pdTRUE,              // 函数退出时清零等待的事件
                            (BaseType_t)pdTRUE,              // 等待 等待事件中的所有事件
                            (TickType_t)portMAX_DELAY);      // 等待时间
        lcd_fill(6,131,233,313,lcd_discolor[++task2_value%11]); // LCD区域刷新
        vTaskDelay(10);
    }
}

// task3函数实现
void task3(void *pvParameters)
{
    EventBits_t event_value; // 变量名恢复
    while(1)
    {
        event_value = xEventGroupGetBits((EventGroupHandle_t)Event_handle); 
        lcd_show_xnum(182,110,event_value,1,16,0,BLUE);
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/