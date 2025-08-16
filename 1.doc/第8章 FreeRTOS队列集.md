# 第八章 FreeRTOS队列集

## 1. 队列集简介

在使用队列进行任务之间的“沟通交流”时，一个队列只允许任务间传递的消息为同一种数据类型，如果需要在任务间传递不同数据类型的消息时，那么就可以使用队列集。 FreeRTOS提供的队列集功能可以对多个队列进行“监听”，只要被监听的队列中有一个队列有有效的消息，那么队列集的读取任务都可以读取到消息，如果读取任务因读取队列集而被阻塞，那么队列集将解除读取任务的阻塞。使用队列集的好处在于，队列集可以是的任务可以读取多个队列中的消息，而无需遍历所有待读取的队列，以确定具体读取哪一个队列。

使用队列集功能，需要在 FreeRTOSConfig.h 文件中将配置项 configUSE_QUEUE_SETS 配置为 1，来启用队列集功能。

## 2. FreeRTOS队列集相关函数

| `xQueueCreateSet()`            | 创建队列集              |
| ------------------------------ | ------------------ |
| `xQueueAddToSet()`             | 队列添加到队列集中          |
| `xQueueRemoveFromSet()`        | 从队列集中移除队列          |
| `xQueueSelectFromSet()`        | 获取队列集中有有效消息的队列     |
| `xQueueSelectFromSetFromISR()` | 在中断中获取队列集中有有效消息的队列 |

### 2.1 创建队列集

`xQueueCreateSet()` 函数用于创建一个新的 FreeRTOS 队列集。队列集本身不存储数据，它是一个“容器”，可以包含多个队列和/或信号量。

**函数原型：**

```c
QueueSetHandle_t xQueueCreateSet( const UBaseType_t uxEventQueueLength );
```

**参数说明：**

- `uxEventQueueLength`: 队列集中可以同时处于“可用”状态（即有数据可读或可以被获取）的事件（队列或信号量）的最大数量。这个参数指定了队列集内部用于管理事件的列表长度。

**返回值：**

- `QueueSetHandle_t`: 如果队列集成功创建，则返回一个队列集句柄（`QueueSetHandle_t` 类型），该句柄用于后续的队列集操作。
- `NULL`: 如果由于内存不足而无法创建队列集，则返回 `NULL`。

**注意事项：**

- 要使此函数可用，`configUSE_QUEUE_SETS` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 队列集是动态分配的，因此需要有可用的 FreeRTOS 堆内存。
- 创建队列集后，需要使用 `xQueueAddToSet()` 将队列或信号量添加到其中。

### 2.2 队列添加到队列集中

`xQueueAddToSet()` 函数用于将一个队列或信号量添加到已创建的队列集中。一旦添加到队列集，该队列或信号量将成为队列集的一部分，其事件（消息或信号量可用）将可以通过队列集进行监测。

**函数原型：**

```c
BaseType_t xQueueAddToSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet );
```

**参数说明：**

- `xQueueOrSemaphore`: 要添加到队列集中的队列句柄（`QueueHandle_t`）或信号量句柄（`SemaphoreHandle_t`）。由于 `QueueHandle_t` 和 `SemaphoreHandle_t` 都是 `QueueSetMemberHandle_t` 的别名，因此可以直接传入。
- `xQueueSet`: 目标队列集的句柄。

**返回值：**

- `pdPASS`: 成功将队列或信号量添加到队列集。
- `pdFAIL`: 队列或信号量已在队列集中，或者队列集已满。

**注意事项：**

- 要使此函数可用，`configUSE_QUEUE_SETS` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 一个队列或信号量只能被添加到 **一个** 队列集中。
- 在将队列或信号量添加到队列集之后，对该队列或信号量的直接接收/获取函数（例如 `xQueueReceive()`、`xSemaphoreTake()`）将不再正常工作，而应该通过队列集的相关函数来获取数据。

### 2.3 从队列集中移除队列

`xQueueRemoveFromSet()` 函数用于将一个队列或信号量从队列集中移除。移除后，可以再次使用其直接的接收/获取函数。

**函数原型：**

```c
BaseType_t xQueueRemoveFromSet( QueueSetMemberHandle_t xQueueOrSemaphore, QueueSetHandle_t xQueueSet );
```

**参数说明：**

- `xQueueOrSemaphore`: 要从队列集中移除的队列句柄或信号量句柄。
- `xQueueSet`: 目标队列集的句柄。

**返回值：**

- `pdPASS`: 成功从队列集中移除队列或信号量。
- `pdFAIL`: 队列或信号量不在队列集中。

**注意事项：**

- 要使此函数可用，`configUSE_QUEUE_SETS` 必须在 `FreeRTOSConfig.h` 中定义为 1。

### 2.4 获取队列集中有效消息的队列

`xQueueSelectFromSet()` 函数用于阻塞任务，直到队列集中有任何成员（队列或信号量）变为“可用”状态。当有成员可用时，该函数返回该可用成员的句柄。

**函数原型：**

```c
QueueSetMemberHandle_t xQueueSelectFromSet( QueueSetHandle_t xQueueSet, TickType_t xTicksToWait );
```

**参数说明：**

- `xQueueSet`: 要监测的队列集句柄。
- `xTicksToWait`: 如果队列集没有可用成员，任务愿意阻塞等待的节拍数。设置为 `portMAX_DELAY` 表示无限期等待，设置为 `0` 表示不等待。

**返回值：**

- `QueueSetMemberHandle_t`: 如果有成员可用，则返回该成员（队列或信号量）的句柄。
- `NULL`: 如果等待时间已到且没有成员可用。

**注意事项：**

- 要使此函数可用，`configUSE_QUEUE_SETS` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 一旦 `xQueueSelectFromSet()` 返回一个句柄，任务就可以对该句柄进行相应的读取操作（例如 `xQueueReceive()` 或 `xSemaphoreTake()`）

### 2.5 在中断中获取队列集中有有效消息的队列

### `xQueueSelectFromSetFromISR()`

`xQueueSelectFromSetFromISR()` 函数是 `xQueueSelectFromSet()` 的中断安全版本，用于在 ISR 中查询队列集中是否有可用成员。

**函数原型：**

```c
QueueSetMemberHandle_t xQueueSelectFromSetFromISR( QueueSetHandle_t xQueueSet );
```

**参数说明：**

- `xQueueSet`: 要查询的队列集句柄。

**返回值：**

- `QueueSetMemberHandle_t`: 如果有成员可用，则返回该成员的句柄。
- `NULL`: 如果没有成员可用。

**注意事项：**

- 要使此函数可用，`configUSE_QUEUE_SETS` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 此函数不会阻塞。如果返回非 `NULL` 句柄，则表示对应的队列或信号量中有数据，但 **不会** 自动执行上下文切换。如果需要唤醒等待该队列集的任务，通常需要在 ISR 外部手动处理或通过其他中断安全机制。

## 3. 队列集操作测试

### 3.1 队列配置

```c
/*----------------队列配置区-----------------*/
static QueueSetHandle_t xQueueSet; // 队列集
QueueHandle_t xQueuel; // 队列1
SemaphoreHandle_t xSemaphore; // 二值信号量
#define QUEUE_LENGTH    1 // 队列长度
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // 每条信息大小
#define SEMAPHORE_BINARY_LENGTH  1 // 二值信号量有效长度
#define QUEUESET_LENGTH (QUEUE_LENGTH+SEMAPHORE_BINARY_LENGTH) // 队列集长度
/*---------------------------------------------*/
```

### 3.2 任务配置

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

### 3.3 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10, 47, 220, 24, 24, "Queue Set", RED);
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
    // 创建队列集
    xQueueSet = xQueueCreateSet(QUEUESET_LENGTH);
    // 创建队列1
    xQueuel = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
    // 创建二值信号量
    xSemaphore = xSemaphoreCreateBinary();
    // 将队列1和二值信号量加入队列集
    xQueueAddToSet(xQueuel, xQueueSet);
    xQueueAddToSet(xSemaphore, xQueueSet);
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

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t key_value = 0;  // 改成 uint32_t
    while(1)
    {
        uint8_t temp = key_scan(0);  // 临时用 uint8_t 接收
        switch(temp)
        {
            case KEY0_PRES:
                key_value = temp;  // 赋值给 32 位变量
                xQueueSend(xQueuel, &key_value, portMAX_DELAY);
                break;
            case KEY1_PRES:
                xSemaphoreGive(xSemaphore);
                break;
            default:
                break;
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t queue_recv = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // 等待队列集中的队列接受到消息
        if(activate_member == xQueuel)
        {
            xQueueReceive(activate_member, &queue_recv, portMAX_DELAY);
                    printf("recv from queue1:%d\r\n", queue_recv);
        }
        else if(activate_member == xSemaphore)
        {
            xSemaphoreTake(activate_member, portMAX_DELAY); // 等待二值信号量释放
            printf("recv from semaphore\r\n");
        }
    }
}
/*---------------------------------------------*/
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

## 4. 队列集模拟事件标志位

### 4.1 队列配置

```c
/*----------------队列配置区-----------------*/
static QueueSetHandle_t xQueueSet; // 队列集
QueueHandle_t xQueue1; // 队列1
QueueHandle_t xQueue2; // 队列2
#define QUEUE_LENGTH 1 // 队列支持消息个数
#define QUEUE_ITEM_SIZE sizeof(uint32_t) // 队列消息大小
#define QUEUESET_LENGTH (2*QUEUE_LENGTH) // 队列集支持队列个数
#define EVENTBIT_0 (1<<0) // 事件位
#define EVENTBIT_1 (1<<1) 
#define EVENTBIT_ALL (EVENTBIT_0 | EVENTBIT_1)
/*---------------------------------------------*/
```

### 4.2 任务配置

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

### 4.3 任务实现

```c
// task1函数实现
void task1(void *pvParameters)
{
    uint32_t key_value =0;
    uint32_t eventbit_0 = EVENTBIT_0;
    uint32_t eventbit_1 = EVENTBIT_1;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xQueueSend(xQueue1, &eventbit_0, portMAX_DELAY); // 发送事件到队列1
                break;
            }
            case KEY1_PRES:
            {
                xQueueSend(xQueue2, &eventbit_1, portMAX_DELAY);
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
    uint32_t event_val = 0;
    uint32_t event_recv = 0;
    QueueSetMemberHandle_t activate_member = NULL;
    uint32_t task2_num = 0;
    while(1)
    {
        activate_member = xQueueSelectFromSet(xQueueSet, portMAX_DELAY); // 选择队列集中的一个队列
        xQueueReceive(activate_member, &event_recv, portMAX_DELAY);
        event_val |= event_recv; // 合并事件值
        lcd_show_xnum(182,110,event_val,1,16,0,BLUE);
        if(event_val == EVENTBIT_ALL) // 事件集中包含所有事件
        {
            event_val = 0; // 清空事件集
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
        }
    }
}
/*---------------------------------------------*/
            lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
        }
    }
}
```

### 4.4 主函数

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


