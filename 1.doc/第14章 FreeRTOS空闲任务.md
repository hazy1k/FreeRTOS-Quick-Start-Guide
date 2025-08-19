# 第十四章 FreeRTOS空闲任务

## 1. 空闲任务简介

在前面章节分析 vTaskStartScheduler()函数启动任务调度器的时候，就了解到， FreeRTOS 会自动创建一个空闲任务，这样就可以确保系统中永远都至少有一个正在被执行的任务。空闲任务是以系统中最低的任务优先级被创建的，这样可以确保空闲任务不会占用其他就绪态任务的被执行时间。

当有任务被函数 vTaskDelete()函数删除时，如果函数 vTaskDelete()的调用者与被删除任务不是同一个任务，那么被删除任务的删除工作可以由函数 vTaskDelete()的调用者完成。如果vTaskDelete()的调用者和被删除任务为同一个任务，即一个任务调用函数 vTaskDelete()删除了自身，那么删除工作是不能完全由任务本身完成的， 因此这项删除任务自身的工作就交给了空闲任务，也正因如此，如果在任务中调用了函数 vTaskDelete()删除了自身，那么就必须要保证最低任务优先级的空闲任务能够被分配到运行时间。

## 2. 空闲任务钩子函数

### 2.1 FreeRTOS 中的钩子函数

FreeRTOS 提供了多种钩子函数，当系统运行到某个功能或函数的某个位置时，就会调用相应的钩子函数，至于钩子函数具体要实现什么功能，可有由用户自行编写。当然，钩子函数是一项可选功能，用户如果不需要使用相应的钩子函数，那就无需编写相应的钩子函数。在FreeRTOSConfig.h 文件中就可以看到启用钩子函数的相关配置项，具体的代码如下所示：

```c
/* 钩子函数相关定义 */
#define configUSE_IDLE_HOOK 0 /* 空闲任务钩子函数 */
#define configUSE_TICK_HOOK 0 /* 系统时钟节拍中断钩子函数 */
#define configUSE_MALLOC_FAILED_HOOK 0 /* 动态内存申请失败钩子函数 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK 0 /* 首次执行定时器服务任务钩子函数 */
```

如果要启用相应的钩子函数，只需将对应的配置项配置为 1 即可，当然也不要忘了编写相应的钩子函数。

### 2.2 FreeRTOS 空闲任务钩子函数

如果将宏 configUSE_IDLE_HOOK 配置为1，那么在空闲任务的每一个运行周期中，都会调用一次函数 vApplicationIdleHook()，此函数就是空闲任务的钩子函数。

如果想在空闲任务相同的优先级中处理某些事务，那么有两种选择：

1. 在空闲任务的钩子函数中处理需要处理的任务

在这种情况下，需要特别注意，因为不论在什么时候，都应该保证系统中有一个正在被执行的任务，因此在空闲任务的钩子函数中，不能够调用会到时空闲任务被阻塞或挂起的函数，例如函数 vTaskDelay()。

2. 在和空闲任务相同任务优先级的任务中处理需要处理的事务

创建一个和空闲任务相同优先级的任务来处理需要处理的事务是一个比较好的方法，但是这会导致消耗更多的 RAM。

通常在空闲任务的钩子函数中设置处理器进入相应的低功耗模式，以达到降低整体功率的目的，为了与 FreeRTOS 自带的低功耗 Tickless 模式做区分，这里暂且将这种使用空闲任务钩子函数的低功耗模式成为通用低功耗模式，这是因为，几乎所有的 RTOS 都可以使用这种方式实现低功耗。

通用的低功耗模式会使处理器在每次进入空闲任务函数时，进入相应的低功耗模式，并且在每次 SysTick 中断发生的时候都会被唤醒， 可见通用方式实现的低功耗效果远不如 FreeRTOS自带的低功耗 Tickless 模式，但是这种方式更加通用。

## 3. 空闲任务钩子函数测试

### 3.1 任务配置

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
/*---------------------------------------------*/
```

### 3.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"HOOK",RED);    
    delay_ms(2000);  
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
    lcd_display_off();
    LCD_BL(0);
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
    while(1)
    {
        LED0(1); // LED0灭，代表退出低功耗模式
        delay_ms(3000); // 模拟任务运行，此时不会进入低功耗模式
        LED0(0); // LED0亮，代表进入低功耗模式
        vTaskDelay(3000);
    }
}
```

### 3.3 钩子函数

```c
void before_sleep(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
}

void after_wakeup(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}

// 空闲任务钩子函数
void vApplicationIdleHook(void)
{
    __disable_irq();
    __dsb(0);
    __isb(0);
    before_sleep();

    // 进入睡眠模式（等待中断唤醒）
    __WFI();

    after_wakeup();
    __dsb(0);
    __isb(0);
    __enable_irq();
}
```

### 3.4 主函数

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


