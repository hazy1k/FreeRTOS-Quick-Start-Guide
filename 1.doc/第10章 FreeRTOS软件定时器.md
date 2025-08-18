# 第十章 FreeRTOS软件定时器

## 1. 软件定时器简介

软件定时器是指具有定时功能的软件， FreeRTOS 提供的软件定时器允许在创建前设置一个软件定时器定时超时时间，在软件定时器成功创建并启动后，软件定时器开始定时，当软件定时器的定时时间达到或超过先前设置好的软件定时器定时器超时时间时，软件定时器就处于超时状态，此时软件定时器就会调用相应的回调函数，一般这个回调函数的处理的事务就是需要周期处理的事务。

FreeRTOS 提供的软件定时器还能够根据需要设置成单次定时器和周期定时器。当单次定时器定时超时后，不会自动启动下一个周期的定时，而周期定时器在定时超时后，会自动地启动下一个周期的定时。

FreeRTOS 提供的软件定时器功能，属于 FreeRTOS 的中可裁剪可配置的功能， 如果要使能软件定时器功能，那需要在 FreeRTOSConfig.h 文件中将 configUSE_TIMERS 配置项配置成 1。

要注意的是，软件定时器的超时回调函数是由软件定时器服务任务调用的，软件定定时器的超时回调函数本身不是任务，因此不能在该回调函数中使用可能会导致任务阻塞的 API 函数，例如 vTaskDelay()、 vTaskDelayUntil()和一些会到时任务阻塞的等到事件函数，这些函数将会导致软件定时器服务任务阻塞，这是不可以出现的。

### 1.1 软件定时器服务任务简介

使能了软件定时器功能后，在调用函数 vTaskStartScheduler()开启任务调度器的时候，会创建一个用于管理软件定时器的任务，这个任务就叫做软件定时器服务任务。软件定时器服务任务，主要负责软件定时器超时的逻辑判断、调用超时软件定时器的超时回调函数以及处理软件定时器命令队列。

### 1.2 软件定时器命令队列

FreeRTOS 提供了许多软件定时器相关的 API 函数，这些 API 函数，大部分都是往定时器的队列中写入消息（发送命令），这个队列叫做软件定时器命令队列，是提供给 FreeRTOS 中的软件定时器使用的，用户是不能直接访问的。软件定时器命令队列的操作过程如下图所示：

![屏幕截图 2025-08-18 120132.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-12-01-49-屏幕截图%202025-08-18%20120132.png)

上图中，左侧的代码为应用程序中用户任务的代码，而右侧的代码为软件定时器服务任务的代码。当用户任务需要操作软件定时器时，就需要调用软件定时器相关的 API 函数，例如图中调用了函数 vTaskStart()启动软件定时器的定时，而函数 vTaskStart()实际上会往软件定时器命令队列写入一条消息（发送命令），这条消息就包含了待操作的定时器对象以及操作的命令（启动软件定时器），软件定时器服务任务就会去读取软件定时器命令队列中的消息（接收命令），并处理这些消息（处理命令）。可以看出，用户任务并不会直接操作软件定时器对象，而是发送命令给软件定时器服务任务，软件定时器服务任务接收到命令后，根据命令内容去操作软件定时器。

### 1.3 软件定时器的状态

软件定时器可以处于一下两种状态中一种：

1. 休眠态

休眠态软件定时器可以通过其句柄被引用，但是因为没有运行，所以其定时超时回调函数不会被执行。

2. 运行态

处于运行态或在上次定时超时后再次定时超时的软件定时器，会执行其定时超时回调函数。

### 1.4 单次定时器和周期定时器

FreeRTOS 提供了两种软件定时器，如下：

1. 单次定时器

单次定时器的一旦定时超时，只会执行一次其软件定时器超时回调函数，超时后可以被手动重新开启，但单次定时器不会自动重新开启定时。

2. 周期定时器

周期定时器的一旦被开启，会在每次超时时，自动地重新启动定时器，从而周期地执行其软件定时器回调函数。

单次定时器和周期定时器之间的差异如下图所示：

![屏幕截图 2025-08-18 120255.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-12-03-00-屏幕截图%202025-08-18%20120255.png)

上图展示了单次定时器和周期定时器之间的差异，图中的垂直虚线的间隔时间为一个单位时间，可以理解为一个系统时钟节拍。其中 Timer1 为周期定时器，定时超时时间为 2 个单位时间， Timer2 为单次定时器，定时超时时间为 1 个单位时间。可以看到， Timer1 在开启后，一直以 2 个时间单位的时间间隔重复执行，为 Timer2 则在第一个超时后就不在执行了。

### 1.5 软件定时器的状态转换图

单次定时器的状态转化图，如下图所示：

![屏幕截图 2025-08-18 120338.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-12-04-11-屏幕截图%202025-08-18%20120338.png)

周期定时器的状态转换图，如下图所示：

![屏幕截图 2025-08-18 120422.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-12-04-26-屏幕截图%202025-08-18%20120422.png)

### 1.6 复位软件定时器

除了开启和停止软件定时器的定时，还可以对软件定时器进行复位。复位软件定时器会使软件定时器的重新开启定时，复位后的软件定时器以复位时的时刻作为开启时刻重新定时，软件定时器的复位示意图如下图所示：

![屏幕截图 2025-08-18 120449.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/18-12-04-53-屏幕截图%202025-08-18%20120449.png)

上图展示了软件定时器的复位过程，图中在 t0 时刻创建并启动了一个超时时间为 5 个单位时间的软件定时器，接着在 t3 时刻对软件定时器进行了复位，复位后软件定时器的超时时刻以复位时刻为开启时刻重新计算，在 t7 时刻又再次对软件定时器进行了复位，最终计算出软件定时器的超时时刻为最后一次复位的时刻（t7）加上软件定时器的超时时间（5 个单位时间），于是该软件定时器在 t12 时刻超时，并执行其超时回调函数。

## 2. 软件定时器相关配置

1. configUSE_TIMERS

此宏用于使能软件定时器功能，如果要使用软件定时器功能，则需要将该宏定义定义为 1。开启软件定时器功能后，系统会系统创建软件定时器服务任务。

2. configTIMER_TASK_PRIORITY

此宏用于配置软件定时器服务任务的任务优先级，当使能了软件定时器功能后，需要配置该宏定义，此宏定义可以配置为 0~(configMAX_PRIORITY-1)的任意值。

3. configTIMER_QUEUE_LENGTH

此宏用于配置软件定时器命令队列的队列长度，当使能了软件定时器功能后，需要配置该宏定义，若要正常使用软件定时器功能，此宏定义需定义成一个大于 0 的值。

4. configTIMER_TASK_STACK_DEPTH

此宏用于配置软件定时器服务任务的栈大小，当使能了软件定时器功能后，需要配置该宏定义，由于所有软件定时器的定时器超时回调函数都是由软件定时器服务任务调用的，因此这些软件定时器超时回调函数运行时使用的都是软件定时器服务任务的栈。

## 3. 软件定时器相关函数

| 函数                            | 描述                 |
| ----------------------------- | ------------------ |
| `xTimerCreate()`              | 动态方式创建软件定时器        |
| `xTimerCreateStatic()`        | 静态方式创建软件定时器        |
| `xTimerStart()`               | 开启软件定时器定时          |
| `xTimerStartFromISR()`        | 在中断中开启软件定时器定时      |
| `xTimerStop()`                | 停止软件定时器定时          |
| `xTimerStopFromISR()`         | 在中断中停止软件定时器定时      |
| `xTimerReset()`               | 复位软件定时器定时          |
| `xTimerResetFromISR()`        | 在中断中复位软件定时器定时      |
| `xTimerChangePeriod()`        | 更改软件定时器的定时超时时间     |
| `xTimerChangePeriodFromISR()` | 在中断中更改软件定时器的定时超时时间 |
| `xTimerDelete()`              | 删除软件定时器            |

### 3.1 创建软件定时器

- **`xTimerCreate()`**
  
  - **描述：** 动态方式创建软件定时器。定时器所需的内存从 FreeRTOS 堆中分配。
  - **函数原型：** `TimerHandle_t xTimerCreate( const char * const pcTimerName, TickType_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void *pvTimerID, TimerCallbackFunction_t pxCallbackFunction );`
  - **参数：**
    - `pcTimerName`: 定时器的名称，用于调试。
    - `xTimerPeriodInTicks`: 定时器的周期（或单次超时时间），以节拍为单位。
    - `uxAutoReload`: 如果设置为 `pdTRUE`，定时器将周期性地触发；如果设置为 `pdFALSE`，定时器将只触发一次。
    - `pvTimerID`: 用户定义的 ID，可以用于区分不同的定时器或传递上下文信息给回调函数。
    - `pxCallbackFunction`: 定时器超时时将被调用的回调函数指针。
  - **返回值：** 如果定时器成功创建，则返回一个定时器句柄（`TimerHandle_t` 类型）；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configSUPPORT_DYNAMIC_ALLOCATION` 需在 `FreeRTOSConfig.h` 中定义为 1。

- **`xTimerCreateStatic()`**
  
  - **描述：** 静态方式创建软件定时器。定时器所需的内存由用户在编译时或运行时提供。
  - **函数原型：** `TimerHandle_t xTimerCreateStatic( const char * const pcTimerName, TickType_t xTimerPeriodInTicks, UBaseType_t uxAutoReload, void *pvTimerID, TimerCallbackFunction_t pxCallbackFunction, StaticTimer_t *pxTimerBuffer );`
  - **参数：**
    - 前五个参数与 `xTimerCreate()` 相同。
    - `pxTimerBuffer`: 指向用户提供的 `StaticTimer_t` 结构体变量的指针，该结构体将用作定时器的控制块。
  - **返回值：** 如果定时器成功创建，则返回一个定时器句柄；否则返回 `NULL`。
  - **注意事项：** 要使此函数可用，`configSUPPORT_STATIC_ALLOCATION` 需在 `FreeRTOSConfig.h` 中定义为 1。用户需要自行管理 `pxTimerBuffer` 所指向的内存生命周期。

### 3.2 控制软件定时器

- **`xTimerStart()`**
  
  - **描述：** 启动一个软件定时器。如果定时器已经是活动状态，此函数会复位定时器（使其从头开始计数）。
  - **函数原型：** `BaseType_t xTimerStart( TimerHandle_t xTimer, TickType_t xBlockTime );`
  - **参数：**
    - `xTimer`: 要启动的定时器句柄。
    - `xBlockTime`: 如果定时器命令队列已满，调用任务愿意阻塞等待的节拍数。通常设置为 `0` 或较小值，因为定时器命令是发送给守护任务的，通常不会阻塞太久。
  - **返回值：** `pdPASS` 表示命令成功发送到守护任务；`pdFALSE` 表示命令发送失败（通常是超时）。

- **`xTimerStartFromISR()`**
  
  - **描述：** 在中断中启动一个软件定时器。此函数不会阻塞。
  - **函数原型：** `BaseType_t xTimerStartFromISR( TimerHandle_t xTimer, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xTimer`: 要启动的定时器句柄。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。如果调用此函数导致更高优先级的任务（通常是守护任务）解除阻塞，则该变量将被设置为 `pdTRUE`。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`。

- **`xTimerStop()`**
  
  - **描述：** 停止一个软件定时器。
  - **函数原型：** `BaseType_t xTimerStop( TimerHandle_t xTimer, TickType_t xBlockTime );`
  - **参数：**
    - `xTimer`: 要停止的定时器句柄。
    - `xBlockTime`: 同 `xTimerStart()`。
  - **返回值：** 同 `xTimerStart()`。

- **`xTimerStopFromISR()`**
  
  - **描述：** 在中断中停止一个软件定时器。
  - **函数原型：** `BaseType_t xTimerStopFromISR( TimerHandle_t xTimer, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 同 `xTimerStartFromISR()`。

- **`xTimerReset()`**
  
  - **描述：** 复位一个软件定时器。无论定时器当前是否活动，此函数都会停止它（如果正在运行），并将其内部计数器重置为零，然后重新启动它。这有效地“重置”了定时器的超时周期。
  - **函数原型：** `BaseType_t xTimerReset( TimerHandle_t xTimer, TickType_t xBlockTime );`
  - **参数、返回值及注意事项** 同 `xTimerStart()`。

- **`xTimerResetFromISR()`**
  
  - **描述：** 在中断中复位一个软件定时器。
  - **函数原型：** `BaseType_t xTimerResetFromISR( TimerHandle_t xTimer, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 同 `xTimerStartFromISR()`。

- **`xTimerChangePeriod()`**
  
  - **描述：** 更改软件定时器的定时超时时间（周期）。此函数会停止定时器（如果正在运行），更新其周期，然后重新启动它。
  - **函数原型：** `BaseType_t xTimerChangePeriod( TimerHandle_t xTimer, TickType_t xNewPeriod, TickType_t xBlockTime );`
  - **参数：**
    - `xTimer`: 要更改周期的定时器句柄。
    - `xNewPeriod`: 新的定时器周期，以节拍为单位。
    - `xBlockTime`: 同 `xTimerStart()`。
  - **返回值：** 同 `xTimerStart()`。

- **`xTimerChangePeriodFromISR()`**
  
  - **描述：** 在中断中更改软件定时器的定时超时时间。
  - **函数原型：** `BaseType_t xTimerChangePeriodFromISR( TimerHandle_t xTimer, TickType_t xNewPeriod, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 同 `xTimerStartFromISR()`。

### 3.3 删除软件定时器

- **`xTimerDelete()`**
  - **描述：** 删除一个软件定时器，并释放其占用的内存（如果它是动态创建的）。
  - **函数原型：** `BaseType_t xTimerDelete( TimerHandle_t xTimer, TickType_t xBlockTime );`
  - **参数：**
    - `xTimer`: 要删除的定时器句柄。
    - `xBlockTime`: 同 `xTimerStart()`。
  - **返回值：** 同 `xTimerStart()`。
  - **注意事项：** 只能删除动态创建的定时器。删除后，不应再使用该句柄。

## 4. 软件定时器测试

### 4.1 任务配置

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

TimerHandle_t TIM1_Handler; // 软件定时器句柄
TimerHandle_t TIM2_Handler;
void Timer1_Callback(TimerHandle_t xTimer); // 软件定时器回调函数
void Timer2_Callback(TimerHandle_t xTimer);
/*---------------------------------------------*/
```

### 4.2 任务实现

```c
/*------------------任务实现区------------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"Timer",RED);
    lcd_draw_rectangle(5, 110, 115, 314, BLACK);
    lcd_draw_rectangle(125, 110, 234, 314, BLACK);
    lcd_draw_line(5, 130, 115, 130, BLACK);
    lcd_draw_line(125, 130, 234, 130, BLACK);
    lcd_show_string(15, 111, 110, 16, 16, "Timer1: 000", BLUE);
    lcd_show_string(135, 111, 110, 16, 16, "Timer2: 000", BLUE);
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
    // TIM1为周期定时器
    TIM1_Handler = xTimerCreate((const char*)"TIM1",                      // 定时器名字
                                (TickType_t)1000,                         // 定时器超时时间
                                (UBaseType_t)pdTRUE,                      // 周期性定时器
                                (void*)1,                                 // 定时器ID
                                (TimerCallbackFunction_t)Timer1_Callback);// 定时器回调函数
    // TIM2为单次定时器
    TIM2_Handler = xTimerCreate((const char*)"TIM2",                     
                                (TickType_t)1000,                         
                                (UBaseType_t)pdFALSE,                     
                                (void*)2,                                 
                                (TimerCallbackFunction_t)Timer2_Callback);
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
    uint8_t key_value = 0;
    while(1)
    {
        if((TIM1_Handler != NULL) && (TIM2_Handler != NULL))
        {
            key_value = key_scan(0);
            switch(key_value)
            {
                case KEY0_PRES: // KEY0启动软件定时器
                {
                    xTimerStart((TimerHandle_t)TIM1_Handler,// 待启动的定时器
                                (TickType_t)portMAX_DELAY); // 等待系统启动定时器
                    xTimerStart((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                case KEY1_PRES: // KEY1停止软件定时器
                {
                    xTimerStop((TimerHandle_t)TIM1_Handler,
                                (TickType_t)portMAX_DELAY);
                    xTimerStop((TimerHandle_t)TIM2_Handler,
                                (TickType_t)portMAX_DELAY);
                    break;
                }
                default:break;
            }
        }
        vTaskDelay(10);
    }
}
```

### 4.3 定时器超时回调

```c
// Timer1_Callback函数实现
void Timer1_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim1_num = 0;
    lcd_fill(6, 131, 114, 313, lcd_discolor[++tim1_num % 11]);    /* LCD区域刷新 */
    lcd_show_xnum(79, 111, tim1_num, 3, 16, 0x80, BLUE);          /* 显示定时器1超时次数 */
}

// Timer2_Callback函数实现
void Timer2_Callback(TimerHandle_t xTimer)
{
    static uint32_t tim2_num = 0;
    lcd_fill(126, 131, 233, 313, lcd_discolor[++tim2_num % 11]);  /* LCD区域刷新 */
    lcd_show_xnum(199, 111, tim2_num, 3, 16, 0x80, BLUE);         /* 显示定时器2超时次数 */
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


