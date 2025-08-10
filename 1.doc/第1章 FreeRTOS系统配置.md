# 第一章 FreeRTOS系统配置

## 1. FreeRTOSConfig.h文件

针对 FreeRTOSConfig.h 文件，在 FreeRTOS 官方的在线文档中有详细的说明，网址为： https://www.freertos.org/a00110.html

FreeRTOS 使用 FreeRTOSConfig.h 文件进行配置和裁剪。 FreeRTOSConfig.h 文件中有几十个配置项，这使得用户能够很好地配置和裁剪 FreeRTOS。

FreeRTOSConfig.h 文件中的配置项可分为三大类：“config”配置项、“INCLUDE”配置项和其他配置项，下面就为读者详细地讲解这三类配置项。

## 2. "config"配置项

“config”配置项按照配置的功能分类，可分为十类，分别为基础配置项、内存分配相关定义、钩子函数相关定义、运行时间和任务状态统计相关定义、协程相关定义、软件定时器相关定义、中断嵌套行为配置、断言、 FreeRTOS MPU 特殊定义和 ARMv8-M 安全侧端口相关定义。下面就分别介绍这写“config”配置项。

### 2.1 基础配置项

1. configUSE_PREEMPTION

此宏用于设置系统的调度方式。 当宏 configUSE_PREEMPTION 设置为 1 时，系统使用抢占式调度； 当宏 configUSE_PREEMPTION 设置为 0 时，系统使用协程式调度。抢占式调度和协程式调度的区别在于， 协程式调度是正在运行的任务主动释放 CPU 后才能切换到下一个任务，任务切换的时机完全取决于正在运行的任务。 协程式的优点在于可以节省开销，但是功能比较有限，现在的 MCU 性能都比较强大，建议使用抢占式调度。

2. configUSE_PORT_OPTIMISED_TASK_SELECTION

FreeRTOS 支持两种方法来选择下一个要执行的任务，分别为通用方法和特殊方法。

当宏 configUSE_PORT_OPTIMISED_TASK_SELECTION 设置为 0 时，使用通用方法。 通用方法是完全使用 C 实现的软件算法，因此支持所用硬件，并且不限制任务优先级的最大值，但效率相较于特殊方法低。

当宏 configUSE_PORT_OPTIMISED_TASK_SELECTION 设置为 1 时，使用特殊方法。特殊方法的效率相较于通用方法高，但是特殊方法依赖于一个或多个特定架构的汇编指令（一般是类似计算前导零[CLZ]的指令），因此特殊方法并不支持所有硬件，并且对任务优先级的最大值一般也有限制，通常为 32。

3. configUSE_TICKLESS_IDLE

当宏 configUSE_TICKLESS_IDLE 设置为 1 时，使能 tickless 低功耗模式；设置为 0 时， tick 中断则会移植运行。 tickless 低功耗模式并不适用于所有硬件。

4. configCPU_CLOCK_HZ

此宏应设置为 CPU 的内核时钟频率，单位为 Hz。

5. configSYSTICK_CLOCK_HZ

此宏应设置为 SysTick 的时钟频率，当 SysTick 的时钟源频率与内核时钟频率不同时才可以定义，单位为 Hz。

6. configTICK_RATE_HZ

此宏用于设置 FreeRTOS 系统节拍的中断频率，单位为 Hz。

7. configMAX_PRIORITIES

此 宏 用 于 定 义 系 统 支 持 的 最 大 任 务 优 先 级 数 量 ， 最 大 任 务 优 先 级 数 值 为configMAX_PRIORITIES-1。

8. configMINIMAL_STACK_SIZE

此宏用于设置空闲任务的栈空间大小，单位为 word。

9. configMAX_TASK_NAME_LEN

此宏用于设置任务名的最大字符数。

10. configUSE_16_BIT_TICKS

此宏用于定义系统节拍计数器的数据类型， 当宏 configUSE_16_BIT_TICKS 设置为 1 时，系统节拍计数器的数据类型为 16 位无符号整形； 当宏 configUSE_16_BIT_TICKS 设置为 0 时，系统节拍计数器的数据类型为 32 为无符号整型。

11. configIDLE_SHOULD_YIELD

当宏 configIDLE_SHOULD_YIELD 设置为 1 时，在抢占调度下，同等优先级的任务可抢占空闲任务，并延用空闲任务剩余的时间片。

12. configUSE_TASK_NOTIFICATIONS

当宏 configUSE_TASK_NOTIFICATIONS 设置为 1 时，开启任务通知功能。 当开启任务通知功能后，每个任务将多占用 8 字节的内存空间。

13. configTASK_NOTIFICATION_ARRAY_ENTRIES

此宏用于定义任务通知数组的大小。

14. configUSE_MUTEXES

此宏用于使能互斥信号量， 当宏 configUSE_MUTEXS 设置为 1 时，使能互斥信号量； 当宏configUSE_MUTEXS 设置为 0 时，则不使能互斥信号量。

15. configUSE_RECURSIVE_MUTEXES

此宏用于使能递归互斥信号量，当宏 configUSE_RECURSIVE_MUTEXES 设置为 1 时，使能递归互斥信号量；当宏 configUSE_RECURSIVE_MUTEXES 设置为 0 时，则不使能递归互斥信号量。

16. configUSE_COUNTING_SEMAPHORES

此宏用于使能计数型信号量， 当宏 configUSE_COUNTING_SEMAPHORES 设置为 1 时，使能计数型信号量； 当宏 configUSE_COUNTING_SEMAPHORES 设置为 0 时，则不使能计数型信号量。

17. configUSE_ALTERNATIVE_API

此宏在 FreeRTOS V9.0.0 之后已弃用。

18. configQUEUE_REGISTRY_SIZE

此宏用于定义可以注册的队列和信号量的最大数量。此宏定义仅用于调试使用。

19. configUSE_QUEUE_SETS

此宏用于使能队列集， 当宏 configUSE_QUEUE_SETS 设置为 1 时，使能队列集； 当宏configUSE_QUEUE_SETS 设置为 0 时，则不使能队列集。

20. configUSE_TIME_SLICING

此宏用于使能时间片调度， 当宏 configUSE_TIMER_SLICING 设置为 1 且使用抢占式调度时，使能时间片调度； 当宏 configUSE_TIMER_SLICING 设置为 0 时，则不使能时间片调度。

21. configUSE_NEWLIB_REENTRANT

此宏用于为每个任务分配一个 NewLib 重入结构体，当宏configUSE_NEWLIB_REENTRANT 设置为 1 时， FreeRTOS 将为每个创建的任务的任务控制块中分配一个 NewLib 重入结构体。

22. configENABLE_BACKWARD_COMPATIBILITY

此宏用于兼容 FreeRTOS 老版本的 API 函数。

23. configNUM_THREAD_LOCAL_STORAGE_POINTERS

此宏用于在任务控制块中分配一个线程本地存储指着数组，当此宏被定义为大于 0 时， configNUM_THREAD_LOCAL_STORAGE_POINTERS 为线程本地存储指针数组的元素个数；当宏 configNUM_THREAD_LOCAL_STORAGE_POINTERS 为 0 时，则禁用线程本地存储指针数组。

24. configSTACK_DEPTH_TYPE

此宏用于定义任务堆栈深度的数据类型，默认为 uint16_t。

25. configMESSAGE_BUFFER_LENGTH_TYPE

此宏用于定义消息缓冲区中消息长度的数据类型，默认为 size_t。

### 2.2 内存分配相关定义

1. configSUPPORT_STATIC_ALLOCATION

当宏 configSUPPORT_STSTIC_ALLOCATION 设置为 1 时， FreeRTOS 支持使用静态方式管理内存，此宏默认设置为 0。 如果将 configSUPPORT_STATIC_ALLOCATION 设置为 1，用户还 需 要 提 供 两 个 回 调 函 数 ： vApplicationGetIdleTaskMemory() 和vApplicationGetTimerTaskMemory()

2. configSUPPORT_DYNAMIC_ALLOCATION

当宏 configSUPPORT_DYNAMIC_ALLOCATION 设置为 1 时， FreeRTOS 支持使用动态方式管理内存，此宏默认设置为 1。

3. configTOTAL_HEAP_SIZE

此宏用于定义用于 FreeRTOS 动态内存管理的内存大小，即 FreeRTOS 的内存堆，单位为Byte。

4. configAPPLICATION_ALLOCATED_HEAP

此宏用于自定义 FreeRTOS 的内存堆， 当宏 configAPPLICATION_ALLOCATED_HEAP 设置为 1 时，用户需要自行创建 FreeRTOS 的内存堆，否则 FreeRTOS 的内存堆将由编译器进行分配。利用此宏定义，可以使用 FreeRTOS 动态管理外扩内存。

5. configSTACK_ALLOCATION_FROM_SEPARATE_HEAP

此宏用于自定义动态创建和删除任务时，任务栈内存的申请与释放函数pvPortMallocStack()和vPortFreeStack()， 当宏configSTACK_ALLOCATION_FROM_SEPARATE_HEAP 设置为1是，用户需提供 pvPortMallocStack()和 vPortFreeStack()函数。

### 2.3 钩子函数相关定义

1. configUSE_IDLE_HOOK

此宏用于使能使用空闲任务钩子函数， 当宏 configUSE_IDLE_HOOK 设置为 1 时，使能使用空闲任务钩子函数，用户需自定义相关钩子函数； 当宏 configUSE_IDLE_HOOK 设置为 0 时，则不使能使用空闲任务钩子函数。

2. configUSE_TICK_HOOK

此宏用于使能使用系统时钟节拍中断钩子函数， 当宏 configUSE_TICK_HOOK 设置为 1 时，使能使用系统时钟节拍中断钩子函 数，用户需自定义相关钩子函数；当 宏configUSE_TICK_HOOK 设置为 0 时， 则不使能使用系统时钟节拍中断钩子函数。

3. configCHECK_FOR_STACK_OVERFLOW

此宏用于使能栈溢出检测， 当宏 configCHECK_FOR_STACK_OVERFLOW 设置为 1 时，使用栈溢出检测方法一； 当宏 configCHECK_FOR_STACK_OVERFLOW 设置为 2 时，栈溢出检测方法二； 当宏 configCHECK_FOR_STACK_OVERFLOW 设置为 0 时，不使能栈溢出检测。

4. configUSE_MALLOC_FAILED_HOOK

此宏用于使能使用动态内存分配失败钩子函数，当宏configUSE_MALLOC_FAILED_HOOK 设置为 1 时，使能使用动态内存分配失败钩子函数，用户需自定义相关钩子函数； 当宏 configUSE_MALLOC_FAILED_HOOK 设置为 0 时，则不使能使用动态内存分配失败钩子函数。

5. configUSE_DAEMON_TASK_STARTUP_HOOK

此宏用于使能使用定时器服务任务首次执行前的钩子函数，当 宏configUSE_DEAMON_TASK_STARTUP_HOOK 设置为 1 时，使能使用定时器服务任务首次执行前的钩子函数， 此时用户需定义定时器服务任务首次执行的相关钩子函数； 当宏configUSE_DEAMON_TASK_STARTUP_HOOK 设置为 0 时，则不使能使用定时器服务任务首次执行前的钩子函数。

### 2.4 运行时间和任务状态统计相关定义

1. configGENERATE_RUN_TIME_STATS

此宏用于使能任务运行时间统计功能， 当宏 configGENERATE_RUN_TIME_STATS 设置为1 时，使能任务运行时间统计功能，此时用户需要提供两个函数，一个是用于配置任务运行时间统计功能的函数 portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()，一般是完成定时器的初始化，另一个函数是 portGET_RUN_TIME_COUNTER_VALUE()，该函数用于获取定时器的计时值； 当宏 configGENERATE_RUN_TIME_STATS 设置为 0 时，则不使能任务运行时间统计功能。

2. configUSE_TRACE_FACILITY

此宏用于使能可视化跟踪调试， 当宏 configUSE_TRACE_FACILITY 设置为 1 时，使能可视化跟踪调试； 当宏 configUSE_TRACE_FACILITY 设置为 0 时，则不使能可视化跟踪调试。

3. configUSE_STATS_FORMATTING_FUNCTIONS

当此宏与 configUSE_TRACE_FACILITY 同时设置为 1 时，将编译函数 vTaskList()和函数vTaskGetRunTimeStats()，否则将忽略编译函数 vTaskList()和函数 vTaskGetRunTimeStats()。

### 2.5 协程相关定义

1. configUSE_CO_ROUTINES

此宏用于启用协程， 当宏 configUSE_CO_ROUTINES 设置为 1 时，启用协程； 当宏configUSE_CO_ROUTINES 设置为 0 时，则不启用协程。

2. configMAX_CO_ROUTINE_PRIORITIES

此宏用于设置协程的最大任务优先级数量，协程的最大任务优先级数值为configMAX_CO_ROUTINE_PRIORITIES-1

### 2.6 软件定时器相关定义

1. configUSE_TIMERS

此宏用于启用软件定时器功能， 当宏 configUSE_TIMERS 设置为 1 时，启用软件定时器功能； 当宏 configUSE_TIMERS 设置为 0 时，则不启用软件定时器功能。

2. configTIMER_TASK_PRIORITY

此宏用于设置软件定时器处理任务的优先级，当启用软件定时器功能时，系统会创建一个用于处理软件定时器的软件定时器处理任务。

3. configTIMER_QUEUE_LENGTH

此宏用于定义软件定时器队列的长度， 软件定时器的开启、停止与销毁等操作都是通过队列实现的。

4. configTIMER_TASK_STACK_DEPTH

此宏用于设置软件定时器处理任务的栈空间大小，当启用软件定时器功能时，系统会创建一个用于处理软件定时器的软件定时器处理任务。

### 2.7 中断嵌套行为配置

1. configPRIO_BITS

此宏应定义为 MCU 的 8 位优先级配置寄存器实际使用的位数。

2. configLIBRARY_LOWEST_INTERRUPT_PRIORITY

此宏应定义为 MCU 的最低中断优先等级，对于 STM32，在使用 FreeRTOS 时，建议将中断优先级分组设置为组 4，此时中断的最低优先级为 15。 此宏定义用于辅助配置宏configKERNEL_INTERRUPT_PRIORITY。

3. configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

此宏定义用于设置 FreeRTOS 可管理中断的最高优先级，当中断的优先级数值小于configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 时，此中断不受 FreeRTOS 管理。此宏定义用于辅助配置宏 configMAX_SYSCALL_INTERRUPT_PRIORITY。

4. configKERNEL_INTERRUPT_PRIORITY

此宏应定义为 MCU 的最低中断优先等级在中断优先级配置寄存器中的值，对于 STM32，即宏 configLIBRARY_LOWEST_INTERRUPT_PRIORITY 偏移 4bit 的值。

5. configMAX_SYSCALL_INTERRUPT_PRIORITY

此宏应定义为 FreeRTOS 可管理中断的最高优先等级在中断优先级配置寄存器中的值，对于 STM32，即宏 configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 偏移 4bit 的值。

6. configMAX_API_CALL_INTERRUPT_PRIORITY

此宏为宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 的新名称，只被用在 FreeRTOS官方一些新的移植当中，此宏与宏 configMAX_SYSCALL_INTERRUPT_PRIORITY 是等价的。

### 2.8 断言

1. vAssertCalled(char, int)

此宏用于辅助配置宏 configASSERT( x )以通过串口打印相关信息。

2. configASSERT( x )

此宏为 FreeRTOS 操作系统中的断言，断言会对表达式 x 进行判断，当 x 为假时，断言失败，表明程序出错，于是使用宏 vAssertCalled(char, int)通过串口打印相关的错误信息。断言常用于检测程序中的错误，使用断言将增加程序的代码大小和执行时间，因此建议在程序调试通过后将宏 configASSERT( x )进行注释，以较少额外的开销。

## 3. “INCLUDE”配置项

FreeRTOS 使用“INCLUDE”配置项对部分 API 函数进行条件编译，当“INCLUDE”配置项被定义为 1 时，其对应的 API 函数则会加入编译。对于用不到的 API 函数，用户则可以将其对应的“INCLUDE”配置项设置为 0，那么这个 API 函数就不会加入编译，以减少不必要的系统开销。“INCLUDE”配置项与其对应 API 函数的功能描述如下表所示：

![屏幕截图 2025-08-10 151751.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/10-15-18-06-屏幕截图%202025-08-10%20151751.png)

## 4. 其他配置项

1. ureconfigMAX_SECURE_CONTEXTS

此宏为 ARMv8-M 安全侧端口的相关配置项，本文暂不涉及 ARMv8-M 安全侧端口的相关内容，感兴趣的读者可自行查阅相关资料。

2. endSVHandler 和 vPortSVCHandler

这两个宏为 PendSV 和 SVC 的中断服务函数，主要用于 FreeRTOS 操作系统的任务切换，有关 FreeRTOS 操作系统中任务切换的相关内容

---


