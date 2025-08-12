# 第二章 FreeRTOS中断管理

## 1. FreeRTOS中断配置项

1. configPRIO_BITS

此宏是用于辅助配置的宏，主要用于辅助配置宏 configKERNEL_INTERRUPT_PRIORITY和宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 的， 此宏应定义为 MCU 的 8 位优先级配置寄存器实际使用的位数，因为 STM32 只使用到了中断优先级配置寄存器的高 4 位，因此，此宏应配置为 4。

2. configLIBRARY_LOWEST_INTERRUPT_PRIORITY

此宏是用于辅助配置宏 configKERNEL_INTERRUPT_PRIORITY 的，此宏应设置为 MCU的最低优先等级，因为 STM32 只使用了中断优先级配置寄存器的高 4 位，因此 MCU 的最低优先等级就是 2^4-1=15，因此，此宏应配置为 15。

3. configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

此宏是用于辅助配置宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 的，此宏适用于配置 FreeRTOS 可管理的最高优先级的中断，此功能就是操作 BASEPRI 寄存器来实现的。此宏的值可以根据用户的实际使用场景来决定，本教程的配套例程源码全部将此宏配置为 5，即中断优先级高于 5 的中断不受 FreeRTOS 影响，如下图所示：

![屏幕截图 2025-08-11 212944.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/11-21-30-00-屏幕截图%202025-08-11%20212944.png)

4. configKERNEL_INTERRUPT_PRIORITY

此宏应配置为 MCU 的最低优先级在中断优先级配置寄存器中的值，在 FreeRTOS 的源码中，使用此宏将 SysTick 和 PenSV 的中断优先级设置为最低优先级。 因为 STM32 只使用了中断优先级配置寄存器的高 4 位，因此，此宏应配置为最低中断优先级在中断优先级配置寄存器高 4 位的表示，即(configLIBRARY_LOWEST_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))。

5. configMAX_SYSCALL_INTERRUPT_PRIORITY

此宏用于配置 FreeRTOS 可管理的最高优先级的中断，在 FreeRTOS 的源码中，使用此宏来打开和关闭中断。 因为 STM32 只使用了中断优先级配置寄存器的高 4 位，因此，此宏应配置为(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY<<(8-configPRIO_BITS))。

6. configMAX_API_CALL_INTERRUPT_PRIORITY

此宏为宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 的新名称，只被用在 FreeRTOS官方一些新的移植当中，此宏于宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 是等价的。

## 2. 临界区

临界区是指那些必须完整运行的区域，在临界区中的代码必须完整运行，不能被打断。例如一些使用软件模拟的通信协议，通信协议在通信时，必须严格按照通信协议的时序进行，不能被打断。 FreeRTOS 在进出临界区的时候，通过关闭和打开受 FreeRTOS 管理的中断，以保护临界区中的代码。 FreeRTOS 的源码中就包含了许多临界区的代码，这部分代码都是用临界区进行保护，用户在使用 FreeRTOS 编写应用程序的时候，也要注意一些不能被打断的操作，并为这部分代码加上临界区进行保护。

对于进出临界区，FreeRTOS 的源码中有四个相关的宏定义，分别为 taskENTER_CRITICAL() 、 taskENTER_CRITICAL_FROM_ISR() 、 taskEXIT_CRITICAL() 、taskEXIT_CRITICAL_FROM_ISR(x)， 这四个宏定义分别用于在中断和非中断中进出临界区， 定义代码如下所示：

```c
/* 进入临界区 */
#define taskENTER_CRITICAL() portENTER_CRITICAL()
#define portENTER_CRITICAL() vPortEnterCritical()
/* 中断中进入临界区 */
#define taskENTER_CRITICAL_FROM_ISR() portSET_INTERRUPT_MASK_FROM_ISR()
#define portSET_INTERRUPT_MASK_FROM_ISR() ulPortRaiseBASEPRI()
/* 退出临界区 */
#define taskEXIT_CRITICAL() portEXIT_CRITICAL()
#define portEXIT_CRITICAL() vPortExitCritical()
/* 中断中退出临界区 */
#define taskEXIT_CRITICAL_FROM_ISR(x) portCLEAR_INTERRUPT_MASK_FROM_ISR(x)
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) vPortSetBASEPRI(x)
```

## 3. FreeRTOS中断测试

### 3.1 程序流程图

本实验主要用于测试 FreeRTOS 打开和关闭中断对中断的影响，本实验设计了两个任务，这两个任务的功能如下表所示：

![屏幕截图 2025-08-11 215214.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/11-21-52-18-屏幕截图%202025-08-11%20215214.png)

本实验的程序流程图，如下图所示：

![屏幕截图 2025-08-11 220632.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/11-22-06-38-屏幕截图%202025-08-11%20220632.png)

### 3.2 FreeRTOS函数解析

#### 3.2.1 `portDISABLE_INTERRUPTS()`

功能：禁用处理器的全局中断，防止中断服务程序（ISR）打断当前正在执行的代码。

#### 3.2.2 `portENABLE_INTERRUPTS()`

功能：重新启用处理器的全局中断。

### 3.2 代码分析

#### 3.2.1 任务参数宏定义

```c
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
```

#### 3.2.2 start_task 任务实现

```c
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
```

#### 3.2.3 task1 任务实现

```c
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
```

#### 3.2.4 主函数

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


