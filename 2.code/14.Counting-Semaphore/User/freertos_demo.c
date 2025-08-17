/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
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
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

SemaphoreHandle_t CountSemaphore; // 计数信号量
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Counting Semaphore",RED);
    lcd_show_string(54, 111, 200, 16, 16, "Notify Value:  0", BLUE);
    lcd_draw_rectangle(5, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 234, 130, BLACK);
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
    // 创建计数信号量
    CountSemaphore = xSemaphoreCreateCounting((UBaseType_t)255, (UBaseType_t)0); // 创建计数信号量，初始值为0，最大值为255  
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
    uint8_t key_value = 0;
    UBaseType_t sema_value = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xSemaphoreGive(CountSemaphore); // 给计数信号量加1
                sema_value = uxSemaphoreGetCount(CountSemaphore); // 获取计数信号量的值
                lcd_show_xnum(166, 111, sema_value, 2, 16, 0, BLUE);
            }
            default:break;
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    UBaseType_t sema_value = 0;
    uint32_t task2_num = 0;
    while(1)
    {
        xSemaphoreTake(CountSemaphore, portMAX_DELAY); // 拿走计数信号量，阻塞等待
        sema_value = uxSemaphoreGetCount(CountSemaphore);
        lcd_show_xnum(166, 111, sema_value, 2, 16, 0, BLUE);             
        lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);     
        vTaskDelay(1000);
    }
}
/*---------------------------------------------*/
