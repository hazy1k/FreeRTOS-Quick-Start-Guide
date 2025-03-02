# 第三章 创建单任务-SRAM动态内存

这里，我们创建一个单任务，任务使用的栈和任务控制块是在创建任务的时候FreeRTOS 动态分配的，并不是预先定义好的全局变量。

## 1. 动态内存空间的堆从哪里来

在创建单任务—SRAM 静态内存的例程中，任务控制块和任务栈的内存空间都是从内部的 SRAM 里面分配的，具体分配到哪个地址由编译器决定。现在我们开始使用动态内存，即堆，其实堆也是内存，也属于 SRAM。 FreeRTOS 做法是在 SRAM 里面定义一个大数组，也就是堆内存，供 FreeRTOS 的动态内存分配函数使用，在第一次使用的时候，系统会将定义的堆内存进行初始化，这些代码在 FreeRTOS 提供的内存管理方案中实现（heap_1.c、heap_2.c、 heap_4.c 等，具体的内存管理方案后面详细讲解）

```c
//系统所有总的堆大小
#define configTOTAL_HEAP_SIZE ((size_t)(36*1024)) (1)
static uint8_t ucHeap[ configTOTAL_HEAP_SIZE ]; (2)
/* 如果这是第一次调用 malloc 那么堆将需要初始化， 以设置空闲块列表。 */
if ( pxEnd == NULL )
{
    prvHeapInit(); (3)
} else
{
    mtCOVERAGE_TEST_MARKER();
}
```

## 2. 定义任务函数

用动态内存的时候，任务的主体函数与使用静态内存时是一样的

```c
// LED任务函数
static void LED_Task(void* pvParameters)
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

## 3. 定义任务栈

使用动态内存的时候，任务栈在任务创建的时候创建，不用跟使用静态内存那样要预先定义好一个全局的静态的栈空间，动态内存就是按需分配内存，随用随取。

## 4. 定义任务控制块指针

使用动态内存时候，不用跟使用静态内存那样要预先定义好一个全局的静态的任务控制块空间。任务控制块是在任务创建的时候分配内存空间创建，任务创建函数会返回一个指针，用于指向任务控制块，所以要预先为任务栈定义一个任务控制块指针，也是我们常说的任务句柄

```c
/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄
static TaskHandle_t LED_Task_Handler = NULL; // LED任务句柄
```

## 5. 动态创建任务

使用静态内存时，使用 xTaskCreateStatic()来创建一个任务，而使用动态内存的时，则使用 xTaskCreate()函数来创建一个任务，两者的函数名不一样，具体的形参也有区别

```c
// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // 进入临界区
    xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数
                         (const char*)"LED_Task",   // 任务名称
                         (uint16_t)512,             // 任务堆栈大小
                         (void*)NULL,              // 任务入口函数参数
                         (UBaseType_t)2,            // 任务优先级
                         (TaskHandle_t*)&LED_Task_Handler);// 任务控制块指针                           
    if(xReturn == pdPASS)
    {
        printf("Create LED_Task Success\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
} 
```

## 6. 启动任务

当任务创建好后，是处于任务就绪（Ready） ，在就绪态的任务可以参与操作系统的调度。但是此时任务仅仅是创建了，还未开启任务调度器，也没创建空闲任务与定时器任务（如果使能了 configUSE_TIMERS 这个宏定义），那这两个任务就是在启动任务调度器中实现，每个操作系统，任务调度器只启动一次，之后就不会再次执行了， FreeRTOS 中启动任务调度器的函数是 vTaskStartScheduler()，并且启动任务调度器的时候就不会返回，从此任务管理都由FreeRTOS管理，此时才是真正进入实时操作系统中的第一步

```c
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
        printf("FreeRTOS Start Success\r\n");
    }
```

## 7. 测试工程全貌

```c
#include "FreeRTOS.h"
#include "task.h"
#include "uart.h"
#include "led.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄
static TaskHandle_t LED_Task_Handler = NULL; // LED任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数
static void LED_Task(void* pvParameters); // LED任务函数
static void BSP_Init(void); // 板级初始化函数

// 主函数启动流程
/*
    1.BSP初始化
    2.创建APP任务
    3.启动FreeRTOS,开启调度
*/
int main(void)
{
    BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
    BSP_Init(); // 板级初始化
    printf("FreeRTOS SRAM Dynamic Create Single Task Example\r\n");
    // 创建AppTaskCreate任务   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*    )"AppTaskCreate",
                        (uint16_t       )512,  
                        (void*          )NULL,
                        (UBaseType_t    )1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
        printf("FreeRTOS Start Success\r\n");
    }
    else
    {
        return -1;
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS;
    taskENTER_CRITICAL(); // 进入临界区
    xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数
                         (const char*)"LED_Task",   // 任务名称
                         (uint16_t)512,             // 任务堆栈大小
                         (void*)NULL,              // 任务入口函数参数
                         (UBaseType_t)2,            // 任务优先级
                         (TaskHandle_t*)&LED_Task_Handler);// 任务控制块指针                           
    if(xReturn == pdPASS)
    {
        printf("Create LED_Task Success\r\n");
    }
    vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
    taskEXIT_CRITICAL(); // 退出临界区
}

// LED任务函数
static void LED_Task(void* pvParameters)
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

```


