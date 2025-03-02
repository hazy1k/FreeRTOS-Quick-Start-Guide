# 第二章 创建单任务-SRAM静态内存

    我们创建一个单任务，任务使用的栈和任务控制块都使用静态内存，即预先定义好的全局变量，这些预先定义好的全局变量都存在内部的 SRAM 中。

## 1. 定义任务函数

```c
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
```

任务实际上就是一个无限循环且不带返回值的 C 函数。目前，我们创建一个这样的任务，让开发板上面的 LED 灯以 500ms 的频率闪烁，

## 2. 空闲任务与定时器任务堆栈函数实现

当我们使用了静态创建任务的时候， configSUPPORT_STATIC_ALLOCATION 这个宏定 义 必 须为 1 （在 FreeRTOSConfig.h 文 件 中 ） ， 并且 我 们需 要 实 现两 个 函数 ： vApplicationGetIdleTaskMemory()与 vApplicationGetTimerTaskMemory()，这两个函数是用户设定的空闲（Idle）任务与定时器（Timer）任务的堆栈大小，必须由用户自己分配，而不能是动态分配

```c
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE]; // 空闲任务栈
static StackType_t Timer_Task_Stack[configMINIMAL_STACK_SIZE]; // 定时器任务栈
static StaticTask_t Idle_Task_TCB; // 空闲任务控制块
static StaticTask_t Timer_Task_TCB; // 定时器任务控制块

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
```

## 3. 定义任务栈

目前我们只创建了一个任务，当任务进入延时的时候，因为没有另外就绪的用户任务，那么系统就会进入空闲任务，空闲任务是 FreeRTOS 系统自己启动的一个任务，优先级最低。当整个系统都没有就绪任务的时候，系统必须保证有一个任务在运行，空闲任务就是为这个设计的。当用户任务延时到期，又会从空闲任务切换回用户任务。

在 FreeRTOS 系统中，每一个任务都是独立的，他们的运行环境都单独的保存在他们的栈空间当中。那么在定义好任务函数之后，我们还要为任务定义一个栈，目前我们使用的是静态内存，所以任务栈是一个独立的全局变量，任务的栈占用的是 MCU 内部的 RAM，当任务越多的时候，需要使用的栈空间就越大，即需要使用的RAM 空间就越多。一个 MCU 能够支持多少任务，就得看你的 RAM 空间有多少。

```c
static StackType_t AppTaskCreate_Stack[128]; // 创建任务任务栈
static StackType_t LED_Task_Stack[128]; // LED任务栈

```

## 4. 定义任务控制块

定义好任务函数和任务栈之后，我们还需要为任务定义一个任务控制块，通常我们称这个任务控制块为任务的身份证。在 C 代码上，任务控制块就是一个结构体，里面有非常多的成员，这些成员共同描述了任务的全部信息

```c
static StaticTask_t AppTaskCreate_TCB; // 创建任务控制块
static StaticTask_t LED_Task_TCB; // LED任务控制块
```



## 5. 静态创建任务

一个任务的三要素是任务主体函数，任务栈，任务控制块，那么怎么样把这三个要素联合在一起？ FreeRTOS 里面有一个叫静态任务创建函数 xTaskCreateStatic()，它就是干这个活的。 它将任务主体函数， 任务栈（静态的）和任务控制块（静态的）这三者联系在一起，让任务可以随时被系统启动

```c
    // 创建AppTaskCreate任务
    AppTaskCreate_Handler = xTaskCreateStatic((TaskFunction_t)AppTaskCreate,   // 任务函数
                                             (const char*)"AppTaskCreate",     // 任务名称
                                             (uint32_t)128,                    // 任务堆栈大小
                                             (void*)NULL,                      // 传递给任务函数的参数
                                             (UBaseType_t)3,                   // 任务优先级
                                             (StackType_t*)AppTaskCreate_Stack,// 任务栈
                                             (StaticTask_t*)&AppTaskCreate_TCB);// 任务控制块
```

## 6. 启动任务

当任务创建好后，是处于任务就绪（Ready） ，在就绪态的任务可以参与操作系统的调度。但是此时任务仅仅是创建了，还未开启任务调度器，也没创建空闲任务与定时器任务（如果使能了 configUSE_TIMERS 这个宏定义），那这两个任务就是在启动任务调度器中实现， 每个操作系统， 任务调度器只启动一次，之后就不会再次执行了， FreeRTOS 中启动任务调度器的函数是 vTaskStartScheduler()，并且启动任务调度器的时候就不会返回，从此任务管理都由FreeRTOS管理，此时才是真正进入实时操作系统中的第一步.

```c
    if(AppTaskCreate_Handler != NULL) // 创建成功
    {
        vTaskStartScheduler(); // 开启调度
    }
```

## 7. 测试工程全貌

```c
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

```


