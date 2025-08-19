# 第十二章 FreeRTOS任务通知

## 1. 任务通知

在 FreeRTOS 中，每一个任务都有两个用于任务通知功能的数组，分别为任务通知数组和任务通知状态数组。其中任务通知数组中的每一个元素都是一个 32 位无符号类型的通知值；而任务通知状态数组中的元素则表示与之对应的任务通知的状态。

任务通知数组中的 32 位无符号通知值，用于任务到任务或中断到任务发送通知的“媒介”。当通知值为 0 时，表示没有任务通知；当通知值不为 0 时，表示有任务通知，并且通知值就是通知的内容。

任务通知状态数组中的元素，用于标记任务通知数组中通知的状态，任务通知有三种状态，分别为未等待通知状态、等待通知状态和等待接收通知状态。其中未等待通知状态为任务通知的复位状态；当任务在没有通知的时候接收通知时，在任务阻塞等待任务通知的这段时间内，任务所等待的任务通知就处于等待通知状态；当有其他任务向任务发送通知，但任务还未接收这一通知的这段期间内，任务通知就处于等待接收通知状态。

任务通知功能所使用到的任务通知数组和任务通知状态数组为任务控制块中的成员变量，因此任务通知的传输是直接传出到任务中的，不同通过任务的通讯对象（队列、事件标志组和信号量就属于通讯对象） 这个间接的方式。 间接通讯示意图如下所示：

![屏幕截图 2025-08-18 164412.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-16-44-22-屏幕截图%202025-08-18%20164412.png)

任务通知则是直接地往任务中发送通知，直接通讯示意图如下所示：

![屏幕截图 2025-08-18 164436.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-16-44-41-屏幕截图%202025-08-18%20164436.png)

### 1.1 任务通知的优势

使用任务通知向任务发送事件或数据比使用队列、事件标志组或信号量快得多；并且使用任务通知代替队列、事件标志组或信号量，可以节省大量的内存，这是因为每个通讯对象在使用之前都需要被创建，而任务通知功能中的每个通知只需要在每个任务中占用固定的 5 字节内存

### 1.2 任务通知的缺点

虽然任务通知功能相比通讯对象，有着更快、占用内存少的优点，但是任务通知功能并不能适用于所有情况，例如以下列出的几种情况：

1. 发送事件或数据到中断

通讯对象可以发送事件或数据从中断到任务，或从任务到中断，但是由于任务通知依赖于任务控制块中的两个成员变量，并且中断不是任务，因此任务通知功能并不适用于从任务往中断发送事件或数据的这种情况，但是任务通知功能可以在任务之间或从中断到任务发送事件或数据。

2. 存在多个接收任务

通讯对象可以被已知通讯对象句柄的任意多个任务或中断访问（发送或接收），但任务通知是直接发送事件或数据到指定接收任务的，因传输的事件或数据只能由接收任务处理。然而在实际中很少受到这种情况的限制，因为，虽然多个任务和中断发送事件或数据到一个通讯对象是很常见的，但很少出现多个任务或中断接收同一个通讯对象的情况。

3. 缓冲多个数据项

通讯对象中的队列是可以一次性保存多个已经被发送到队列，但还未被接收的事件或数据的，也就是说，通讯对象有着一定的缓冲多个数据的能力，但是任务通知是通过更新任务通知值来发送事件或数据的，一个任务通知值只能保存一次。

4. 广播到多个任务

通讯对象中的事件标志组是可以将一个事件同时发送到多个任务中的，但任务通知只能是被指定的一个接收任务接收并处理。

5. 阻塞等待接收任务

当通讯对象处于暂时无法写入的状态（例如队列已满，此时无法再向队列写入消息）时，发送任务是可以选择阻塞等待接收任务接收，但是任务因尝试发送任务通知到已有任务通知但还未处理的任务而进行阻塞等待的。但是任务通知也很少在实际情况中收到这种情况的限制。

## 3. 任务通知相关函数

### 3.1 基本任务通知相关函数

这些函数操作任务的默认（索引为 0）通知值。

| 函数                             | 描述                     |
| ------------------------------ | ---------------------- |
| `xTaskNotify()`                | 发送任务通知                 |
| `xTaskNotifyAndQuery()`        | 发送任务通知，并获取旧的通知值        |
| `xTaskNotifyGive()`            | 递增接收任务的通知值             |
| `xTaskNotifyFromISR()`         | 在中断中发送任务通知             |
| `xTaskNotifyAndQueryFromISR()` | 在中断中发送任务通知，并获取旧的通知值    |
| `vTaskNotifyGiveFromISR()`     | 在中断中递增接收任务的通知值         |
| `ulTaskNotifyTake()`           | 接收任务通知 (作为二值信号量或计数信号量) |
| `xTaskNotifyWait()`            | 接收任务通知 (作为事件标志组或直接值)   |

#### 1. 发送任务通知

- **`xTaskNotify()`**
  
  - **描述：** 向指定任务发送通知，并可以对接收任务的通知值执行不同的操作（如覆盖、设置位、递增）。
  - **函数原型：** `BaseType_t xTaskNotify( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction );`
  - **参数：**
    - `xTaskToNotify`: 接收通知的任务句柄。
    - `ulValue`: 要发送的值，其具体含义取决于 `eAction`。
    - `eAction`: 指定如何使用 `ulValue` 更新接收任务的通知值（例如：`eSetBits` 设置位, `eIncrement` 递增, `eSetValueWithOverwrite` 覆盖等）。
  - **返回值：**
    - `pdPASS`: 成功发送通知。
    - `pdFAIL`: 通常不会失败，除非 `xTaskToNotify` 为 `NULL` 或通知机制未使能。

- **`xTaskNotifyAndQuery()`**
  
  - **描述：** 与 `xTaskNotify()` 类似，但它会返回接收任务通知值被更新前的值。
  - **函数原型：** `BaseType_t xTaskNotifyAndQuery( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue );`
  - **参数：**
    - 前三个参数与 `xTaskNotify()` 相同。
    - `pulPreviousNotificationValue`: 指向 `uint32_t` 变量的指针，用于接收通知值被更新前的值。如果不需要旧值，可以传入 `NULL`。
  - **返回值：** 同 `xTaskNotify()`。

- **`xTaskNotifyGive()`**
  
  - **描述：** 递增接收任务的通知值。这模拟了信号量的 `Give` 操作，每次调用都会使通知值加 1，直到其达到最大值 (`portMAX_DELAY`)。
  - **函数原型：** `BaseType_t xTaskNotifyGive( TaskHandle_t xTaskToNotify );`
  - **参数：** `xTaskToNotify` - 接收通知的任务句柄。
  - **返回值：** `pdPASS` 表示成功递增通知值。

#### 2. 中断中发送任务通知

这些函数是其对应非ISR版本的变体，用于在中断上下文安全地发送通知。它们通过 `pxHigherPriorityTaskWoken` 参数指示是否需要进行上下文切换。

- **`xTaskNotifyFromISR()`**
  
  - **描述：** 在中断中向指定任务发送通知，并对接收任务的通知值执行操作。
  - **函数原型：** `BaseType_t xTaskNotifyFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xTaskToNotify`, `ulValue`, `eAction`: 同 `xTaskNotify()`。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

- **`xTaskNotifyAndQueryFromISR()`**
  
  - **描述：** 在中断中向指定任务发送通知，并获取通知值被更新前的值。
  - **函数原型：** `BaseType_t xTaskNotifyAndQueryFromISR( TaskHandle_t xTaskToNotify, uint32_t ulValue, eNotifyAction eAction, uint32_t *pulPreviousNotificationValue, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 同 `xTaskNotifyFromISR()` 和 `xTaskNotifyAndQuery()` 的组合。

- **`vTaskNotifyGiveFromISR()`**
  
  - **描述：** 在中断中递增接收任务的通知值。
  - **函数原型：** `BaseType_t vTaskNotifyGiveFromISR( TaskHandle_t xTaskToNotify, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xTaskToNotify`: 接收通知的任务句柄。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

#### 3. 接收任务通知

- **`ulTaskNotifyTake()`**
  
  - **描述：** 接收任务通知，并可以使其表现为二值信号量或计数信号量。它会清零通知计数器，并返回其旧值。
  - **函数原型：** `uint32_t ulTaskNotifyTake( BaseType_t xClearCountOnExit, TickType_t xTicksToWait );`
  - **参数：**
    - `xClearCountOnExit`: 如果设置为 `pdTRUE`，则在返回前将通知值重置为 0。如果设置为 `pdFALSE`，则仅将通知值减 1（类似于计数信号量）。
    - `xTicksToWait`: 如果没有待处理的通知，任务愿意阻塞等待的节拍数。
  - **返回值：** 接收通知前任务的通知值。

- **`xTaskNotifyWait()`**
  
  - **描述：** 接收任务通知，并可以使其表现为事件标志组或直接值。
  - **函数原型：** `BaseType_t xTaskNotifyWait( uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );`
  - **参数：**
    - `ulBitsToClearOnEntry`: 进入函数时，哪些位将被清零（设置为 0）。
    - `ulBitsToClearOnExit`: 退出函数时（收到通知后），哪些位将被清零。
    - `pulNotificationValue`: 指向 `uint32_t` 变量的指针，用于存储接收到的通知值。
    - `xTicksToWait`: 如果没有待处理的通知，任务愿意阻塞等待的节拍数。
  - **返回值：** `pdPASS` 表示成功接收通知；`pdFALSE` 表示超时未收到通知。

---

### 3.2 带索引的任务通知相关函数

FreeRTOS v10.0.0 引入了带索引的任务通知，允许每个任务拥有一个通知值数组（由 `configTASK_NOTIFICATION_ARRAY_ENTRIES` 定义数组大小，默认为 1），而不是单个通知值。这使得一个任务可以同时等待来自多个源的通知，而无需创建多个信号量或队列。

这些函数的命名通常在基本函数名后加上 `Indexed` 后缀。它们都多了一个 `uxIndexToNotify` 或 `uxIndexToWait` 参数，用于指定操作哪个通知值数组元素。

- **`xTaskNotifyIndexed()`**
- **`xTaskNotifyAndQueryIndexed()`**
- **`xTaskNotifyGiveIndexed()`**
- **`xTaskNotifyIndexedFromISR()`**
- **`xTaskNotifyAndQueryIndexedFromISR()`**
- **`vTaskNotifyGiveIndexedFromISR()`**
- **`ulTaskNotifyTakeIndexed()`**
- **`xTaskNotifyWaitIndexed()`**

这些带索引的函数的功能和参数与它们对应的基本任务通知函数类似，只是多了一个 `uxIndex` 参数来指定操作哪个通知槽。

**示例：`xTaskNotifyIndexed()`**

- **函数原型：** `BaseType_t xTaskNotifyIndexed( TaskHandle_t xTaskToNotify, UBaseType_t uxIndexToNotify, uint32_t ulValue, eNotifyAction eAction );`
- **参数：**
  - `xTaskToNotify`: 接收通知的任务句柄。
  - `uxIndexToNotify`: 要通知的通知值数组的索引。
  - `ulValue`: 要发送的值，其具体含义取决于 `eAction`。
  - `eAction`: 指定如何更新通知值。

**示例：`xTaskNotifyWaitIndexed()`**

- **函数原型：** `BaseType_t xTaskNotifyWaitIndexed( UBaseType_t uxIndexToWait, uint32_t ulBitsToClearOnEntry, uint32_t ulBitsToClearOnExit, uint32_t *pulNotificationValue, TickType_t xTicksToWait );`
- **参数：**
  - `uxIndexToWait`: 要等待的通知值数组的索引。
  - 其他参数同 `xTaskNotifyWait()`。

## 4. 任务通知模拟二值信号量测试

### 4.1 任务配置

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
/*---------------------------------------------*/
```

### 4.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_draw_rectangle(5, 130, 234, 314, BLACK); 
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

    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        if(Task2Task_Handler != NULL)
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES:
                {
                    xTaskNotifyGive((TaskHandle_t)Task2Task_Handler); // 发送通知给TASK2
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t nofify_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        nofify_value = ulTaskNotifyTake((BaseType_t)pdTRUE, (TickType_t)portMAX_DELAY); // 等待通知
        if(nofify_value != 0)
        {
            lcd_fill(6,131,233,313,lcd_discolor[++task2_value%11]);
        }
    }
}
/*---------------------------------------------*/
```

### 4.3 主函数

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

## 5. 任务通知模拟消息邮箱测试

### 5.1 任务配置

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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*---------------------------------------------*/
```

### 5.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_draw_rectangle(5, 125, 234, 314, BLACK);
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

    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        key_value = key_scan(0);
        if((Task2Task_Handler != NULL) && (key_value != 0))
        {
            // 接受任务通知的句柄 要更新的bit位 更新方式为覆写
            xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)key_value, (eNotifyAction)eSetValueWithOverwrite);
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t nofify_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        xTaskNotifyWait((uint32_t)0x00000000,      // 进入函数时，不清除任务通知值
                        (uint32_t)0xFFFFFFFF,      // 函数退出时，清除所有任务通知值
                        (uint32_t*)&nofify_value,  // 等待接收通知值
                        (TickType_t)portMAX_DELAY);
        switch(nofify_value)
        {
            case KEY0_PRES:
            {
                lcd_fill(6, 126, 233, 313, lcd_discolor[++task2_value % 11]);
                break;
            }
            case KEY1_PRES:
            {
                LED0_TOGGLE();
                break;
            }
            case WKUP_PRES:
            {
                LED1_TOGGLE();
                break;
            }
            default:break;
        }
    }
}
/*---------------------------------------------*/
```

### 5.3 主函数

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

## 6. 任务通知模拟计数信号量测试

### 6.1 任务配置

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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);
/*---------------------------------------------*/
```

### 6.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_show_string(54, 111, 200, 16, 16, "Notify Value:  0", BLUE);
    lcd_draw_rectangle(5, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 234, 130, BLACK);
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

    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        key_value = key_scan(0);
        if(Task2Task_Handler != NULL)
        {
            switch(key_value)
            {
                case KEY0_PRES:
                {
                    xTaskNotifyGive((TaskHandle_t)Task2Task_Handler); // 发送通知给TASK2
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t nofify_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        nofify_value = ulTaskNotifyTake((BaseType_t)pdTRUE, (TickType_t)portMAX_DELAY); // 等待通知
        lcd_show_xnum(166,111,nofify_value-1,2,16,0,BLUE);
        lcd_fill(6,131,233,313,lcd_discolor[++task2_value%11]);
        vTaskDelay(1000);
    }
}
/*---------------------------------------------*/
```

### 6.3 主函数

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

## 7. 任务通知模拟事件标志组

### 7.1 任务配置

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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

#define EventBit_0 (1<<0)
#define EventBit_1 (1<<1)
#define EventBit_ALL (EventBit_0 | EventBit_1)
/*---------------------------------------------*/
```

### 7.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Task Notify Demo",RED);
    lcd_show_string(30,110,220,16,16,"Event Group Value: 0",BLUE);
    lcd_draw_rectangle(5,130,234,314,BLACK);            
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

    // 创建TASK2任务
    xTaskCreate((TaskFunction_t)task2,  
                (const char*)"task2",       
                (uint16_t)TASK2_STK_SIZE,   
                (void*)NULL,                
                (UBaseType_t)TASK2_PRIO,    
                (TaskHandle_t*)&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

void task1(void *pvParameters)
{
    uint8_t key_value =0;
    while(1)
    {
        if(Task1Task_Handler != NULL)
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES:
                {
                    xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)EventBit_0, (eNotifyAction)eSetBits);
                    break;
                }
                case KEY1_PRES:
                {
                    xTaskNotify((TaskHandle_t)Task2Task_Handler, (uint32_t)EventBit_1, (eNotifyAction)eSetBits);
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t notify_value = 0;
    uint32_t event_value = 0;
    uint32_t task2_value = 0;
    while(1)
    {
        xTaskNotifyWait((uint32_t)0x00000000,      // 进入函数时，不清除任务通知值
                        (uint32_t)0xFFFFFFFF,      // 函数退出时，清除所有任务通知值
                        (uint32_t*)&notify_value,  // 等待接收通知值
                        (TickType_t)portMAX_DELAY);
        if(notify_value & EventBit_0)
        {
            event_value |= EventBit_0;
        }
        else if(notify_value & EventBit_1)
        {
            event_value |= EventBit_1;
        }
        lcd_show_xnum(182,110,event_value,1,16,0,BLUE);
        if(event_value == EventBit_ALL)
        {
            event_value = 0;
             lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_value % 11]);
        }
    }
}
/*---------------------------------------------*/
```

### 7.3 主函数

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
