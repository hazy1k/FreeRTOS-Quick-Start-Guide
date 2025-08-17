# 第六章 FreeRTOS任务相关API函数介绍

## 1. 任务相关函数

| 函数                               | 描述               |
| -------------------------------- | ---------------- |
| `uxTaskPriorityGet()`            | 获取任务优先级          |
| `vTaskPrioritySet()`             | 设置任务优先级          |
| `uxTaskGetSystemState()`         | 获取所有任务的状态信息      |
| `vTaskGetInfo()`                 | 获取单个任务的状态信息      |
| `xTaskGetApplicationTaskTag()`   | 获取任务 Tag         |
| `xTaskGetCurrentTaskHandle()`    | 获取当前任务的任务句柄      |
| `xTaskGetHandle()`               | 获取指定任务的任务句柄      |
| `xTaskGetIdleTaskHandle()`       | 获取空闲任务的任务句柄      |
| `uxTaskGetStackHighWaterMark()`  | 获取任务的栈历史剩余最小值    |
| `eTaskGetState()`                | 获取任务状态           |
| `pcTaskGetName()`                | 获取任务名            |
| `xTaskGetTickCount()`            | 获取系统时钟节拍计数器的值    |
| `xTaskGetTickCountFromISR()`     | 中断中获取系统使用节拍计数器的值 |
| `xTaskGetSchedulerState()`       | 获取任务调度器状态        |
| `uxTaskGetNumberOfTasks()`       | 获取系统中任务的数量       |
| `vTaskList()`                    | 以“表格”形式获取所有任务的信息 |
| `vTaskGetRunTimeStats()`         | 获取任务的运行时间等信息     |
| `vTaskSetApplicationTaskTag()`   | 设置任务 Tag         |
| `SetThreadLocalStoragePointer()` | 设置任务的独有数据记录数组指针  |
| `GetThreadLocalStoragePointer()` | 获取任务的独有数据记录数组指针  |

### 1.1 任务优先级相关函数

- **`uxTaskPriorityGet()`**
  
  - **描述：** 获取指定任务的当前优先级。
  - **函数原型：** `UBaseType_t uxTaskPriorityGet( TaskHandle_t xTask );`
  - **参数：** `xTask` - 任务句柄。如果传入 `NULL`，则返回调用此函数的任务的优先级。
  - **返回值：** 任务的优先级。
  - **注意事项：** 要使此函数可用，`INCLUDE_uxTaskPriorityGet` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`vTaskPrioritySet()`**
  
  - **描述：** 设置指定任务的优先级。
  - **函数原型：** `void vTaskPrioritySet( TaskHandle_t xTask, UBaseType_t uxNewPriority );`
  - **参数：**
    - `xTask` - 任务句柄。如果传入 `NULL`，则设置调用此函数的任务的优先级。
    - `uxNewPriority` - 新的优先级。
  - **注意事项：** 要使此函数可用，`INCLUDE_vTaskPrioritySet` 需在 `FreeRTOSConfig.h` 中定义为 1。改变任务优先级可能会导致立即的上下文切换。

### 1.2 任务信息获取函数

- **`uxTaskGetSystemState()`**
  
  - **描述：** 获取系统中所有任务的状态信息，包括任务句柄、任务名称、优先级、堆栈使用情况等。这些信息存储在一个数组中。
  - **函数原型：** `UBaseType_t uxTaskGetSystemState( TaskStatus_t *pxTaskStatusArray, UBaseType_t uxArraySize, uint32_t *pulTotalRunTime );`
  - **参数：**
    - `pxTaskStatusArray` - 指向 `TaskStatus_t` 结构体数组的指针，用于存储任务状态信息。
    - `uxArraySize` - `pxTaskStatusArray` 数组的大小（即可以容纳多少个任务的状态信息）。
    - `pulTotalRunTime` - 可选参数，指向一个 `uint32_t` 变量的指针，用于返回自启动以来所有任务的总运行时间（如果 `configGENERATE_RUNTIME_STATS` 为 1）。
  - **返回值：** 获取到的任务数量。
  - **注意事项：** 要使此函数可用，`configGENERATE_RUNTIME_STATS` 需在 `FreeRTOSConfig.h` 中定义为 1。此函数会占用较长时间，因为它需要遍历所有任务。

- **`vTaskGetInfo()`**
  
  - **描述：** 获取单个任务的详细状态信息。
  - **函数原型：** `void vTaskGetInfo( TaskHandle_t xTask, TaskStatus_t *pxTaskStatus, BaseType_t xGetStackHighWaterMark, eTaskState eGetState );`
  - **参数：**
    - `xTask` - 要获取信息的任务句柄。
    - `pxTaskStatus` - 指向 `TaskStatus_t` 结构体的指针，用于存储任务信息。
    - `xGetStackHighWaterMark` - 如果为 `pdTRUE`，则计算并返回任务堆栈的历史剩余最小值。
    - `eGetState` - 如果为 `pdTRUE`，则返回任务的当前状态。
  - **注意事项：** 要使此函数可用，`INCLUDE_vTaskGetInfo` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xTaskGetApplicationTaskTag()`**
  
  - **描述：** 获取任务的应用层 Tag 值。Tag 是一个用户定义的指针，可以与任务关联，用于存储自定义数据或上下文。
  - **函数原型：** `TaskHookFunction_t xTaskGetApplicationTaskTag( TaskHandle_t xTask );`
  - **参数：** `xTask` - 任务句柄。如果传入 `NULL`，则返回当前任务的 Tag。
  - **返回值：** 任务的应用层 Tag 值。
  - **注意事项：** 要使此函数可用，`configUSE_APPLICATION_TASK_TAG` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xTaskGetCurrentTaskHandle()`**
  
  - **描述：** 获取当前正在运行的任务的句柄。
  - **函数原型：** `TaskHandle_t xTaskGetCurrentTaskHandle( void );`
  - **返回值：** 当前任务的句柄。
  - **注意事项：** 总是可用的，不需要额外配置。

- **`xTaskGetHandle()`**
  
  - **描述：** 根据任务名称获取任务的句柄。
  - **函数原型：** `TaskHandle_t xTaskGetHandle( const char *pcNameToQuery );`
  - **参数：** `pcNameToQuery` - 要查询的任务名称。
  - **返回值：** 如果找到匹配的任务，则返回任务句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configINCLUDE_QUERY_HEAP_INFO` 需在 `FreeRTOSConfig.h` 中定义为 1。任务名称必须是唯一的。

- **`xTaskGetIdleTaskHandle()`**
  
  - **描述：** 获取空闲任务的句柄。
  - **函数原型：** `TaskHandle_t xTaskGetIdleTaskHandle( void );`
  - **返回值：** 空闲任务的句柄。
  - **注意事项：** 总是可用的，不需要额外配置。

- **`uxTaskGetStackHighWaterMark()`**
  
  - **描述：** 获取指定任务的堆栈历史剩余最小值（“高水位线”）。这个值表示任务运行期间堆栈的最小空闲空间。
  - **函数原型：** `UBaseType_t uxTaskGetStackHighWaterMark( TaskHandle_t xTask );`
  - **参数：** `xTask` - 任务句柄。如果传入 `NULL`，则返回调用此函数的任务的堆栈高水位线。
  - **返回值：** 堆栈历史剩余最小值，以字为单位。
  - **注意事项：** 要使此函数可用，`INCLUDE_uxTaskGetStackHighWaterMark` 需在 `FreeRTOSConfig.h` 中定义为 1。这个值对于调试堆栈溢出非常有用。

- **`eTaskGetState()`**
  
  - **描述：** 获取指定任务的当前状态（例如：就绪、运行、阻塞、挂起、删除）。
  - **函数原型：** `eTaskState eTaskGetState( TaskHandle_t xTask );`
  - **参数：** `xTask` - 任务句柄。
  - **返回值：** 任务的当前状态，类型为 `eTaskState` 枚举。
  - **注意事项：** 要使此函数可用，`INCLUDE_eTaskGetState` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`pcTaskGetName()`**
  
  - **描述：** 获取指定任务的名称。
  - **函数原型：** `char *pcTaskGetName( TaskHandle_t xTaskToQuery );`
  - **参数：** `xTaskToQuery` - 任务句柄。如果传入 `NULL`，则返回当前任务的名称。
  - **返回值：** 指向任务名称字符串的指针。
  - **注意事项：** 任务名称字符串是只读的。要使此函数可用，`INCLUDE_pcTaskGetName` 需在 `FreeRTOSConfig.h` 中定义为 1。

### 1.3 系统时间与调度器状态相关函数

- **`xTaskGetTickCount()`**
  
  - **描述：** 获取 FreeRTOS 系统自启动以来的时钟节拍计数器的值。
  - **函数原型：** `TickType_t xTaskGetTickCount( void );`
  - **返回值：** 系统时钟节拍计数器的当前值。
  - **注意事项：** 此函数通常用于实现延时和超时机制。

- **`xTaskGetTickCountFromISR()`**
  
  - **描述：** 在中断服务例程 (ISR) 中获取 FreeRTOS 系统自启动以来的时钟节拍计数器的值。这是 `xTaskGetTickCount()` 的中断安全版本。
  - **函数原型：** `TickType_t xTaskGetTickCountFromISR( void );`
  - **返回值：** 系统时钟节拍计数器的当前值。
  - **注意事项：** 只能在 ISR 中调用。

- **`xTaskGetSchedulerState()`**
  
  - **描述：** 获取 FreeRTOS 调度器的当前状态。
  - **函数原型：** `BaseType_t xTaskGetSchedulerState( void );`
  - **返回值：** 调度器状态（例如：调度器已启动、调度器已挂起）。
  - **注意事项：** 总是可用的，不需要额外配置。

- **`uxTaskGetNumberOfTasks()`**
  
  - **描述：** 获取系统中当前存在的任务数量（包括空闲任务）。
  - **函数原型：** `UBaseType_t uxTaskGetNumberOfTasks( void );`
  - **返回值：** 系统中任务的总数量。
  - **注意事项：** 总是可用的，不需要额外配置。

### 1.4 调试与统计相关函数

- **`vTaskList()`**
  
  - **描述：** 以格式化的字符串（“表格”形式）获取所有任务的信息，包括任务名称、状态、优先级、堆栈使用情况等，以便于调试和监控。
  - **函数原型：** `void vTaskList( char *pcWriteBuffer );`
  - **参数：** `pcWriteBuffer` - 指向字符缓冲区的指针，用于存储格式化后的任务信息。
  - **注意事项：** 要使此函数可用，`configUSE_STATS_FORMATTING_FUNCTIONS` 和 `configSUPPORT_DYNAMIC_ALLOCATION` 需在 `FreeRTOSConfig.h` 中定义为 1。此函数会占用较长时间，且需要足够大的缓冲区。

- **`vTaskGetRunTimeStats()`**
  
  - **描述：** 获取所有任务的运行时间统计信息，前提是已配置了运行时间统计功能。
  - **函数原型：** `void vTaskGetRunTimeStats( char *pcWriteBuffer );`
  - **参数：** `pcWriteBuffer` - 指向字符缓冲区的指针，用于存储格式化后的运行时间统计信息。
  - **注意事项：** 要使此函数可用，`configGENERATE_RUNTIME_STATS` 和 `configUSE_STATS_FORMATTING_FUNCTIONS` 需在 `FreeRTOSConfig.h` 中定义为 1，并且需要一个硬件定时器来提供运行时间计数。

### 1.5 任务本地存储与 Tag 相关函数

- **`vTaskSetApplicationTaskTag()`**
  
  - **描述：** 设置指定任务的应用层 Tag 值。
  - **函数原型：** `void vTaskSetApplicationTaskTag( TaskHandle_t xTask, TaskHookFunction_t pxHookFunction );`
  - **参数：**
    - `xTask` - 任务句柄。如果传入 `NULL`，则设置当前任务的 Tag。
    - `pxHookFunction` - 要设置的 Tag 值（通常是一个函数指针或任意指针）。
  - **注意事项：** 要使此函数可用，`configUSE_APPLICATION_TASK_TAG` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`SetThreadLocalStoragePointer()`**
  
  - **描述：** 设置任务的线程本地存储 (TLS) 指针。FreeRTOS 为每个任务提供了一个独立的 TLS 数组，每个元素可以存储一个指向任意数据的指针。
  - **函数原型：** `void SetThreadLocalStoragePointer( TaskHandle_t xTaskToSet, BaseType_t xIndex, void *pvValue );`
  - **参数：**
    - `xTaskToSet` - 要设置 TLS 指针的任务句柄。如果传入 `NULL`，则设置当前任务的 TLS 指针。
    - `xIndex` - TLS 数组的索引。
    - `pvValue` - 要存储的指针值。
  - **注意事项：** 要使此函数可用，`configTHREAD_LOCAL_STORAGE_POINTERS` 需在 `FreeRTOSConfig.h` 中定义为大于 0。

- **`GetThreadLocalStoragePointer()`**
  
  - **描述：** 获取任务的线程本地存储 (TLS) 指针。
  - **函数原型：** `void *GetThreadLocalStoragePointer( TaskHandle_t xTaskToQuery, BaseType_t xIndex );`
  - **参数：**
    - `xTaskToQuery` - 要查询 TLS 指针的任务句柄。如果传入 `NULL`，则查询当前任务的 TLS 指针。
    - `xIndex` - TLS 数组的索引。
  - **返回值：** 存储在指定 TLS 索引处的指针值。
  - **注意事项：** 要使此函数可用，`configTHREAD_LOCAL_STORAGE_POINTERS` 需在 `FreeRTOSConfig.h` 中定义为大于 0。

## 2. 任务状态与信息查询测试

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
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t i = 0;
    UBaseType_t task_num = 0;
    TaskStatus_t *status_array = NULL;
    TaskHandle_t task_handle = NULL;
    TaskStatus_t *task_info = NULL;
    eTaskState task_state = eInvalid;
    char *task_state_str = NULL;
    char *task_info_buf = NULL;

    /*--------------------函数uxTaskGetSystemState()的使用------==--------------*/
    printf("-------------Step1 函数uxTaskGetSystemState()的使用-------------\r\n");
    task_num = uxTaskGetNumberOfTasks(); // 获取系统中任务数量
    status_array = mymalloc(SRAMIN, task_num*sizeof(TaskStatus_t)); // 内存分配
    task_num = uxTaskGetSystemState((TaskStatus_t*)status_array, // 任务状态信息
                                    (UBaseType_t)task_num, // buffer大小
                                    (uint32_t*)NULL); // 不获取任务运行时间信息
    printf("任务名\t\t优先级\t\t任务编号\r\n");
    for(i=0;i<task_num;i++)
    {
        printf("%s\t%s%ld\t\t%ld\r\n",
        status_array[i].pcTaskName,
        strlen(status_array[i].pcTaskName) > 7 ? "": "\t",
        status_array[i].uxCurrentPriority,
        status_array[i].xTaskNumber);
    }   
    myfree(SRAMIN, status_array);     
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*-------------------------函数vTaskGetInfo()的使用--------------------------*/
    printf("-----------------Step2 函数vTaskGetInfo()的使用------------------\r\n");
    task_info = mymalloc(SRAMIN, sizeof(TaskStatus_t)); // 内存分配
    task_handle = xTaskGetHandle("task1"); // 获取任务句柄
    vTaskGetInfo((TaskHandle_t)task_handle, // 任务句柄
                 (TaskStatus_t*)task_info,  // 任务状态信息
                 (BaseType_t)pdTRUE,        // 允许统计任务堆栈历史最小值
                 (eTaskState)eInvalid);     // 任务状态
    printf("任务名:\t\t\t%s\r\n", task_info->pcTaskName);
    printf("任务编号:\t\t%ld\r\n", task_info->xTaskNumber);
    printf("任务壮态:\t\t%d\r\n", task_info->eCurrentState);
    printf("任务当前优先级:\t\t%ld\r\n", task_info->uxCurrentPriority);
    printf("任务基优先级:\t\t%ld\r\n", task_info->uxBasePriority);
    printf("任务堆栈基地址:\t\t0x%p\r\n", task_info->pxStackBase);
    printf("任务堆栈历史剩余最小值:\t%d\r\n", task_info->usStackHighWaterMark);
    myfree(SRAMIN, task_info);
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*-------------------------函数eTaskGetState()的使用--------------------------*/
    printf("-----------------Step3 函数eTaskGetState()的使用------------------\r\n");
    task_state_str = mymalloc(SRAMIN, 10); // 内存分配
    task_handle = xTaskGetHandle("task1");
    task_state = eTaskGetState(task_handle);
    sprintf(task_state_str, task_state == eRunning ? "Runing" :
                            task_state == eReady ? "Ready" :
                            task_state == eBlocked ? "Blocked" :
                            task_state == eSuspended ? "Suspended" :
                            task_state == eDeleted ? "Deleted" :
                            task_state == eInvalid ? "Invalid" :
                                                     "");
    printf("任务状态值: %d，对应状态为: %s\r\n", task_state, task_state_str);
    myfree(SRAMIN, task_state_str);
    printf("-----------------------------END-------------------------------\r\n");
    printf("按下KEYO以继续！！！\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    /*---------------------------函数vTaskList()的使用--------------------------*/
    printf("------------------Step4 函数vTaskList()的使用-------------------\r\n");
    task_info_buf = mymalloc(SRAMIN, 500);
    vTaskList(task_info_buf); // 打印任务列表
    printf("任务名\t\t状态\t优先级\t剩余栈\t任务序号\r\n");
    printf("%s\r\n", task_info_buf);
    myfree(SRAMIN, task_info_buf);
    printf("-----------------------------END-------------------------------\r\n");
    while(1)
    {
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/
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

## 3. 任务运行时间统计测试

### 3.1 定时器配置

```c
#include "btim.h"
#include "FreeRTOS.h"

TIM_HandleTypeDef g_tim3_handle;        /* 定时器3句柄 */

uint32_t FreeRTOSRunTimeTicks;          /* FreeRTOS时间统计所用的节拍计数器 */

void ConfigureTimeForRunTimeStats(void)
{
    FreeRTOSRunTimeTicks = 0;           /* 节拍计数器初始化为0 */
    btim_tim3_int_init(10-1, 2400-1);   /* 初始化TIM3 */
}

/**
 * @brief       基本定时器TIM3定时中断初始化函数
 * @note
 *              基本定时器的时钟来自APB1,当D2PPRE1≥2分频的时候
 *              基本定时器的时钟为APB1时钟的2倍, 而APB1为120M, 所以定时器时钟 = 240Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void btim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    g_tim3_handle.Instance = BTIM_TIM3_INT;                      /* 通用定时器3 */
    g_tim3_handle.Init.Prescaler = psc;                          /* 分频 */
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;         /* 向上计数器 */
    g_tim3_handle.Init.Period = arr;                             /* 自动装载值 */
    g_tim3_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;   /* 时钟分频因子 */
    HAL_TIM_Base_Init(&g_tim3_handle);

    HAL_TIM_Base_Start_IT(&g_tim3_handle);   /* 使能定时器3和定时器3更新中断 */
}

/**
 * @brief       定时器底层驱动，开启时钟，设置中断优先级
                此函数会被HAL_TIM_Base_Init()函数调用
 * @param       无
 * @retval      无
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == BTIM_TIM3_INT)
    {
        BTIM_TIM3_INT_CLK_ENABLE();                      /* 使能TIM3时钟 */
        HAL_NVIC_SetPriority(BTIM_TIM3_INT_IRQn, 4, 0);  /* 设置中断优先级，抢占优先级4，子优先级0 */
        HAL_NVIC_EnableIRQ(BTIM_TIM3_INT_IRQn);          /* 开启ITM3中断 */
    }
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void BTIM_TIM3_INT_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);
}

/**
 * @brief       定时器更新中断回调函数
 * @param       htim:定时器句柄指针
 * @retval      无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim == (&g_tim3_handle))
    {
        FreeRTOSRunTimeTicks++;
    }
}
```

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

// TASK3配置
#define TASK3_PRIO      4                   
#define TASK3_STK_SIZE  128                 
TaskHandle_t Task3Task_Handler;          
void task3(void *pvParameters);                
/*-------------------------------------------*/
```

### 3.3 任务实现

```c
/*------------------任务实现区----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Get Run Time Stats",RED);
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
    vTaskDelete(StartTask_Handler); // 开始任务已经完成自己使命，删除自己
    taskEXIT_CRITICAL(); // 退出临界区
}

// task1函数实现
void task1(void *pvParameters)
{
    uint32_t task1_num = 0;
    while(1)
    {
        lcd_fill(6,131,114,313,lcd_discolor[++task1_num%11]);
        lcd_show_xnum(71,111,task1_num,3,16,0x80,BLUE);
        vTaskDelay(1000);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint32_t task2_num = 0;
    while(1)
    {
        lcd_fill(126,131,233,313,lcd_discolor[11-(++task2_num%11)]);
        lcd_show_xnum(191,111,task2_num,3,16,0x80,BLUE);
        vTaskDelay(1000);
    }
}

// task3函数实现
void task3(void *pvParameters)
{
    uint8_t key_value = 0;
    char *runtime_info = NULL;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                runtime_info = mymalloc(SRAMIN ,100);
                vTaskGetRunTimeStats(runtime_info);
                printf("任务名\t\t运行时间\t运行所占百分比\r\n");
                printf("%s\r\n", runtime_info);
                myfree(SRAMIN, runtime_info);
                break;
            }
            default:
            {
                break;
            }
        }
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/
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
