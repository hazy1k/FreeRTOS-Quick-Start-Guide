#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handler; // 创建任务句柄
static TaskHandle_t LED_Task_Handler; // LED任务句柄

/* 全局变量声明 */
static StackType_t AppTaskCreate_Stack[128]; // 创建任务任务栈
static StackType_t LED_Task_Stack[128]; // LED任务栈
static StaticTask_t AppTaskCreate_TCB; // 创建任务控制块
static StaticTask_t LED_Task_TCB; // LED任务控制块
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE]; // 空闲任务栈
static StackType_t Timer_Task_Stack[configMINIMAL_STACK_SIZE]; // 定时器任务栈
static StaticTask_t Idle_Task_TCB; // 空闲任务控制块
static StaticTask_t Timer_Task_TCB; // 定时器任务控制块

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数
static void LED_Task(void* parameter); // LED任务函数
static void BSP_Init(void); // 板级初始化函数

/*
    使用了静态分配内存，以下这两个函数是由用户实现，函数在 task.c 文件中有引用
    当且仅当 configSUPPORT_STATIC_ALLOCATION 这个宏定义为 1 的时候才有效
*/
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize);

// 主函数启动流程
/*
    1.BSP初始化
    2.创建APP任务
    3.启动FreeRTOS,开启调度
*/
int main(void)
{
    BSP_Init(); // 板级初始化
    printf("FreeRTOS SRAM Static Create Single Task Example\r\n");
    // 创建AppTaskCreate任务
    AppTaskCreate_Handler = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,   // 任务函数
                                             (const char*)"AppTaskCreate",     // 任务名称
                                             (uint32_t)128,                    // 任务堆栈大小
                                             (void*)NULL,                      // 传递给任务函数的参数
                                             (UBaseType_t)3,                   // 任务优先级
                                             (StackType_t*)AppTaskCreate_Stack,// 任务栈
                                             (StaticTask_t*)&AppTaskCreate_TCB);// 任务控制块
    if(AppTaskCreate_Handler != NULL) // 创建成功
    {
        vTaskStartScheduler(); // 开启调度
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
    taskENTER_CRITICAL(); // 进入临界区
    // 创建LED任务
    LED_Task_Handler = xTaskCreateStatic((TaskFunction_t)LED_Task,    // 任务函数
                                        (const char*)"LED_Task",      // 任务名称
                                        (uint32_t)128,                // 任务堆栈大小
                                        (void*)NULL,                  // 传递给任务函数的参数
                                        (UBaseType_t)4,               // 任务优先级
                                        (StackType_t*)LED_Task_Stack, // 任务栈
                                        (StaticTask_t*)&LED_Task_TCB);// 任务控制块
    if(LED_Task_Handler != NULL) // LED任务创建成功
    {
        printf("LED Task Create Success\r\n");
    }
    else
    {
        printf("LED Task Create Failed\r\n");
    }  
    vTaskDelete(AppTaskCreate_Handler); // 删除AppTaskCreate任务，因为LED任务已经创建成功
    taskEXIT_CRITICAL(); // 退出临界区                                 
}

// LED任务函数
static void LED_Task(void* parameter)
{
    while(1)
    {
        LED1_ON();
        printf("LED ON\r\n");
        vTaskDelay(500);
        LED1_OFF();
        printf("LED OFF\r\n");
        vTaskDelay(500);
    }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
}

// 获取空闲任务的任务堆栈和任务控制块内存
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,StackType_t **ppxIdleTaskStackBuffer,uint32_t *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer=&Idle_Task_TCB;   /* 任务控制块内存 */
    *ppxIdleTaskStackBuffer=Idle_Task_Stack; /* 任务堆栈内存 */
    *pulIdleTaskStackSize=configMINIMAL_STACK_SIZE; /* 任务堆栈大小 */
}

// 获取定时器任务的任务堆栈和任务控制块内存
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,StackType_t **ppxTimerTaskStackBuffer,uint32_t *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer=&Timer_Task_TCB; /* 任务控制块内存 */
    *ppxTimerTaskStackBuffer=Timer_Task_Stack; /* 任务堆栈内存 */
    *pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH; /* 任务堆栈大小 */
}
