# 第十一章 FreeRTOS事件标志组

## 1. 事件标志组简介

事件标志组与信号量一样属于任务间同步的机制，但是信号量一般用于任务间的单事件同步，对于任务间的多事件同步，仅使用信号量就显得力不从心了。 FreeRTOS 提供的事件标志组可以很好的处理多事件情况下的任务同步。

1. 事件标志

事件标志是一个用于指示事件是否发生的布尔值，一个事件标志只有 0 或 1 两种状态， FreeRTOS 将多个事件标志储存在一个变量类型为 EventBits_t 变量中， 这个变量就是事件组。

2. 事件组

事件组是一组事件标志的集合，一个事件组就包含了一个 EventBites_t 数据类型的变量，变量类型 EventBits_t 的定义如下所示：

```c
typedef TickType_t EventBits_t;
#if ( configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#else
typedef uint32_t TickType_t;
#endif
#define configUSE_16_BIT_TICKS 0
```

从上面可以看出， EventBits_t 实际上是一个 16 位或 32 位无符号的数据类型。当configUSE_16_BIT_TICKS 配置为 0 时， EventBits_t 是一个 32 位无符号的数据类型；当configUSE_16_BIT_TICKS 配置为 1 时， EventBits_t 是一个 16 为无符号的数据类型。在本套教程的所有配套例程中，都将配置项 configUSE_16_BIT_TICKS 配置为 0，因此本文就以EventBits_t 为 32 位无符号数据类型为例进行讲解，对于另外一种情况，也是大同小异的。

虽然说使用了 32 位无符号的数据类型变量来存储事件标志，但这并不意味着，一个EventBits_t 数据类型的变量能够存储 32 个事件标志， FreeRTOS 将这个 EventBits_t 数据类型的变量拆分成两部分，其中低 24 位[23:0] （configUSE_16_BIT_TICKS 配置位 1 时，是低 8 位[7:0]）用于存储事件标志，而高 8 位[31:24]（configUSE_16_BIT_TICKS 配置位 1 时，依然是高 8 位[15:8]） 用作存储事件标志组的一些控制信息，也就是说一个事件组最多可以存储 24 个事件标志。 EventBits_t 数据类型变量的位使用情况如下图所示：

![屏幕截图 2025-08-18 152726.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-15-27-39-屏幕截图%202025-08-18%20152726.png)

从上图中可以看到，变量中低 24 位中的每一位都是一个事件标志，当某一位被置一时，就表示这一位对应的事件发生了。

## 2. 事件标志组相关函数

| 函数                              | 描述                 |
| ------------------------------- | ------------------ |
| `xEventGroupCreate()`           | 使用动态方式创建事件标志组      |
| `xEventGroupCreateStatic()`     | 使用静态方式创建事件标志组      |
| `vEventGroupDelete()`           | 删除事件标志组            |
| `xEventGroupWaitBits()`         | 等待事件标志位            |
| `xEventGroupSetBits()`          | 设置事件标志位            |
| `xEventGroupSetBitsFromISR()`   | 在中断中设置事件标志位        |
| `xEventGroupClearBits()`        | 清零事件标志位            |
| `xEventGroupClearBitsFromISR()` | 在中断中清零事件标志位        |
| `xEventGroupGetBits()`          | 获取事件组中各事件标志位的值     |
| `xEventGroupGetBitsFromISR()`   | 在中断中获取事件组中各事件标志位的值 |
| `xEventGroupSync()`             | 设置事件标志位，并等待事件标志位   |

### 2.1 创建事件标志组

- **`xEventGroupCreate()`**
  
  - **描述：** 动态方式创建事件标志组。事件标志组所需的内存从 FreeRTOS 堆中分配。创建时，所有事件标志位都被初始化为 0。
  - **函数原型：** `EventGroupHandle_t xEventGroupCreate( void );`
  - **返回值：** 如果事件标志组成功创建，则返回一个句柄（`EventGroupHandle_t` 类型）；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configSUPPORT_DYNAMIC_ALLOCATION` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xEventGroupCreateStatic()`**
  
  - **描述：** 静态方式创建事件标志组。事件标志组所需的内存由用户在编译时或运行时提供。创建时，所有事件标志位都被初始化为 0。
  - **函数原型：** `EventGroupHandle_t xEventGroupCreateStatic( StaticEventGroup_t *pxEventGroupBuffer );`
  - **参数：** `pxEventGroupBuffer` - 指向用户提供的 `StaticEventGroup_t` 结构体变量的指针，该结构体将用作事件标志组的控制块。
  - **返回值：** 如果事件标志组成功创建，则返回一个句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configSUPPORT_STATIC_ALLOCATION` 需在 `FreeRTOSConfig.h` 中定义为 1。用户需要自行管理 `pxEventGroupBuffer` 所指向的内存生命周期。

### 2.2 等待事件标志位

- **`xEventGroupWaitBits()`**
  - **描述：** 任务阻塞等待一个或多个事件标志位被设置。可以配置为等待所有指定位（逻辑 AND）或任意一个位（逻辑 OR）。还可以选择在成功接收后自动清除这些位。
  - **函数原型：** `EventBits_t xEventGroupWaitBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToWaitFor, const BaseType_t xClearOnExit, const BaseType_t xWaitForAllBits, TickType_t xTicksToWait );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToWaitFor`: 要等待的事件标志位的掩码（例如 `(1 << 0) | (1 << 2)`）。
    - `xClearOnExit`: 如果设置为 `pdTRUE`，当等待的位被满足后，它们会被自动清除；如果设置为 `pdFALSE`，则不会自动清除。
    - `xWaitForAllBits`: 如果设置为 `pdTRUE`，任务将等待所有 `uxBitsToWaitFor` 中的位都设置；如果设置为 `pdFALSE`，任务将等待 `uxBitsToWaitFor` 中任意一个位设置。
    - `xTicksToWait`: 如果事件标志位未被设置，任务愿意阻塞等待的节拍数。
  - **返回值：** 调用函数时事件组中所有标志位的当前值。可以通过检查返回值来确定哪些位被设置。

### 2.3 设置事件标志位

- **`xEventGroupSetBits()`**
  
  - **描述：** 设置事件标志组中的一个或多个标志位。设置位可能会导致一个或多个等待该事件组的任务解除阻塞。
  - **函数原型：** `EventBits_t xEventGroupSetBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToSet`: 要设置的事件标志位的掩码。
  - **返回值：** 调用函数后事件组中所有标志位的当前值。

- **`xEventGroupSetBitsFromISR()`**
  
  - **描述：** 在中断中设置事件标志组中的一个或多个标志位。此函数不会阻塞。
  - **函数原型：** `BaseType_t xEventGroupSetBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToSet`: 要设置的事件标志位的掩码。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

### 2.4 清零事件标志位

- **`xEventGroupClearBits()`**
  
  - **描述：** 清零（设置为 0）事件标志组中的一个或多个标志位。
  - **函数原型：** `EventBits_t xEventGroupClearBits( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToClear`: 要清零的事件标志位的掩码。
  - **返回值：** 调用函数后事件组中所有标志位的当前值。

- **`xEventGroupClearBitsFromISR()`**
  
  - **描述：** 在中断中清零事件标志组中的一个或多个标志位。
  - **函数原型：** `BaseType_t xEventGroupClearBitsFromISR( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToClear );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToClear`: 要清零的事件标志位的掩码。
  - **返回值：** 总是返回 `pdPASS`。此函数不会导致任务切换，因此没有 `pxHigherPriorityTaskWoken` 参数。

### 2.5 获取事件标志位的值

- **`xEventGroupGetBits()`**
  
  - **描述：** 获取事件组中所有事件标志位的当前值。此函数不会阻塞。
  - **函数原型：** `EventBits_t xEventGroupGetBits( EventGroupHandle_t xEventGroup );`
  - **参数：** `xEventGroup` - 目标事件标志组的句柄。
  - **返回值：** 事件组中所有标志位的当前值。

- **`xEventGroupGetBitsFromISR()`**
  
  - **描述：** 在中断中获取事件组中所有事件标志位的当前值。
  - **函数原型：** `EventBits_t xEventGroupGetBitsFromISR( EventGroupHandle_t xEventGroup );`
  - **参数：** `xEventGroup` - 目标事件标志组的句柄。
  - **返回值：** 事件组中所有标志位的当前值。

### 2.6 同步任务：`xEventGroupSync()`

- **描述：** `xEventGroupSync()` 是一个同步原语，它结合了设置位和等待位的功能。一个或多个任务可以调用此函数来同步它们的执行。当所有参与的任务都设置了各自指定的位后，它们才会被解除阻塞。
  - **函数原型：** `EventBits_t xEventGroupSync( EventGroupHandle_t xEventGroup, const EventBits_t uxBitsToSet, const EventBits_t uxBitsToWaitFor, TickType_t xTicksToWait );`
  - **参数：**
    - `xEventGroup`: 目标事件标志组的句柄。
    - `uxBitsToSet`: 调用任务要设置的标志位。
    - `uxBitsToWaitFor`: 要等待的所有任务共同设置的标志位。
    - `xTicksToWait`: 如果等待的位未被满足，任务愿意阻塞等待的节拍数。
  - **返回值：** 成功返回 `uxBitsToWaitFor` 中所有位都被设置时的事件组值；超时则返回 0。
  - **注意事项：** 这是一个非常强大的同步机制，可以用于任务屏障（Task Barrier）的实现。

### 2.7 删除事件标志组

- **`vEventGroupDelete()`**
  - **描述：** 删除一个事件标志组，并释放其占用的内存（如果它是动态创建的）。
  - **函数原型：** `void vEventGroupDelete( EventGroupHandle_t xEventGroup );`
  - **参数：** `xEventGroup` - 要删除的事件标志组句柄。
  - **注意事项：** 只能删除动态创建的事件标志组。删除后，不应再使用该句柄。

## 3. 事件标志组测试

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

// TASK2配置
#define TASK2_PRIO      4                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      3                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

EventGroupHandle_t Event_handle;
#define EVENT0_BIT (1 << 0)
#define EVENT1_BIT (1 << 1)
#define EVENTALL_BIT (EVENT0_BIT | EVENT1_BIT)
/*---------------------------------------------*/
```

### 3.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Event Group", RED);
    lcd_draw_rectangle(5, 130, 234, 314, BLACK);     
    lcd_show_string(30, 110, 220, 16, 16, "Event Group Value: 0", BLUE);
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
    // 创建事件标志组
    Event_handle = xEventGroupCreate();
    // 创建TASK1任务
    xTaskCreate((TaskFunction_t)task1,  
                (const char*)"task1",       
                (uint16_t)TASK1_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK1_PRIO,    
                (TaskHandle_t*)&Task1Task_Handler);
    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    // 创建TASK3任务
    xTaskCreate((TaskFunction_t)task3,  
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT0_BIT);
                break;
            }
            case KEY1_PRES:
            {
                xEventGroupSetBits((EventGroupHandle_t)Event_handle, (EventBits_t)EVENT1_BIT);
                break;
            }
            default:break;
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t task2_value;
    while(1)
    {
        xEventGroupWaitBits((EventGroupHandle_t)Event_handle,// 等待的事件标志组
                            (EventBits_t)EVENTALL_BIT,       // 等待的事件标志
                            (BaseType_t)pdTRUE,              // 函数退出时清零等待的事件
                            (BaseType_t)pdTRUE,              // 等待 等待事件中的所有事件
                            (TickType_t)portMAX_DELAY);      // 等待事件
        lcd_fill(6,131,233,313,lcd_discolor[++task2_value%11]);
        vTaskDelay(10);
    } 
}

// task3函数实现
void task3(void *pvParameters)
{
    EventBits_t event_value;
    while(1)
    {
        event_value = xEventGroupGetBits((EventGroupHandle_t)Event_handle);  /* 获取的事件标志组句柄 */
        lcd_show_xnum(132,110,event_value,1,16,0,BLUE);
        vTaskDelay(10);
    }
}

/*---------------------------------------------*/
```

### 3.3 主函数

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
