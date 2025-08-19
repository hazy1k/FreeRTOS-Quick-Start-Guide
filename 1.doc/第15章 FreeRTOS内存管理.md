# 第十五章 FreeRTOS内存管理

## 1. 内存管理简介

在使用 FreeRTOS 创建任务、队列、信号量等对象的时候， FreeRTOS 一般都提供了两种方法，一种是动态地申请创建对象时所需要的内存，这种方法也叫动态方法；一种是由用户自定义对象，在编译器编译程序的时候，会为已经在程序中定义好的对象分配一定的内存空间，这种方法也叫静态方法。

静态方法创建任务、队列、信号量等对象的 API 函数一般是以“Static”结尾的，例如静态创建任务的 API 函数 xTaskCreateStatic()。使用静态方式创建各种对象时，需要用户提供各种内存空间，例如任务的栈空间、任务控制块所用内存空间等等， 并且使用静态方式占用的内存空间一般固定下来了，即使任务、队列等被删除后，这些被占用的内存空间也没有其他用途。

在使用动态方式管理内存的时候， FreeRTOS 就能够在创建任务、队列、信号量等对象的时候，自动地从 FreeRTOS 管理的内存堆中申请所创建对象所需的内存，在对象被删除后，又可以将这块内存释放会 FreeRTOS 管理的内存堆，这样看来，动态方式管理内存相比与静态方式，显得灵活许多。

除了 FreeRTOS 提供的动态内存管理方法，标准的 C 库也提供了函数 malloc()和函数 free()来实现动态地申请和释放内存，但是标准 C 库的动态内存管理方法有如下几个缺点：

1. 并不适用于所有嵌入式系统。

2. 占用大量的代码空间。

3. 没有线程安全的相关机制。

4. 具有不确定性，体现在每次执行的时间不同。

5. ……

为此， FreeRTOS 提供了动态方式管理内存的方法。不同的嵌入式系统对于动态内存管理的需求不同，因此 FreeRTOS 提供了多种内存管理算法选项，并将其作为 FreeRTOS 移植层的一部分，这样一来， FreeRTOS 的使用者就能够根据自己实际的需求选的何时的动态内存管理算法，并将其移植到系统中。

FreeRTOS 一共提供了 5 种动态内存管理算法，这 5 种动态内存管理算法本别对应了 5 个C 源文件，分别为： heap_1.c、 heap_2.c、 heap_3.c、 heap_4.c、 heap_5.c， 在后面小节中将会讲解这 5 种动态内存管理算法的异同。

## 2. FreeRTOS 内存管理算法

FreeRTOS提供了5种动态内存管理算法，分别为heap_1、heap_2、heap_3、heap_4和heap_5，这 5 种动态内存管理算法各自的特点如下所示：

### 2.1 `heap_1.c`

- **特点：**
  - **最简单**的实现。
  - 只支持内存分配 (`pvPortMalloc()`)，**不支持内存释放 (`vPortFree()`)**。
  - 因此，不会产生内存碎片。
  - 分配速度快，是确定性的。
- **适用场景：**
  - 内存分配只在系统初始化阶段进行，或者应用程序中分配的内存永远不会被释放。
  - 适用于资源非常有限、对内存管理开销要求极高且可以预先确定所有内存需求的简单嵌入式系统。

### 2.2 `heap_2.c`

- **特点：**
  - 支持内存分配 (`pvPortMalloc()`) 和内存释放 (`vPortFree()`)。
  - 内存分配是确定性的，但**内存释放不是确定性的**，因为涉及搜索最佳匹配块。
  - 使用“最佳适配”（best fit）算法来查找分配块。
  - 释放内存时，不会自动合并相邻的空闲块，因此**可能导致内存碎片**。
- **适用场景：**
  - 早期的 FreeRTOS 版本中经常使用。
  - 适用于分配和释放操作数量有限，并且对内存碎片化容忍度较高，或者可以通过重启来解决碎片化问题的系统。
  - 相对于 `heap_4.c`，它在某些特定场景下可能会更快，但通常不作为首选。

### 2.3 `heap_3.c`

- **特点：**
  - 支持内存分配 (`pvPortMalloc()`) 和内存释放 (`vPortFree()`)。
  - **包装了标准库的 `malloc()` 和 `free()` 函数。**
  - 线程安全：通过互斥量（Mutex）保护对 `malloc()` 和 `free()` 的调用，使其在多任务环境中是安全的。
  - 性能和碎片化特性取决于底层标准库的实现。
- **适用场景：**
  - 当你的嵌入式系统有充足的 RAM 并且已经集成了标准 C 库的 `malloc()` 和 `free()`，并且希望利用这些成熟的实现时。
  - 开发者不希望自己管理 FreeRTOS 堆的复杂性。
  - 需要注意：标准库的 `malloc()` 和 `free()` 通常不是为实时系统设计的，可能存在非确定性行为和性能瓶颈。

### 2.4 `heap_4.c`

- **特点：**
  - 支持内存分配 (`pvPortMalloc()`) 和内存释放 (`vPortFree()`)。
  - **最常用且推荐的通用内存管理方案。**
  - 使用“首次适配”（first fit）算法来查找分配块。
  - **释放内存时，会自动合并相邻的空闲块**，从而有效减少内存碎片。
  - 分配和释放操作都不是严格确定性的（因为需要遍历空闲列表），但通常在大多数应用中性能良好。
- **适用场景：**
  - 大多数需要动态内存分配和释放的 FreeRTOS 应用程序。
  - 对内存碎片化有一定关注，希望堆空间能够高效重用的系统。
  - 需要相对平衡的性能和内存效率。

### 2.5 `heap_5.c`

- **特点：**
  - 支持内存分配 (`pvPortMalloc()`) 和内存释放 (`vPortFree()`)。
  - 与 `heap_4.c` 类似，也支持合并相邻空闲块以减少碎片。
  - 主要区别在于，`heap_5.c` 允许**将堆空间分布在多个不连续的内存区域**。
- **适用场景：**
  - 当硬件架构中 RAM 是非连续的，例如，部分 RAM 在内部，部分 RAM 在外部，或者 RAM 分布在不同的地址空间中。
  - 需要整合多个独立的内存块形成一个逻辑上的大堆。

### 2.6 总结选择指南

| 特性 / 算法   | `heap_1.c` | `heap_2.c` | `heap_3.c` | `heap_4.c` | `heap_5.c` |
| --------- | ---------- | ---------- | ---------- | ---------- | ---------- |
| **内存分配**  | 支持         | 支持         | 支持         | 支持         | 支持         |
| **内存释放**  | **不支持**    | 支持         | 支持         | 支持         | 支持         |
| **碎片化**   | 无          | 存在可能       | 取决于StdLib  | 较少（合并）     | 较少（合并）     |
| **确定性**   | 分配确定       | 分配确定       | 取决于StdLib  | 非确定        | 非确定        |
| **自动合并**  | N/A        | 否          | 取决于StdLib  | 是          | 是          |
| **不连续内存** | 否          | 否          | 否          | 否          | **是**      |
| **推荐度**   | 特殊场景       | 不推荐        | 仅限特定场景     | **最常用**    | 特殊硬件场景     |

在大多数 FreeRTOS 应用程序中，`heap_4.c` 是最常用且推荐的通用内存管理算法，因为它在内存碎片化和效率之间提供了很好的平衡。只有当有非常特定的需求（如不连续内存、绝对零碎片化或利用现有标准库）时，才考虑其他选项。

## 3. 内存管理测试

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
    lcd_show_string(10,47,220,24,24,"Memory Management",RED);    
    lcd_show_string(30, 118, 200, 16, 16, "Total Mem:      Bytes", RED);
    lcd_show_string(30, 139, 200, 16, 16, "Free  Mem:      Bytes", RED);
    lcd_show_string(30, 160, 200, 16, 16, "Malloc Addr:", RED);
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

void task1(void *pvParameters)
{
    uint8_t key_value = 0;
    uint8_t *buf = NULL;
    size_t free_size = 0;
    while(1)
    {
        key_value = key_scan(0);
        switch(key_value)
        {
            case KEY0_PRES:
            {
                buf = pvPortMalloc(30); // 分配30字节内存
                sprintf((char*)buf, "0x%p",buf);
                lcd_show_string(130,160,200,16,16,(char*)buf,BLUE);
                break;
            }
            case KEY1_PRES:
            {
                if(NULL != buf)
                {
                    vPortFree(buf); // 释放内存
                    buf = NULL;
                }
                break;
            }
            default:break;
        }
        lcd_show_xnum(114,118,configTOTAL_HEAP_SIZE,5,16,0,BLUE); // 显示总内存大小
        free_size = xPortGetFreeHeapSize(); // 获取空闲内存大小
        lcd_show_xnum(114,139,free_size,5,16,0,BLUE);
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


