/* 用户头文件 */
#include "freertos_demo.h"
#include "key.h"
#include "lcd.h"
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"

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
/*---------------------------------------------*/

/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Memory Management",RED);    
    lcd_show_string(30, 118, 200, 16, 16, "Total Mem:      Bytes", RED);
    lcd_show_string(30, 139, 200, 16, 16, "Free  Mem:      Bytes", RED);
    lcd_show_string(30, 160, 200, 16, 16, "Malloc Addr:", RED);
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
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value = 0;
    uint8_t *buf = NULL;
    size_t free_size = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                buf = pvPortMalloc(30); // 分配30字节内存
                sprintf((char*)buf, "0x%p",buf);
                lcd_show_string(130,160,200,16,16,(char*)buf,BLUE);
                break;
            }
            case KEY1_PRES:
            {
                if(NULL != buf)
                {
                    vPortFree(buf); // 释放内存
                    buf = NULL;
                }
                break;
            }
            default:break;
        }
        lcd_show_xnum(114,118,configTOTAL_HEAP_SIZE,5,16,0,BLUE); // 显示总内存大小
        free_size = xPortGetFreeHeapSize(); // 获取空闲内存大小
        lcd_show_xnum(114,139,free_size,5,16,0,BLUE);
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/