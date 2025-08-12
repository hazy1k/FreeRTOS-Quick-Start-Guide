/* 用户头文件 */
#include "freertos_demo.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

/*-------------全局变量声明和其他配置------------*/
// LCD显示颜色
uint16_t lcd_discolor[11] = {WHITE, BLACK, BLUE, RED,
                             MAGENTA, GREEN, CYAN, YELLOW,
                             BROWN, BRRED, GRAY};
/*---------------------------------------------*/

/*----------------任务函数配置区-----------------*/
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

// TASK3配置
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);
/*---------------------------------------------*/

/*----------------任务函数实现区----------------*/
void freertos_demo(void)
{
    // 初始化LCD显示
    lcd_show_string(10, 47, 220, 30, 24, "Task Create&Delete", RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);

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
    // 创建TASK3任务
    xTaskCreate((TaskFunction_t)task3,      
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,               
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // 删除START_TASK任务
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        lcd_fill(6,131,114,313,lcd_discolor[++task1_count % 11]);
        lcd_show_xnum(71,111,task1_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task2函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task2(void *pvParameters)
{
    uint32_t task2_count = 0;
    while(1)
    {
        lcd_fill(126,131,233,313,lcd_discolor[11-(++task2_count % 11)]);
        lcd_show_xnum(191,111,task2_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task3函数实现
// 实现功能：按下KEY0删除task1任务，按下KEY1删除task2任务
void task3(void *pvParameters)
{
    uint8_t key_value;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY0_PRES)
        {
            if(Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                lcd_show_string(15, 111, 110, 16, 16, "Task1: Del", RED); 
            }
        }
        if(key_value == KEY1_PRES)
        {
            if(Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                lcd_show_string(135, 111, 110, 16, 16, "Task2: Del", RED);
            }
        }
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/
