# 第三章 FreeRTOS任务基础

## 1. 创建和删除任务

| 函数                              | 描述                 |
| ------------------------------- | ------------------ |
| `xTaskCreate()`                 | 动态方式创建任务           |
| `xTaskCreateStatic()`           | 静态方式创建任务           |
| `xTaskCreateRestricted()`       | 动态方式创建使用 MPU 限制的任务 |
| `xTaskCreateRestrictedStatic()` | 静态方式创建使用 MPU 限制的任务 |
| `vTaskDelete()`                 | 删除任务               |

### 1.1 动态方式创建任务

`xTaskCreate()` 函数用于创建一个新的 FreeRTOS 任务并将其添加到就绪任务列表中，以便调度器进行管理和运行。

**函数原型：**

```c
BaseType_t xTaskCreate( TaskFunction_t pvTaskCode,
                        const char * const pcName,
                        unsigned short usStackDepth,
                        void *pvParameters,
                        UBaseType_t uxPriority,
                        TaskHandle_t *pxCreatedTask );
```

**参数说明：**

- `pvTaskCode`: 指向任务入口函数的指针。任务函数通常实现为一个无限循环，并且不应该返回或退出。
- `pcName`: 任务的描述性名称。主要用于调试，最大长度由 `configMAX_TASK_NAME_LEN` 定义 (默认为 16)。
- `usStackDepth`: 任务堆栈的大小，以 **字** 为单位，而不是字节。例如，如果堆栈是 16 位宽，`usStackDepth` 为 100，则分配 200 字节的堆栈。
- `pvParameters`: 指向将作为参数传递给被创建任务的指针。
- `uxPriority`: 任务的优先级。
- `pxCreatedTask`: 用于将创建的任务句柄传回的指针。这个句柄可以用来引用任务，例如在 `vTaskDelete()` 中删除任务。 此参数是可选的，可以设置为 `NULL`。

**返回值：**

- `pdPASS`：任务成功创建并添加到就绪列表。
- 其他错误代码：定义在 `errors.h` 文件中。

**注意事项：**

- 使用 `xTaskCreate()` 创建任务时，所需的 RAM 会自动从 FreeRTOS 堆中分配。
- 如果系统支持 MPU，可以使用 `xTaskCreateRestricted()` 来创建受 MPU 约束的任务。
- 如果 `configSUPPORT_DYNAMIC_ALLOCATION` 未定义或设置为 1，则 `xTaskCreate()` 函数可用。

### 1.2 静态方式创建任务

与 `xTaskCreate()` 动态分配内存不同，`xTaskCreateStatic()` 允许用户在编译时或运行时提供任务堆栈和任务控制块（TCB）所需的内存，从而避免了运行时的堆内存分配。这在对内存管理有严格要求或不能使用动态内存分配的嵌入式系统中非常有用。

**函数原型：**

```c
TaskHandle_t xTaskCreateStatic(   TaskFunction_t pxTaskCode,
                                  const char * const pcName,
                                  const uint32_t ulStackDepth,
                                  void * const pvParameters,
                                  UBaseType_t uxPriority,
                                  StackType_t * const puxStackBuffer,
                                  StaticTask_t * const pxTaskBuffer );
```

**参数说明：**

- `pxTaskCode`: 指向任务入口函数的指针。
- `pcName`: 任务的描述性名称。
- `ulStackDepth`: 任务堆栈的大小，以 **字** 为单位。
- `pvParameters`: 将作为参数传递给被创建任务的指针。
- `uxPriority`: 任务的优先级。
- `puxStackBuffer`: 指向用户提供的、用于任务堆栈的 `uint32_t` 数组的指针。
- `pxTaskBuffer`: 指向用户提供的、用于任务控制块 (TCB) 的 `StaticTask_t` 变量的指针。

**返回值：**

- `TaskHandle_t`: 任务创建成功返回任务句柄，否则返回 `NULL`。

**注意事项：**

- 要使此函数可用，`configSUPPORT_STATIC_ALLOCATION` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 用户需要自行管理 `puxStackBuffer` 和 `pxTaskBuffer` 所指向的内存生命周期。

### 1.3 动态方式创建使用 MPU 限制的任务

`xTaskCreateRestricted()` 用于创建具有内存保护单元 (MPU) 限制的任务。MPU 允许为任务定义独立的内存访问权限，增强系统的安全性、隔离性和稳定性。

**函数原型：**

```c
BaseType_t xTaskCreateRestricted(  const TaskParameters_t * const pxTaskDefinition,
                                   TaskHandle_t *pxCreatedTask );
```

**参数说明：**

- `pxTaskDefinition`: 指向 `TaskParameters_t` 结构体的指针，该结构体包含任务的配置信息，包括任务代码、堆栈大小、优先级以及 MPU 区域定义等。
- `pxCreatedTask`: 用于传回创建的任务句柄的指针。

**返回值：**

- `pdPASS`：任务成功创建。
- 其他错误代码。

**注意事项：**

- 要使此函数可用，`configENABLE_MPU` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 需要处理器支持 MPU 功能。

### 1.4 静态方式创建使用 MPU 限制的任务

`xTaskCreateRestrictedStatic()` 结合了 `xTaskCreateRestricted()` 的 MPU 限制功能和 `xTaskCreateStatic()` 的静态内存分配特性。它允许用户在编译时或运行时提供内存，并同时为任务设置 MPU 权限。

**函数原型：**

```c
TaskHandle_t xTaskCreateRestrictedStatic(  const TaskParameters_t * const pxTaskDefinition,
                                           TaskHandle_t *pxCreatedTask );
```

**参数说明：**

- `pxTaskDefinition`: 指向 `TaskParameters_t` 结构体的指针，其中包含任务的配置信息，包括 MPU 区域定义以及指向用户提供的堆栈和 TCB 缓冲区的指针。
- `pxCreatedTask`: 用于传回创建的任务句柄的指针。

**返回值：**

- `TaskHandle_t`: 任务创建成功返回任务句柄，否则返回 `NULL`。

**注意事项：**

- 要使此函数可用，`configENABLE_MPU` 和 `configSUPPORT_STATIC_ALLOCATION` 都必须在 `FreeRTOSConfig.h` 中定义为 1。
- 需要处理器支持 MPU 功能。

### 1.5 删除任务：`vTaskDelete()`

`vTaskDelete()` 函数用于将任务从 FreeRTOS 实时内核的管理中移除。

**函数原型：**

```c
void vTaskDelete( TaskHandle_t xTask );
```

**参数说明：**

- `xTask`: 要删除的任务的句柄。如果传入 `NULL`，则会删除调用此函数的任务自身。

**注意事项：**

- 要使此函数可用，`INCLUDE_vTaskDelete` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 被删除的任务将从所有就绪、阻塞、挂起和事件列表中移除。
- 如果任务删除其他任务，FreeRTOS 内核分配的内存会在 API 中立即释放。如果任务删除自身，则由空闲任务 (`idle task`) 负责释放内核分配的内存。因此，如果应用程序调用 `vTaskDelete()`，确保空闲任务有足够的处理时间来释放内存非常重要。
- 任务代码中分配的内存不会自动释放，应在任务删除之前手动释放。
- 删除任务主要有两种情况：自删除（在任务本身的 TaskCode 中调用 `vTaskDelete(NULL)`）和强制删除（在其他任务中删除另一个任务）。
- 对于使用 `xTaskCreate()` 创建的任务，删除后存储资源将在空闲任务中自动释放。对于使用 `xTaskCreateStatic()` 静态创建的任务，存储资源不会被释放，需要手动释放。
- 在删除任务时，需要考虑该任务是否占用了共享资源、是否申请了新资源，以及是否与关联任务或中断服务例程 (ISR) 存在通信关系，以避免后遗症。

## 2. FreeRTOS任务创建与删除测试

### 2.1 动态方法

#### 2.1.1 任务配置

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

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);
/*---------------------------------------------*/
```

#### 2.1.2 任务实现

```c
/*----------------任务函数实现区----------------*/
void freertos_demo(void)
{
    // 初始化LCD显示
    lcd_show_string(10, 47, 220, 30, 24, "Task Create&Delete", RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);

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
```

#### 2.1.3 主函数

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

### 2.2 静态方法

#### 2.2.1 提供空闲任务和软件定时器服务任务

```c
/*---------------FreeRTOS系统配置区-------------*/
static StackType_t IdleTaskStack[configMINIMAL_STACK_SIZE]; // 空闲任务栈
static StaticTask_t IdleTaskTCB; // 空闲任务控制块
static StackType_t TimerTaskStack[configTIMER_TASK_STACK_DEPTH]; // 定时器任务栈
static StaticTask_t TimerTaskTCB; // 定时器任务控制块
/*---------------------------------------------*/

/*----------------任务函数配置区-----------------*/
/**
 * @brief       获取空闲任务地任务堆栈和任务控制块内存，因为本例程使用的
                静态内存，因此空闲任务的任务堆栈和任务控制块的内存就应该
                有用户来提供，FreeRTOS提供了接口函数vApplicationGetIdleTaskMemory()
                实现此函数即可。
 * @param       ppxIdleTaskTCBBuffer:任务控制块内存
                ppxIdleTaskStackBuffer:任务堆栈内存
                pulIdleTaskStackSize:任务堆栈大小
 * @retval      无
 */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, 
                                   StackType_t  **ppxIdleTaskStackBuffer, 
                                   uint32_t     *pulIdleTaskStackSize)
{
    *ppxIdleTaskTCBBuffer   = &IdleTaskTCB;
    *ppxIdleTaskStackBuffer = IdleTaskStack;
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

/**
 * @brief       获取定时器服务任务的任务堆栈和任务控制块内存
 * @param       ppxTimerTaskTCBBuffer:任务控制块内存
                ppxTimerTaskStackBuffer:任务堆栈内存
                pulTimerTaskStackSize:任务堆栈大小
 * @retval      无
 */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t  **ppxTimerTaskStackBuffer,
                                    uint32_t     *pulTimerTaskStackSize)
{
    *ppxTimerTaskTCBBuffer  = &TimerTaskTCB;
    *ppxTimerTaskStackBuffer= TimerTaskStack;
    *pulTimerTaskStackSize  = configTIMER_TASK_STACK_DEPTH;
}
```

#### 2.2.2 任务配置

```c
// START_TASK配置
#define START_TASK_PRIO 1           // 任务优先级 
#define START_STK_SIZE  128         // 任务堆栈大小 
StackType_t StartTaskStack[START_STK_SIZE]; // 任务堆栈
StaticTask_t StartTaskTCB; // 任务控制块
TaskHandle_t StartTask_Handler; // 任务句柄
void start_task(void *pvParameters);

// TASK1配置
#define TASK1_PRIO      2                   
#define TASK1_STK_SIZE  128                
StackType_t Task1TaskStack[TASK1_STK_SIZE]; 
StaticTask_t Task1TaskTCB; 
TaskHandle_t Task1Task_Handler;
void task1(void *pvParameters);

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128
StackType_t Task2TaskStack[TASK2_STK_SIZE]; 
StaticTask_t Task2TaskTCB;                
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128  
StackType_t Task3TaskStack[TASK3_STK_SIZE]; 
StaticTask_t Task3TaskTCB;                
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);
```

#### 2.2.3 任务实现

```c
*----------------任务函数实现区----------------*/
void freertos_demo(void)
{
    // 初始化LCD显示
    lcd_show_string(10, 47, 220, 30, 24, "Task Create&Delete", RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);

    // 创建START_TASK任务
    StartTask_Handler = xTaskCreateStatic((TaskFunction_t)start_task,   // 任务函数
                                          (const char*)"start_task",    // 任务名称
                                          (uint16_t)START_STK_SIZE,     // 任务堆栈大小
                                          (void*)NULL,                  // 传递给任务函数的参数
                                          (UBaseType_t)START_TASK_PRIO, // 任务优先级
                                          (StackType_t*)StartTaskStack, // 任务堆栈
                                          (StaticTask_t*)&StartTaskTCB);// 任务控制块
    // 开始任务调度
    vTaskStartScheduler();
}

// start_task函数实现
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL(); // 进入临界区
    // 创建TASK1任务
    Task1Task_Handler = xTaskCreateStatic((TaskFunction_t)task1,      
                                           (const char*)"task1",       
                                           (uint16_t)TASK1_STK_SIZE,   
                                           (void*)NULL,               
                                           (UBaseType_t)TASK1_PRIO,    
                                           (StackType_t*)Task1TaskStack,
                                           (StaticTask_t*)&Task1TaskTCB);
    // 创建TASK2任务
    Task2Task_Handler = xTaskCreateStatic((TaskFunction_t)task2,      
                                           (const char*)"task2",       
                                           (uint16_t)TASK2_STK_SIZE,   
                                           (void*)NULL,               
                                           (UBaseType_t)TASK2_PRIO,    
                                           (StackType_t*)Task2TaskStack,
                                           (StaticTask_t*)&Task2TaskTCB);
    // 创建TASK3任务
    Task3Task_Handler = xTaskCreateStatic((TaskFunction_t)task3,      
                                           (const char*)"task3",       
                                           (uint16_t)TASK3_STK_SIZE,   
                                           (void*)NULL,               
                                           (UBaseType_t)TASK3_PRIO,    
                                           (StackType_t*)Task3TaskStack,
                                           (StaticTask_t*)&Task3TaskTCB);
    vTaskDelete(StartTask_Handler); // 删除START_TASK任务
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        lcd_fill(6,131,114,313,lcd_discolor[++task1_count % 11]);
        lcd_show_xnum(71,111,task1_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task2函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task2(void *pvParameters)
{
    uint32_t task2_count = 0;
    while(1)
    {
        lcd_fill(126,131,233,313,lcd_discolor[11-(++task2_count % 11)]);
        lcd_show_xnum(191,111,task2_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task3函数实现
// 实现功能：按下KEY0删除task1任务，按下KEY1删除task2任务
void task3(void *pvParameters)
{
    uint8_t key_value;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY0_PRES)
        {
            if(Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                lcd_show_string(15, 111, 110, 16, 16, "Task1: Del", RED); 
            }
        }
        if(key_value == KEY1_PRES)
        {
            if(Task2Task_Handler != NULL)
            {
                vTaskDelete(Task2Task_Handler);
                Task2Task_Handler = NULL;
                lcd_show_string(135, 111, 110, 16, 16, "Task2: Del", RED);
            }
        }
        vTaskDelay(10);
    }
}
/*---------------------------------------------*/
```

#### 2.2.4 主函数

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

## 3. 挂起和恢复任务

| 函数                     | 描述           |
| ---------------------- | ------------ |
| `vTaskSuspend()`       | 挂起任务         |
| `vTaskResume()`        | 恢复被挂起的任务     |
| `xTaskResumeFromISR()` | 在中断中恢复被挂起的任务 |

### 3.1 挂起任务

`vTaskSuspend()` 函数用于将一个任务置于挂起状态。当任务处于挂起状态时，它将不会被 FreeRTOS 调度器调度执行，即使它的优先级很高。

**函数原型：**

```c
void vTaskSuspend( TaskHandle_t xTaskToSuspend );
```

**参数说明：**

- `xTaskToSuspend`: 要挂起的任务的句柄。如果传入 `NULL`，则会挂起调用此函数的任务自身。

**注意事项：**

- 要使此函数可用，`INCLUDE_vTaskSuspend` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 被挂起的任务将从所有就绪、阻塞和事件列表中移除，并放置在挂起列表中。
- 被挂起的任务在被恢复之前不会消耗任何 CPU 时间。
- 如果一个任务被挂起，即使有外部事件（如信号量、队列消息等）发生，它也不会被解除阻塞，直到它被明确地恢复。

### 3.2 恢复被挂起的任务

`vTaskResume()` 函数用于将一个处于挂起状态的任务恢复到就绪状态。一旦任务被恢复，它就可以被 FreeRTOS 调度器调度执行。

**函数原型：**

```c
void vTaskResume( TaskHandle_t xTaskToResume );
```

**参数说明：**

- `xTaskToResume`: 要恢复的任务的句柄。

**注意事项：**

- 要使此函数可用，`INCLUDE_vTaskSuspend` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 此函数不能从中断服务例程 (ISR) 中调用。在 ISR 中恢复任务应使用 `xTaskResumeFromISR()`。
- 如果恢复一个已经处于就绪或阻塞状态的任务，`vTaskResume()` 将不执行任何操作。

### 3.3 在中断中恢复被挂起的任务

`xTaskResumeFromISR()` 函数是 `vTaskResume()` 的中断安全版本，用于在中断服务函数 (ISR) 中恢复一个被挂起的任务。

**函数原型：**

```c
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume );
```

**参数说明：**

- `xTaskToResume`: 要恢复的任务的句柄。

**返回值：**

- `pdTRUE`：如果调用 `xTaskResumeFromISR()` 导致更高优先级的任务被解除阻塞（即需要进行上下文切换），则返回 `pdTRUE`。
- `pdFALSE`：否则返回 `pdFALSE`。

**注意事项：**

- 要使此函数可用，`INCLUDE_vTaskSuspend` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 此函数通常在中断处理结束时用于检查是否需要进行任务切换。如果返回 `pdTRUE`，通常会调用 `portYIELD_FROM_ISR()` 或类似的宏来触发上下文切换，从而立即执行被恢复的高优先级任务。

## 4. 挂起和恢复任务测试

### 4.1 任务配置

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

// TASK2配置
#define TASK2_PRIO      3                   
#define TASK2_STK_SIZE  128                 
TaskHandle_t Task2Task_Handler;          
void task2(void *pvParameters);

// TASK3配置
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);
/*---------------------------------------------*/
```

### 4.2 任务实现

```c
/*----------------任务函数实现区----------------*/
void freertos_demo(void)
{
    // 初始化LCD显示
    lcd_show_string(10, 47, 220, 30, 24, "Task Create&Delete", RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Task1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Task2: 000", BLUE);

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
    // 创建TASK3任务
    xTaskCreate((TaskFunction_t)task3,      
                (const char*)"task3",       
                (uint16_t)TASK3_STK_SIZE,   
                (void*)NULL,               
                (UBaseType_t)TASK3_PRIO,    
                (TaskHandle_t*)&Task3Task_Handler);
    vTaskDelete(StartTask_Handler); // 删除START_TASK任务
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task1(void *pvParameters)
{
    uint32_t task1_count = 0;
    while(1)
    {
        lcd_fill(6,131,114,313,lcd_discolor[++task1_count % 11]);
        lcd_show_xnum(71,111,task1_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task2函数实现
// 实现功能：记录自己的运行次数，每次运行时在LCD显示不同颜色
void task2(void *pvParameters)
{
    uint32_t task2_count = 0;
    while(1)
    {
        lcd_fill(126,131,233,313,lcd_discolor[11-(++task2_count % 11)]);
        lcd_show_xnum(191,111,task2_count,3,16,0x80,BLUE);
        vTaskDelay(500);
    }
}

// task3函数实现
// 实现功能：按下KEY0删除task1任务，按下KEY1删除task2任务
void task3(void *pvParameters)
{
    uint8_t key_value;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value == KEY0_PRES)
        {
            if(Task1Task_Handler != NULL)
            {
                vTaskDelete(Task1Task_Handler);
                Task1Task_Handler = NULL;
                lcd_show_string(15, 111, 110, 16, 16, "Task1: Del", RED); 
            }
        }
        if(key_value == KEY1_PRES)
        {
            if(Task2Task_Handler != NULL)
            {
                vTaskSuspend(Task2Task_Handler); // 挂起task2任务
            }
        }
        if(key_value == WKUP_PRES)
        {
            if(Task2Task_Handler != NULL)
            {
                vTaskResume(Task2Task_Handler); // 恢复task2任务
            }
        }
        vTaskDelay(10);
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

---
