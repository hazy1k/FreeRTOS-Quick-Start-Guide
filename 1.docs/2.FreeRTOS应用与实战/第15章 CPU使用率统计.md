# 第十五章 CPU使用率统计

## 1. CPU利用率的基本概念

CPU 使用率其实就是系统运行的程序占用的 CPU 资源，表示机器在某段时间程序运行的情况，如果这段时间中，程序一直在占用 CPU 的使用权，那么可以人为 CPU 的利用率是 100%。 CPU 的利用率越高，说明机器在这个时间上运行了很多程序，反之较少。利用率的高低与 CPU 强弱有直接关系，就像一段一模一样的程序，如果使用运算速度很慢的CPU，它可能要运行 1000ms，而使用很运算速度很快的 CPU 可能只需要 10ms，那么在1000ms 这段时间中，前者的 CPU 利用率就是 100%，而后者的 CPU 利用率只有 1%，因为1000ms 内前者都在使用 CPU 做运算，而后者只使用 10ms 的时间做运算，剩下的时间CPU 可以做其他事情。

FreeRTOS 是多任务操作系统，对 CPU 都是分时使用的：比如 A 任务占用 10ms，然后 B 任务占用 30ms，然后空闲 60ms，再又是 A任务占 10ms， B 任务占 30ms，空闲 60ms;如果在一段时间内都是如此，那么这段时间内的利用率为 40%，因为整个系统中只有 40%的时间是 CPU 处理数据的时间。

## 2. CPU利用率的作用

一个系统设计的好坏，可以使用 CPU 使用率来衡量，一个好的系统必然是能完美响应急需的处理，并且系统的资源不会过于浪费（性价比高）。举个例子，假设一个系统的CPU 利用率经常在 90%~100%徘徊，那么系统就很少有空闲的时候，这时候突然有一些事情急需 CPU 的处理，但是此时 CPU 都很可能被其他任务在占用了，那么这个紧急事件就有可能无法被相应，即使能被相应，那么占用 CPU 的任务又处于等待状态，这种系统就是不够完美的，因为资源处理得太过于紧迫；反过来，假如 CPU 的利用率在 1%以下，那么我们就可以认为这种产品的资源过于浪费，搞一个那么好的 CPU 去干着没啥意义的活（大部分时间处于空闲状态），使用，作为产品的设计，既不能让资源过于浪费，也不能让资源过于紧迫，这种设计才是完美的，在需要的时候能及时处理完突发事件，而且资源也不会过剩，性价比更高

## 3. CPU利用率统计实验

CPU 利用率实验是是在 FreeRTOS 中创建了三个任务，其中两个任务是普通任务，另一个任务用于获取 CPU 利用率与任务相关信息并通过串口打印出来。

```c
/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "TiMbase.h"
#include "string.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t LED1_Task_Handle = NULL; // LED1任务句柄
static TaskHandle_t LED2_Task_Handle = NULL; // LED2任务句柄
static TaskHandle_t CPU_Task_Handle = NULL; // CPU使用率任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */
/* 宏定义 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void LED1_Task(void* pvParameters);
static void LED2_Task(void* pvParameters);
static void CPU_Task(void* pvParameters);

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
    printf("FreeRTOS CPU使用率统计Demo\r\n");
    // 创建AppTaskCreate任务   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*)"AppTaskCreate",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
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

  xReturn = xTaskCreate((TaskFunction_t)LED1_Task,
                        (const char*)"LED1_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&LED1_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED1任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)LED2_Task,
                        (const char*)"LED2_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&LED2_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("LED2任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t)CPU_Task,
                        (const char*)"CPU_Task",
                        (uint16_t)512,
                        (void*)NULL,
                        (UBaseType_t)4,
                        (TaskHandle_t*)&CPU_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("CPU使用率任务创建成功。\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

static void LED1_Task(void* pvParameters)
{
  while(1)
  {
    LED1_ON();
    vTaskDelay(500);
    printf("LED1_Task Running..., LED1_ON\r\n");
    LED1_OFF();
    vTaskDelay(500);
    printf("LED1_Task Running..., LED1_OFF\r\n");
  }
}

static void LED2_Task(void* pvParameters)
{
  while(1)
  {
    LED2_ON();
    vTaskDelay(1000);
    printf("LED2_Task Running..., LED2_ON\r\n");
    LED2_OFF();
    vTaskDelay(1000);
    printf("LED2_Task Running..., LED2_OFF\r\n");
  }
}
static void CPU_Task(void* parameter)
{
  uint8_t CPU_RunInfo[400]; // 保存任务运行时间信息
  while (1) 
  {
    memset(CPU_RunInfo,0,400); // 信息缓冲区清零
    vTaskList((char*)&CPU_RunInfo); // 获取任务运行时间信息
    printf("---------------------------------------------\r\n");
    printf("任务名 任务状态 优先级 剩余栈 任务序号\r\n");
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n");
    memset(CPU_RunInfo,0,400); //信息缓冲区清零
    vTaskGetRunTimeStats((char *)&CPU_RunInfo);
    printf("任务名 运行计数 使用率\r\n"); 
    printf("%s", CPU_RunInfo);
    printf("---------------------------------------------\r\n\n");
    vTaskDelay(500);
  }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
    BASIC_TIM_Init();
}
```
