/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

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

TimerHandle_t TIM1_Handler; // 软件定时器句柄
TimerHandle_t TIM2_Handler;
void Timer1_Callback(TimerHandle_t xTimer); // 软件定时器回调函数
void Timer2_Callback(TimerHandle_t xTimer);
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Timer",RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
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
    // TIM1为周期定时器
    TIM1_Handler = xTimerCreate((const char*)"TIM1",                      // 定时器名字
                                (TickType_t)1000,                         // 定时器超时时间
                                (UBaseType_t)pdTRUE,                      // 周期性定时器
                                (void*)1,                                 // 定时器ID
                                (TimerCallbackFunction_t)Timer1_Callback);// 定时器回调函数
    // TIM2为单次定时器
    TIM2_Handler = xTimerCreate((const char*)"TIM2",                     
                                (TickType_t)1000,                         
                                (UBaseType_t)pdFALSE,                     
                                (void*)2,                                 
                                (TimerCallbackFunction_t)Timer2_Callback);
    // 创建TASK1任务
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value = 0;
    while(1)
    {
        if((TIM1_Handler != NULL) && (TIM2_Handler != NULL))
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES: // KEY0启动软件定时器
                {
                    xTimerStart((TimerHandle_t)TIM1_Handler,// 待启动的定时器
                                (TickType_t)portMAX_DELAY); // 等待系统启动定时器
                    xTimerStart((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                case KEY1_PRES: // KEY1停止软件定时器
                {
                    xTimerStop((TimerHandle_t)TIM1_Handler,
                                (TickType_t)portMAX_DELAY);
                    xTimerStop((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// Timer1_Callback函数实现
void Timer1_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim1_num = 0;
    lcd_fill(6, 131, 114, 313, lcd_discolor[++tim1_num % 11]);    /* LCD区域刷新 */
    lcd_show_xnum(79, 111, tim1_num, 3, 16, 0x80, BLUE);          /* 显示定时器1超时次数 */
}

// Timer2_Callback函数实现
void Timer2_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim2_num = 0;
    lcd_fill(126, 131, 233, 313, lcd_discolor[++tim2_num % 11]);  /* LCD区域刷新 */
    lcd_show_xnum(199, 111, tim2_num, 3, 16, 0x80, BLUE);         /* 显示定时器2超时次数 */
}

/*---------------------------------------------*/
