# 第九章 FreeRTOS信号量

## 1. 信号量简介

信号量是一种解决同步问题的机制，可以实现对共享资源的有序访问。其中，“同步”指的是任务间的同步，即信号量可以使得一个任务等待另一个任务完成某件事情后，才继续执行；而“有序访问”指的是对被多任务或中断访问的共享资源（如全局变量）的管理，当一个任务在访问（读取或写入）一个共享资源时，信号量可以防止其他任务或中断在这期间访问（读取或写入）这个共享资源。

举一个例子，假设某个停车场有 100 个停车位（共享资源），这个 100 个停车位对所有人（访问共享资源的任务或中断）开放。如果有一个人要在这个停车场停车，那么就需要先判断这个停车场是否还有空车位（判断信号量是否有资源）， 如果此时停车场正好有空车位（信号量有资源），那么就可以直接将车开入空车位进行停车（获取信号量成功）， 如果此时停车场已经没有空车位了（信号量没有资源），那么这个人可以选择不停车（获取信号量失败），也可以选择等待（任务阻塞）其他人将车开出停车场（释放信号量资源）， 让后再将车停入空车位。

在上面的这个例子中，空车位的数量相当于信号量的资源数，获取信号量相当于占用了空车位，而释放信号量就相当于让出了占用的空车位。信号量用于管理共享资源的场景相当于对共享资源上了个锁，只有任务成功获取到了锁的钥匙，才能够访问这个共享资源，访问完共享资源后还得归还钥匙，当然钥匙可以不只一把，即信号量可以有多个资源。

## 2. 二值信号量

前面说过，信号量是基于队列实现的，二值信号量也不例外，二值信号量实际上就是一个队列长度为 1 的队列，在这种情况下，队列就只有空和满两种情况，这不就是二值情况吗？ 二值信号量通常用于互斥访问或任务同步， 与互斥信号量比较类似，但是二值信号量有可能会导

致优先级翻转的问题。优先级翻转问题指的是，当一个高优先级任务因获取一个被低优先级任务获取而处于没有资源状态的二值信号量时，这个高优先级的任务将被阻塞，直到低优先级的任务释放二值信号量，而在这之前，如果有一个优先级介于这个高优先级任务和低优先级任务之间的任务就绪，那么这个中等优先级的任务就会抢占低优先级任务的运行， 这么一来，这三个任务中优先级最高的任务反而要最后才运行，这就是二值信号量带来的优先级翻转问题，用户在实际开发中要注意这种问题。

和队列一样，在获取二值信号量的时候，允许设置一个阻塞超时时间，阻塞超时时间是当任务获取二值信号量时，由于二值信号量处于没有资源的状态，而导致任务进入阻塞状态的最大系统时钟节拍数。如果多个任务同时因获取同一个处于没有资源状态的二值信号量而被阻塞，那么在二值信号量有资源的时候，这些阻塞任务中优先级高的任务将优先获得二值信号量的资源并解除阻塞。

二值信号量是最简单的信号量形式，它只有两种状态：可用（1）和不可用（0），通常用于任务同步或互斥访问。

| 函数                               | 描述            |
| -------------------------------- | ------------- |
| `xSemaphoreCreateBinary()`       | 使用动态方式创建二值信号量 |
| `xSemaphoreCreateBinaryStatic()` | 使用静态方式创建二值信号量 |
| `xSemaphoreTake()`               | 获取信号量         |
| `xSemaphoreTakeFromISR()`        | 在中断中获取信号量     |
| `xSemaphoreGive()`               | 释放信号量         |
| `xSemaphoreGiveFromISR()`        | 在中断中释放信号量     |
| `vSemaphoreDelete()`             | 删除信号量         |

### 2.1 创建二值信号量

- **`xSemaphoreCreateBinary()`**
  
  - **描述：** 使用动态方式创建二值信号量。信号量所需的内存将从 FreeRTOS 堆中分配。创建时，信号量会被初始化为“空”（不可用）状态。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateBinary( void );`
  - **返回值：** 如果信号量成功创建，则返回一个信号量句柄（`SemaphoreHandle_t` 类型）；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_COUNTING_SEMAPHORES` 和 `configSUPPORT_DYNAMIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xSemaphoreCreateBinaryStatic()`**
  
  - **描述：** 使用静态方式创建二值信号量。信号量所需的内存由用户在编译时或运行时提供。创建时，信号量会被初始化为“空”（不可用）状态。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateBinaryStatic( StaticSemaphore_t *pxSemaphoreBuffer );`
  - **参数：** `pxSemaphoreBuffer` - 指向用户提供的 `StaticSemaphore_t` 结构体变量的指针，该结构体将用作信号量的控制块。
  - **返回值：** 如果信号量成功创建，则返回一个信号量句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_COUNTING_SEMAPHORES` 和 `configSUPPORT_STATIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。

### 2.2 获取信号量

- **`xSemaphoreTake()`**
  
  - **描述：** 尝试获取一个二值信号量。如果信号量可用，则获取成功，信号量变为不可用状态。如果信号量不可用，调用任务将进入阻塞状态（如果指定了等待时间），直到信号量可用或超时。
  - **函数原型：** `BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );`
  - **参数：**
    - `xSemaphore`: 要获取的信号量句柄。
    - `xTicksToWait`: 如果信号量不可用，任务愿意阻塞等待的节拍数。设置为 `portMAX_DELAY` 表示无限期等待，设置为 `0` 表示不等待。
  - **返回值：**
    - `pdPASS`: 成功获取信号量。
    - `pdFAIL`: 信号量不可用，且等待时间已到或为 `0` 未等待。
  - **注意事项：** 用于任务之间的同步（如事件通知）或实现互斥（如保护共享资源）。

- **`xSemaphoreTakeFromISR()`**
  
  - **描述：** 在中断服务例程 (ISR) 中尝试获取一个二值信号量。此函数不会阻塞。
  - **函数原型：** `BaseType_t xSemaphoreTakeFromISR( SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xSemaphore`: 要获取的信号量句柄。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。此变量应初始化为 `pdFALSE`。如果调用此函数导致更高优先级的任务解除阻塞（即需要进行上下文切换），则该变量将被设置为 `pdTRUE`。
  - **返回值：**
    - `pdPASS`: 成功获取信号量。
    - `pdFALSE`: 信号量不可用。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`（或类似的宏）来触发上下文切换。

### 2.3 释放信号量

- **`xSemaphoreGive()`**
  
  - **描述：** 释放一个二值信号量，使其变为可用状态。如果当前有任务因等待此信号量而阻塞，则优先级最高的阻塞任务将被解除阻塞。
  - **函数原型：** `BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );`
  - **参数：** `xSemaphore` - 要释放的信号量句柄。
  - **返回值：**
    - `pdPASS`: 成功释放信号量。
    - `pdFAIL`: 信号量已处于可用状态（通常表示逻辑错误，二值信号量不能被多次释放）。
  - **注意事项：** 当用于互斥时，通常由持有信号量的任务释放。当用于同步时，通常由事件发生者释放。

- **`xSemaphoreGiveFromISR()`**
  
  - **描述：** 在中断服务例程 (ISR) 中释放一个二值信号量。
  - **函数原型：** `BaseType_t xSemaphoreGiveFromISR( SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xSemaphore`: 要释放的信号量句柄。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。此变量应初始化为 `pdFALSE`。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
  - **返回值：**
    - `pdPASS`: 成功释放信号量。
    - `pdFALSE`: 信号量已处于可用状态。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

### 2.4 删除信号量

- **`vSemaphoreDelete()`**
  - **描述：** 删除一个二值信号量，并释放其占用的内存（如果它是动态创建的）。
  - **函数原型：** `void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );`
  - **参数：** `xSemaphore` - 要删除的信号量句柄。
  - **注意事项：** 只能删除动态创建的信号量。删除后，不应再使用该句柄。

## 3. 二值信号量操作测试

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
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

SemaphoreHandle_t BinarySema; // 二值信号量
/*---------------------------------------------*/
```

### 3.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Binary Demo",RED);
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
    // 创建二值信号量
    BinarySema = xSemaphoreCreateBinary();
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
    uint8_t key_value = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xSemaphoreGive(BinarySema); // 释放二值信号量
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
    uint32_t task2_num = 0;
    while(1)
    {
        xSemaphoreTake(BinarySema, portMAX_DELAY); // 获取二值信号量
        lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);
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

## 4. 计数型信号量

计数型信号量与二值信号量类似， 二值信号量相当于队列长度为 1 的队列，因此二值信号量只能容纳一个资源，这也是为什么命名为二值信号量，而计数型信号量相当于队列长度大于0 的队列，因此计数型信号量能够容纳多个资源，这是在计数型信号量被创建的时候确定的。计数型信号量通常用于一下两种场合：

1. 事件计数

在这种场合下，每次事件发生后，在事件处理函数中释放计数型信号量（计数型信号量的资源数加 1），其他等待事件发生的任务获取计数型信号量（计数型信号量的资源数减 1），这么一来等待事件发生的任务就可以在成功获取到计数型信号量之后执行相应的操作。在这种场合下，计数型信号量的资源数一般在创建时设置为 0。

2. 资源管理

在这种场合下，计数型信号量的资源数代表着共享资源的可用数量，例如前面举例中停车场中的空车位。一个任务想要访问共享资源，就必须先获取这个共享资源的计数型信号量，之后在成功获取了计数型信号量之后，才可以对这个共享资源进行访问操作，当然，在使用完共享资源后也要释放这个共享资源的计数型信号量。在这种场合下，计数型信号量的资源数一般在创建时设置为受其管理的共享资源的最大可用数量。

| 函数                                 | 描述             |
| ---------------------------------- | -------------- |
| `xSemaphoreCreateCounting()`       | 使用动态方式创建计数型信号量 |
| `xSemaphoreCreateCountingStatic()` | 使用静态方式创建计数型信号量 |
| `xSemaphoreTake()`                 | 获取信号量          |
| `xSemaphoreTakeFromISR()`          | 在中断中获取信号量      |
| `xSemaphoreGive()`                 | 释放信号量          |
| `xSemaphoreGiveFromISR()`          | 在中断中释放信号量      |
| `vSemaphoreDelete()`               | 删除信号量          |

### 4.1 创建计数型信号量

- **`xSemaphoreCreateCounting()`**
  
  - **描述：** 使用动态方式创建计数型信号量。信号量所需的内存将从 FreeRTOS 堆中分配。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateCounting( UBaseType_t uxMaxCount, UBaseType_t uxInitialCount );`
  - **参数：**
    - `uxMaxCount`: 信号量的最大计数值。当信号量计数值达到此值时，不能再进行 `Give` 操作。
    - `uxInitialCount`: 信号量的初始计数值。
  - **返回值：** 如果信号量成功创建，则返回一个信号量句柄（`SemaphoreHandle_t` 类型）；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_COUNTING_SEMAPHORES` 和 `configSUPPORT_DYNAMIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xSemaphoreCreateCountingStatic()`**
  
  - **描述：** 使用静态方式创建计数型信号量。信号量所需的内存由用户在编译时或运行时提供。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateCountingStatic( UBaseType_t uxMaxCount, UBaseType_t uxInitialCount, StaticSemaphore_t *pxSemaphoreBuffer );`
  - **参数：**
    - `uxMaxCount`: 信号量的最大计数值。
    - `uxInitialCount`: 信号量的初始计数值。
    - `pxSemaphoreBuffer`: 指向用户提供的 `StaticSemaphore_t` 结构体变量的指针，该结构体将用作信号量的控制块。
  - **返回值：** 如果信号量成功创建，则返回一个信号量句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_COUNTING_SEMAPHORES` 和 `configSUPPORT_STATIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。用户需要自行管理 `pxSemaphoreBuffer` 所指向的内存生命周期。

### 4.2 获取信号量

- **描述：** 尝试获取一个计数型信号量。如果信号量的计数值大于 0，则计数值减 1，获取成功。如果计数值为 0，调用任务将进入阻塞状态（如果指定了等待时间），直到信号量可用或超时。
- **函数原型：** `BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );`
- **参数：**
  - `xSemaphore`: 要获取的信号量句柄。
  - `xTicksToWait`: 如果信号量不可用，任务愿意阻塞等待的节拍数。
- **返回值：**
  - `pdPASS`: 成功获取信号量。
  - `pdFAIL`: 信号量不可用，且等待时间已到或为 `0` 未等待。
- **注意事项：** 通常用于当某个资源有多个实例时，任务在访问资源前先获取信号量。

### 4.3 在中断中获取信号量

- **描述：** 在中断服务例程 (ISR) 中尝试获取一个计数型信号量。此函数不会阻塞。
- **函数原型：** `BaseType_t xSemaphoreTakeFromISR( SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken );`
- **参数：**
  - `xSemaphore`: 要获取的信号量句柄。
  - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
- **返回值：**
  - `pdPASS`: 成功获取信号量。
  - `pdFALSE`: 信号量不可用。
- **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

### 4.4 释放信号量

- **描述：** 释放一个计数型信号量，使其计数值加 1。如果计数值未达到最大值，且当前有任务因等待此信号量而阻塞，则优先级最高的阻塞任务将被解除阻塞。
- **函数原型：** `BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );`
- **参数：** `xSemaphore` - 要释放的信号量句柄。
- **返回值：**
  - `pdPASS`: 成功释放信号量。
  - `pdFAIL`: 信号量计数值已达到最大值。
- **注意事项：** 当用于资源管理时，通常在任务使用完资源后释放信号量。

### 4.5 在中断中释放信号量

- **描述：** 在中断服务例程 (ISR) 中释放一个计数型信号量。
- **函数原型：** `BaseType_t xSemaphoreGiveFromISR( SemaphoreHandle_t xSemaphore, BaseType_t *pxHigherPriorityTaskWoken );`
- **参数：**
  - `xSemaphore`: 要释放的信号量句柄。
  - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务解除阻塞，则该变量将被设置为 `pdTRUE`。
- **返回值：**
  - `pdPASS`: 成功释放信号量。
  - `pdFALSE`: 信号量计数值已达到最大值。
- **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

### 4.6 删除信号量

- **描述：** 删除一个计数型信号量，并释放其占用的内存（如果它是动态创建的）。
- **函数原型：** `void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );`
- **参数：** `xSemaphore` - 要删除的信号量句柄。
- **注意事项：** 只能删除动态创建的信号量。删除后，不应再使用该句柄。

## 5. 计数型信号量操作测试

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

SemaphoreHandle_t CountSemaphore; // 计数信号量
/*---------------------------------------------*/
```

### 5.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Counting Semaphore",RED);
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
    // 创建计数信号量
    CountSemaphore = xSemaphoreCreateCounting((UBaseType_t)255, (UBaseType_t)0); // 创建计数信号量，初始值为0，最大值为255  
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
    uint8_t key_value = 0;
    UBaseType_t sema_value = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                xSemaphoreGive(CountSemaphore); // 给计数信号量加1
                sema_value = uxSemaphoreGetCount(CountSemaphore); // 获取计数信号量的值
                lcd_show_xnum(166, 111, sema_value, 2, 16, 0, BLUE);
            }
            default:break;
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    UBaseType_t sema_value = 0;
    uint32_t task2_num = 0;
    while(1)
    {
        xSemaphoreTake(CountSemaphore, portMAX_DELAY); // 获取计数信号量，阻塞等待
        sema_value = uxSemaphoreGetCount(CountSemaphore);
        lcd_show_xnum(166, 111, sema_value, 2, 16, 0, BLUE);             
        lcd_fill(6, 131, 233, 313, lcd_discolor[++task2_num % 11]);     
        vTaskDelay(1000);
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

## 6. 互斥信号量

互斥信号量其实就是一个拥有优先级继承的二值信号量，在同步的应用中（任务与任务或中断与任务之间的同步）二值信号量最适合。互斥信号量适合用于那些需要互斥访问的应用中。在互斥访问中互斥信号量相当于一把钥匙， 当任务想要访问共享资源的时候就必须先获得这把钥匙，当访问完共享资源以后就必须归还这把钥匙，这样其他的任务就可以拿着这把钥匙去访问资源。

互斥信号量使用和二值信号量相同的 API 操作函数，所以互斥信号量也可以设置阻塞时间，不同于二值信号量的是互斥信号量具有优先级继承的机制。当一个互斥信号量正在被一个低优先级的任务持有时， 如果此时有个高优先级的任务也尝试获取这个互斥信号量，那么这个高优先级的任务就会被阻塞。不过这个高优先级的任务会将低优先级任务的优先级提升到与自己相同的优先级，这个过程就是优先级继承。优先级继承尽可能的减少了高优先级任务处于阻塞态的时间，并且将“优先级翻转”的影响降到最低。

优先级继承并不能完全的消除优先级翻转的问题，它只是尽可能的降低优先级翻转带来的影响。实时应用应该在设计之初就要避免优先级翻转的发生。互斥信号量不能用于中断服务函数中，原因如下：

1. 互斥信号量有任务优先级继承的机制， 但是中断不是任务，没有任务优先级， 所以互斥信号量只能用与任务中，不能用于中断服务函数。

2. 中断服务函数中不能因为要等待互斥信号量而设置阻塞时间进入阻塞态。

| 函数                              | 描述            |
| ------------------------------- | ------------- |
| `xSemaphoreCreateMutex()`       | 使用动态方式创建互斥信号量 |
| `xSemaphoreCreateMutexStatic()` | 使用静态方式创建互斥信号量 |
| `xSemaphoreTake()`              | 获取信号量         |
| `xSemaphoreGive()`              | 释放信号量         |
| `vSemaphoreDelete()`            | 删除信号量         |

### 6.1 创建互斥信号量

- **`xSemaphoreCreateMutex()`**
  
  - **描述：** 使用动态方式创建互斥信号量。互斥量所需的内存将从 FreeRTOS 堆中分配。创建时，互斥量会被初始化为“空”（可用）状态，其计数值为 1。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateMutex( void );`
  - **返回值：** 如果互斥量成功创建，则返回一个信号量句柄（`SemaphoreHandle_t` 类型）；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_MUTEXES` 和 `configSUPPORT_DYNAMIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。互斥量具有优先级继承特性，当一个低优先级任务持有互斥量而高优先级任务试图获取该互斥量时，低优先级任务的优先级会被临时提升到高优先级任务的优先级，直到它释放互斥量。

- **`xSemaphoreCreateMutexStatic()`**
  
  - **描述：** 使用静态方式创建互斥信号量。互斥量所需的内存由用户在编译时或运行时提供。创建时，互斥量会被初始化为“空”（可用）状态。
  - **函数原型：** `SemaphoreHandle_t xSemaphoreCreateMutexStatic( StaticSemaphore_t *pxSemaphoreBuffer );`
  - **参数：** `pxSemaphoreBuffer` - 指向用户提供的 `StaticSemaphore_t` 结构体变量的指针，该结构体将用作互斥量的控制块。
  - **返回值：** 如果互斥量成功创建，则返回一个信号量句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configUSE_MUTEXES` 和 `configSUPPORT_STATIC_ALLOCATION` 都需在 `FreeRTOSConfig.h` 中定义为 1。用户需要自行管理 `pxSemaphoreBuffer` 所指向的内存生命周期。

### 6.2 获取互斥信号量

- **描述：** 尝试获取一个互斥信号量。如果互斥量可用，则获取成功，互斥量变为不可用状态，并记录下获取该互斥量的任务。如果互斥量不可用，调用任务将进入阻塞状态（如果指定了等待时间），直到互斥量可用或超时。
- **函数原型：** `BaseType_t xSemaphoreTake( SemaphoreHandle_t xSemaphore, TickType_t xTicksToWait );`
- **参数：**
  - `xSemaphore`: 要获取的互斥量句柄。
  - `xTicksToWait`: 如果互斥量不可用，任务愿意阻塞等待的节拍数。
- **返回值：**
  - `pdPASS`: 成功获取互斥量。
  - `pdFAIL`: 互斥量不可用，且等待时间已到或为 `0` 未等待。
- **注意事项：** 互斥量应该总是由获取它的任务释放。在获取互斥量时，FreeRTOS 会自动检查是否有更高优先级的任务正在等待该互斥量，并可能触发优先级继承。

### 6.3 释放互斥信号量

- **描述：** 释放一个互斥信号量，使其变为可用状态。只有持有该互斥量的任务才能成功释放它。如果当前有任务因等待此互斥量而阻塞，则优先级最高的阻塞任务将被解除阻塞。
- **函数原型：** `BaseType_t xSemaphoreGive( SemaphoreHandle_t xSemaphore );`
- **参数：** `xSemaphore` - 要释放的互斥量句柄。
- **返回值：**
  - `pdPASS`: 成功释放互斥量。
  - `pdFAIL`: 信号量已处于可用状态，或者尝试释放的不是持有信号量的任务。
- **注意事项：** 当任务释放互斥量时，如果它之前因为优先级继承而提升了优先级，其优先级将被恢复到原始值。**互斥量不能从 ISR 中释放**（因为 ISR 没有优先级继承的概念，并且不允许阻塞），如果需要在 ISR 中释放，应该考虑使用二值信号量作为同步机制。

### 6.4 删除信号量

- **描述：** 删除一个互斥信号量，并释放其占用的内存（如果它是动态创建的）。
- **函数原型：** `void vSemaphoreDelete( SemaphoreHandle_t xSemaphore );`
- **参数：** `xSemaphore` - 要删除的互斥量句柄。
- **注意事项：** 只能删除动态创建的互斥量。删除后，不应再使用该句柄。

## 7. 互斥信号量操作测试

### 7.1 任务配置

```c
/*----------------任务配置区-----------------*/
// START_TASK配置
#define START_TASK_PRIO 1           // 任务优先级 
#define START_STK_SIZE  128         // 任务堆栈大小 
TaskHandle_t StartTask_Handler;     // 任务句柄 
void start_task(void *pvParameters);// 任务函数 

// TASK1配置
#define TASK1_PRIO      4                   
#define TASK1_STK_SIZE  128                 
TaskHandle_t Task1Task_Handler;          
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      2                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);

SemaphoreHandle_t MutexSemaphore; // 互斥信号量
/*---------------------------------------------*/
```

### 7.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"MultiTask Demo",RED);
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
    // 创建互斥信号量
    MutexSemaphore = xSemaphoreCreateMutex();  
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

// task1函数实现-优先级低
void task1(void *pvParameters)
{
    vTaskDelay(500);
    while(1)
    {
        printf("task1 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // 获取互斥信号量
        printf("task1 has taked mutex\r\n");
        printf("task1 running\r\n");
        printf("task1 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // 释放互斥信号量
        vTaskDelay(100);

    }
}

// task2函数实现-优先级中
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    vTaskDelay(200);
    while(1)
    {
        for(task2_num = 0; task2_num < 5; task2_num++)
        {
            printf("task2 running\r\n");
            delay_ms(100); // 模拟运行，不触发任务调度
        }
        vTaskDelay(1000);
    }
}

// task3函数实现-优先级搞
void task3(void *pvParameters)
{
    uint32_t task3_num = 0;
    while(1)
    {
        printf("task3 ready to take mutex\r\n");
        xSemaphoreTake(MutexSemaphore, portMAX_DELAY); // 获取互斥信号量
        printf("task3 has taked mutex\r\n");
        for(task3_num = 0; task3_num < 5; task3_num++)
        {
            printf("task3 running\r\n");
            delay_ms(100); // 模拟运行，不触发任务调度
        }
        printf("task3 give mutex\r\n");
        xSemaphoreGive(MutexSemaphore); // 释放互斥信号量
        vTaskDelay(1000);
    }
}
// 程序运行流程：task3获取互斥信号量（因为优先级最高）
// task3运行5次后释放互斥信号量
// task2同时运行5次
// task1获取互斥信号量（因为优先级最低）
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

## 8. 三种信号量应用场景总结

### 8.1 二值信号量 (Binary Semaphore)

二值信号量只有两种状态：满（可用）和空（不可用），其行为类似于一个只有一个元素的队列。它主要用于**任务与任务**或**中断与任务**之间的**同步**

**典型应用场景：**

- **事件通知/任务同步：**
  
  - **中断通知任务：** 当一个中断发生时，ISR 释放（Give）一个二值信号量，而一个任务则等待（Take）这个信号量。ISR 不会阻塞，而任务在等待期间可以进入阻塞状态以节省CPU。这是中断通知任务最常用的方式。
    - *示例：* 一个串口接收中断接收到一个完整的帧数据后，通过 `xSemaphoreGiveFromISR()` 释放信号量，通知一个处理任务去解析数据。
  - **任务通知任务：** 一个任务完成某个操作或达到某个状态后，释放信号量通知另一个任务继续执行。
    - *示例：* 一个数据采集任务完成数据采集后，释放信号量通知数据处理任务开始处理数据。

- **简单的互斥锁：**
  
  - 虽然二值信号量可以用于互斥（例如，保护一个共享资源），但它不具备优先级继承机制。这意味着在高优先级任务尝试获取被低优先级任务持有的二值信号量时，可能会发生优先级反转问题。因此，**不推荐**使用二值信号量作为互斥锁来保护共享资源，特别是当可能发生优先级反转时。互斥量（Mutex）是更优的选择。

### 8.2 计数型信号量 (Counting Semaphore)

计数型信号量有一个计数器，其值可以大于 1。它主要用于**资源管理**和**事件计数**。

**典型应用场景：**

- **资源管理：** 当有多个相同类型的资源实例时，计数型信号量可以用于管理这些资源的使用。计数值代表可用资源的数量。
  
  - *示例：* 系统中有3个打印机，创建初始计数值为3的计数型信号量。每个任务需要打印时先获取信号量，使用完后释放信号量。当所有打印机都被占用时，后续尝试获取信号量的任务将阻塞，直到有打印机被释放。
  - *示例：* 一个缓冲区池中有N个空闲缓冲区。每当任务需要一个缓冲区时，它获取信号量；每当任务释放一个缓冲区时，它释放信号量。

- **事件计数：** 用于记录某个事件发生的次数。
  
  - *示例：* 多个中断源都可能触发相同的事件，每次事件发生时，ISR 都释放信号量。一个任务通过获取信号量来处理事件，并知道有多少个事件待处理（通过信号量的当前值）。
  - *示例：* 在生产者-消费者模型中，生产者每次生产一个数据项就释放信号量，消费者每次处理一个数据项就获取信号量。信号量的计数值可以表示队列中待处理的数据项数量。

### 8.3 互斥信号量 (Mutex Semaphore)

互斥信号量是特殊的二值信号量，它增加了 **优先级继承（Priority Inheritance）** 机制。它专门用于**互斥访问共享资源**，有效解决了优先级反转问题。

**典型应用场景：**

- **保护共享资源（临界区）：** 这是互斥信号量的最主要用途。当多个任务需要访问同一块共享内存、外设寄存器或其他非可重入资源时，为了避免数据损坏或状态不一致，需要使用互斥量来确保在任何给定时刻只有一个任务能够访问该资源。
  
  - *示例：* 多个任务需要向同一个串口发送数据。为串口设备创建一个互斥量，每个任务在发送数据前先获取互斥量，发送完成后释放互斥量。这确保了数据发送的原子性，避免了数据混淆。
  - *示例：* 访问共享全局变量或数据结构时，使用互斥量将其操作包装起来，以保证数据完整性。

- **解决优先级反转问题：** 优先级继承是互斥量最重要的特性。当一个高优先级任务尝试获取一个被低优先级任务持有的互斥量时，低优先级任务的优先级会被临时提升到高优先级任务的优先级，直到它释放互斥量。这确保了低优先级任务能够尽快执行并释放资源，避免了高优先级任务被“饿死”的情况。
  
  - *示例：* 任务A (高优先级) 和任务B (低优先级) 都需要访问共享资源X。如果任务B在获取了互斥量保护资源X后，任务A抢占了任务B并尝试获取互斥量X。由于优先级继承，任务B的优先级会临时提升到任务A的优先级，使其能够尽快执行完临界区并释放互斥量，从而避免了任务A被一个中等优先级任务打断而无限期等待的问题。

**总结表格：**

| 特性/信号量类型     | 二值信号量                        | 计数型信号量                       | 互斥信号量           |
| ------------ | ---------------------------- | ---------------------------- | --------------- |
| **主要用途**     | 任务同步、事件通知                    | 资源管理、事件计数                    | 保护共享资源（临界区）     |
| **计数范围**     | 0或1                          | 0到N（N为最大计数值）                 | 0或1             |
| **优先级继承**    | 无                            | 无                            | 有               |
| **ISR中Give** | 支持 (`xSemaphoreGiveFromISR`) | 支持 (`xSemaphoreGiveFromISR`) | **不支持**         |
| **主要优点**     | 轻量、简单高效的同步                   | 管理多个相同资源实例                   | 解决优先级反转，强健的互斥保护 |
| **主要限制**     | 不适合复杂资源管理；无优先级继承             | 无优先级继承                       | 不能从ISR中Give     |

---


