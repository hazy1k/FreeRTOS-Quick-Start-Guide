/* 用户头文件 */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "lcd.h"
#include "btim.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

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
/*---------------------------------------------*/

/*----------------任务函数实现区----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,10,260,32,32,"FreeRTOS INT TEST",BLUE);
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
    btim_tim3_int_init(10000-1, 24000-1);
    btim_tim5_int_init(10000-1, 24000-1);
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

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        if(++task1_count == 5)
        {
            printf("FreeRTOS关闭它能影响的中断，TIM3不受影响");
            portDISABLE_INTERRUPTS(); // 关闭中断
            LED0(0);
            delay_ms(5000);
            printf("FreeRTOS重新打开中断，TIM3继续工作");
            LED0(1);
            portENABLE_INTERRUPTS(); // 重新打开中断
        }
        vTaskDelay(1000);
    }
}
/*---------------------------------------------*/
