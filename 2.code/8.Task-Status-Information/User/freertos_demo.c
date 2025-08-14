/* 用户头文件 */
#include "freertos_demo.h"
#include "usart.h"
#include "delay.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include <string.h>
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
/*-------------------------------------------*/

/*------------------任务实现区----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Demo",RED);
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

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t i = 0;
    UBaseType_t task_num = 0;
    TaskStatus_t *status_array = NULL;
    TaskHandle_t task_handle = NULL;
    TaskStatus_t *task_info = NULL;
    eTaskState task_state = eInvalid;
    char *task_state_str = NULL;
    char *task_info_buf = NULL;

    /*--------------------函数uxTaskGetSystemState()的使用------==--------------*/
    printf("-------------Step1 函数uxTaskGetSystemState()的使用-------------\r\n");
    task_num = uxTaskGetNumberOfTasks(); // 获取系统中任务数量
    status_array = mymalloc(SRAMIN, task_num*sizeof(TaskStatus_t)); // 内存分配
    task_num = uxTaskGetSystemState((TaskStatus_t*)status_array, // 任务状态信息
                                    (UBaseType_t)task_num, // buffer大小
                                    (uint32_t*)NULL); // 不获取任务运行时间信息
    printf("任务名\t\t优先级\t\t任务编号\r\n");
    for(i=0;i<task_num;i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
        status_array[i].pcTaskName,
        strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
        status_array[i].uxCurrentPriority,
        status_array[i].xTaskNumber);
    }   
    myfree(SRAMIN, status_array);     
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }
    
    /*-------------------------函数vTaskGetInfo()的使用--------------------------*/
    printf("-----------------Step2 函数vTaskGetInfo()的使用------------------\r\n");
    task_info = mymalloc(SRAMIN, sizeof(TaskStatus_t)); // 内存分配
    task_handle = xTaskGetHandle("task1"); // 获取任务句柄
    vTaskGetInfo((TaskHandle_t)task_handle, // 任务句柄
                 (TaskStatus_t*)task_info,  // 任务状态信息
                 (BaseType_t)pdTRUE,        // 允许统计任务堆栈历史最小值
                 (eTaskState)eInvalid);     // 任务状态
    printf("任务名:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("任务编号:\t\t%ld\r\n", task_info->xTaskNumber);
    printf("任务壮态:\t\t%d\r\n", task_info->eCurrentState);
    printf("任务当前优先级:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("任务基优先级:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("任务堆栈基地址:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("任务堆栈历史剩余最小值:\t%d\r\n", task_info->usStackHighWaterMark);
    myfree(SRAMIN, task_info);
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*-------------------------函数eTaskGetState()的使用--------------------------*/
    printf("-----------------Step3 函数eTaskGetState()的使用------------------\r\n");
    task_state_str = mymalloc(SRAMIN, 10); // 内存分配
    task_handle = xTaskGetHandle("task1");
    task_state = eTaskGetState(task_handle);
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("任务状态值: %d，对应状态为: %s\r\n", task_state, task_state_str);
    myfree(SRAMIN, task_state_str);
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*---------------------------函数vTaskList()的使用--------------------------*/
    printf("------------------Step4 函数vTaskList()的使用-------------------\r\n");
    task_info_buf = mymalloc(SRAMIN, 500);
    vTaskList(task_info_buf); // 打印任务列表
    printf("任务名\t\t状态\t优先级\t剩余栈\t任务序号\r\n");
    printf("%s\r\n", task_info_buf);
    myfree(SRAMIN, task_info_buf);
    printf("-----------------------------END-------------------------------\r\n");
    while(1)
    {
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/