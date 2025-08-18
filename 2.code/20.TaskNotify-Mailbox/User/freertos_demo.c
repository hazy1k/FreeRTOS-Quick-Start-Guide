/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
#include "led.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

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
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_draw_rectangle(5, 125, 234, 314, BLACK);
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

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        key_value = key_scan(0);
        if((Task2Task_Handler != NULL) && (key_value != 0))
        {
            // 接受任务通知的句柄 要更新的bit位 更新方式为覆写
            xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)key_value, (eNotifyAction)eSetValueWithOverwrite);
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t nofify_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        xTaskNotifyWait((uint32_t)0x00000000,      // 进入函数时，不清除任务通知值
                        (uint32_t)0xFFFFFFFF,      // 函数退出时，清除所有任务通知值
                        (uint32_t*)&nofify_value,  // 等待接收通知值
                        (TickType_t)portMAX_DELAY);
        switch(nofify_value)
        {
            case KEY0_PRES:
            {
                lcd_fill(6, 126, 233, 313, lcd_discolor[++task2_value % 11]);
                break;
            }
            case KEY1_PRES:
            {
                LED0_TOGGLE();
                break;
            }
            case WKUP_PRES:
            {
                LED1_TOGGLE();
                break;
            }
            default:break;
        }
    }
}
/*---------------------------------------------*/