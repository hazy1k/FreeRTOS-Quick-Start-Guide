# 第五章 FreeRTOS任务切换

## 1. PendSV异常

PendSV（Pended Service Call，可挂起服务调用），是一个对 RTOS 非常重要的异常。 PendSV的中断优先级是可以编程的，用户可以根据实际的需求，对其进行配置。 PendSV 的中断由将中断控制状态寄存器（ICSR）中 PENDSVSET 为置一触发。PendSV 与 SVC 不同， PendSV 的中断是非实时的，即 PendSV 的中断可以在更高优先级的中断中触发，但是在更高优先级中断结束后才执行。

利用 PendSV 的这个可挂起特性，在设计 RTOS 时，可以将 PendSV 的中断优先级设置为最低的中断优先级，这么一来， PendSV 的中断服务函数就会在其他所有中断处理完成后才执行。任务切换时，就需要用到 PendSV 的这个特性。

首先，来看一下任务切换的一些基本概念，在典型的 RTOS 中，任务的处理时间被分为多个时间片， OS 内核的执行可以有两种触发方式，一种是通过在应用任务中通过 SVC 指令触发，例如在应用任务在等待某个时间发生而需要停止的时候，那么就可以通过 SVC 指令来触发 OS内核的执行，以切换到其他任务；第二种方式是， SysTick 周期性的中断，来触发 OS 内核的执行。 下图演示了只有两个任务的 RTOS 中，两个任务交替执行的过程：

![屏幕截图 2025-08-13 183226.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/13-18-32-43-屏幕截图%202025-08-13%20183226.png)

在操作系统中，任务调度器决定是否切换任务。图中的任务及切换都是在 SysTick 中断中完成的， SysTick 的每一次中断都会切换到其他任务。

如果一个中断请求（IRQ）在 SysTick 中断产生之前产生，那么 SysTick 就可能抢占该中断请求，这就会导致该中断请求被延迟处理，这在实时操作系统中是不允许的，因为这将会影响到实时操作系统的实时性，如下图所示：

![屏幕截图 2025-08-13 183343.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/13-18-33-47-屏幕截图%202025-08-13%20183343.png)

并且，当 SysTick 完成任务的上下文切换，准备返回任务中运行时，由于存在中断请求， ARM Cortex-M 不允许返回线程模式，因此，将会产生用法错误异常（Usage Fault）。在一些 RTOS 的设计中，会通过判断是否存在中断请求，来决定是否进行任务切换。虽然可以通过检查 xPSR 或 NVIC 中的中断活跃寄存器来判断是否存在中断请求，但是这样可能会影响系统的性能，甚至可能出现中断源在 SysTick 中断前后不断产生中断请求，导致系统无法进行任务切换的情况。PendSV 通过延迟执行任务切换，直到处理完所有的中断请求，以解决上述问题。为了达到这样的效果，必须将 PendSV 的中断优先级设置为最低的中断优先等级。如果操作系统决定切换任务，那么就将 PendSV 设置为挂起状态，并在 PendSV 的中断服务函数中执行任务切换，如下图所示：

![屏幕截图 2025-08-13 183428.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/13-18-34-31-屏幕截图%202025-08-13%20183428.png)

1. 任务一触发 SVC 中断以进行任务切换（例如，任务一正等待某个事件发生）。

2. 系统内核接收到任务切换请求，开始准备任务切换，并挂起 PendSV 异常。

3. 当退出 SVC 中断的时候，立刻进入 PendSV 异常处理，完成任务切换。

4. 当 PendSV 异常处理完成，返回线程模式，开始执行任务二。

5. 中断产生，并进入中断处理函数。

6. 当运行中断处理函数的时候， SysTick 异常（用于内核时钟节拍） 产生。

7. 操作系统执行必要的操作，然后挂起 PendSV 异常，准备进行任务切换。

8. 当 SysTick 中断处理完成，返回继续处理中断。

9. 当中断处理完成，立马进入 PendSV 异常处理，完成任务切换。

10. 当 PendSV 异常处理完成，返回线程模式，继续执行任务一。

PendSV在RTOS的任务切换中，起着至关重要的作用， FreeRTOS的任务切换就是在PendSV中完成的。

## 2. 时间片调度测试

### 2.1 任务配置

```c
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
#define TASK2_PRIO      2 // 相同任务优先级                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*-------------------------------------------*/
```

### 2.2 任务实现

```c
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
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t task1_num = 0;
    while(1)
    {
        taskENTER_CRITICAL(); // 进入临界区
        printf("任务1运行次数：%d\r\n",++task1_num);
        taskEXIT_CRITICAL(); // 退出临界区
                vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    while(1)
    {
        taskENTER_CRITICAL(); // 进入临界区
        printf("任务2运行次数：%d\r\n",++task2_num);
        taskEXIT_CRITICAL(); // 退出临界区
              vTaskDelay(10);
    }
}
```

### 2.3 主函数

```c
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "mpu.h"
#include "lcd.h"
#include "key.h"
#include "malloc.h"
#include "freertos_demo.h"

int main(void)
{
    sys_cache_enable();                 /* 打开L1-Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(240, 2, 2, 4); /* 设置时钟, 480Mhz */
    delay_init(480);                    /* 延时初始化 */
    usart_init(115200);                 /* 串口初始化为115200 */
    led_init();                         /* 初始化LED */
    mpu_memory_protection();            /* 保护相关存储区域 */
    lcd_init();                         /* 初始化LCD */
    key_init();                         /* 初始化按键 */
    my_mem_init(SRAMIN);                /* 初始化内部内存池(AXI) */
    freertos_demo();                    /* 运行FreeRTOS例程 */
}
```

---
