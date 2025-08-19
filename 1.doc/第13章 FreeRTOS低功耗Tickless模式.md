# 第十三章 FreeRTOS低功耗Tickless模式

## 1. 低功耗 Tickless 模式简介

在整个系统的运行过程中，其实大部分的时间是在执行空闲任务的，而空闲任务之所及叫做空闲任务，是因为空闲任务是在系统中的所有其他都阻塞或被挂起时才运行的，因此可以在本该空闲任务执行的期间，让MCU 进入相应的低功耗模式，接着在其他任务因被解除阻塞或其他原因，而准备运行的时候，让 MCU 退出相应的低功耗模式，去执行相应的任务。 在以上这一过程中，主要的难点在于， MCU 进入相应的低功耗模式后，如何判断有除空闲任务外的其他任务就绪，并退出相应的空闲模式去执行就绪任务，也就是如何计算 MCU 进入相应低功耗模式的时间，而 FreeRTOS 的低功耗 Tickless 模式机制已经处理好了这个问题。

## 2. 低功耗 Tickless 模式相关配置项

在前面对 FreeRTOS 低功耗 Tickless 模式的简介中提到了 FreeRTOS 中针对该模式的几个配置，如下表所示：

![屏幕截图 2025-08-19 141428.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/19-14-14-45-屏幕截图%202025-08-19%20141428.png)

1. configUSE_TICKLESS_IDLE

此宏用于使能低功耗 Tickless 模式，当此宏定义为 1 时，系统会在进入空闲任务期间进入相应的低功耗模式大于 configEXPECTED_IDLE_TIME_BEFORE_SLEEP 的时长。

2. configEXPECTED_IDLE_TIME_BEFORE_SLEEP

此宏用于定义系统进入相应低功耗模式的最短时长，如果系统在进入相应低功耗模式前，计算出系统将进入相应低功耗的时长小于 configEXPECTED_IDLE_TIME_BEFORE_SLEEP 定义的最小时长，则系统不进入相应的低功耗模式，要注意的是，此宏的值不能小于 2。

3. configPRE_SLEEP_PROCESSING(x)

此宏用于定义一些需要在系统进入相应低功耗模式前执行的事务，例如可以在进入低功耗模式前关闭一些 MCU 片上外设的时钟，以达到降低功耗的目的。

4. configPOSR_SLEEP_PROCESSING(x)

此宏用于定义一些需要在系统退出相应低功耗模式后执行的事务，例如开启在系统在进入相应低功耗模式前关闭的 MCU 片上外设的时钟，以是系统能够正常运行。

## 3. 低功耗 Tickless测试

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
    lcd_show_string(10,47,220,24,24,"FreeRTOS Tickless",RED);      
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

### 3.3 低功耗处理

```c
#if (configUSE_TICKLESS_IDLE == 1)
// 进入低功耗模式执行的操作，只做演示，以实际要求为准
void PRE_SLEEP_PROCESSING(void)
{
    __HAL_RCC_GPIOA_CLK_DISABLE();
    __HAL_RCC_GPIOB_CLK_DISABLE();
    __HAL_RCC_GPIOC_CLK_DISABLE();
    __HAL_RCC_GPIOD_CLK_DISABLE();
    __HAL_RCC_GPIOE_CLK_DISABLE();
    __HAL_RCC_GPIOF_CLK_DISABLE();
    __HAL_RCC_GPIOG_CLK_DISABLE();
}
void POST_SLEEP_PROCESSING(void)
{
     __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
}
#endif
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


