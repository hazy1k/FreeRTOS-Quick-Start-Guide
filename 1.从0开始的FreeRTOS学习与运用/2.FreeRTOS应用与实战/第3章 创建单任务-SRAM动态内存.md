# 第三章 创建单任务-SRAM动态内存

## 1. 动态内存空间的堆从哪里来

在创建单任务—SRAM 静态内存的例程中，任务控制块和任务栈的内存空间都是从内部的 SRAM 里面分配的，具体分配到哪个地址由编译器决定。现在我们开始使用动态内存，即堆，其实堆也是内存，也属于 SRAM。 FreeRTOS 做法是在 SRAM 里面定义一个大数组，也就是堆内存，供 FreeRTOS 的动态内存分配函数使用，在第一次使用的时候，系统会将定义的堆内存进行初始化，这些代码在 FreeRTOS 提供的内存管理方案中实现（heap_1.c、heap_2.c、 heap_4.c 等，具体的内存管理方案后面详细讲解） ，具体见代码：

```c
//系统所有总的堆大小
#define configTOTAL_HEAP_SIZE ((size_t)(36*1024)) 
static uint8_t ucHeap[configTOTAL_HEAP_SIZE]; 
/*如果这是第一次调用 malloc 那么堆将需要
 初始化， 以设置空闲块列表。 */
if(pxEnd == NULL)
{
    prvHeapInit();
}
else
{
    mtCOVERAGE_TEST_MARKER();
}
```

- 堆内存的大小为 configTOTAL_HEAP_SIZE，在FreeRTOSConfig.h 中由我们自己定义， configSUPPORT_DYNAMIC_ALLOCATION 这个宏定义在使用 FreeRTOS 操作系统的时候必须开启。

- 从内部 SRAMM 里面定义一个静态数组 ucHeap，大小由configTOTAL_HEAP_SIZE 这个宏决定， 目前定义为 36KB。定义的堆大小不能超过内部SRAM 的总大小。、

- 如果这是第一次调用 malloc 那么需要将堆进行初始化，以设置空闲块列表，方便以后分配内存，初始化完成之后会取得堆的结束地址，在 MemMang 中的5 个内存分配 heap_x.c 文件中实现。

## 2. 定义任务函数

使用动态内存的时候，任务的主体函数与使用静态内存时是一样的，具体见代码：

```c
static void LED_Task(void* parameter)
{
    while(1)
    {
        LED1_ON;
        vTaskDelay(500);
        LED1-OFF;
        vTaskDelay(500);
    }
}
```

- 任务必须是一个死循环，否则任务将通过 LR 返回，如果 LR 指向了非法的内存就会产生 HardFault_Handler，而 FreeRTOS 指向一个任务退出函数prvTaskExitError()，里面是一个死循环，那么任务返回之后就在死循环中执行，这样子的任务是不安全的，所以避免这种情况，任务一般都是死循环并且无返回值的。我们的AppTaskCreate 任务，执行一次之后就进行删除，则不影响系统运行，所以，只执行一次的任务在执行完毕要记得及时删除。

- 任务里面的延时函数必须使用 FreeRTOS 里面提供的延时函数，
  
  并不能使用我们裸机编程中的那种延时。这两种的延时的区别是 FreeRTOS 里面的延时是阻塞延时，即调用 vTaskDelay()函数的时候，当前任务会被挂起，调度器会切换到其它就绪的任务，从而实现多任务。如果还是使用裸机编程中的那种延时，那么整个任务就成为了一个死循环，如果恰好该任务的优先级是最高的，那么系统永远都是在这个任务中运行，比它优先级更低的任务无法运行，根本无法实现多任务。

## 3. 定义任务栈

使用动态内存的时候，任务栈在任务创建的时候创建，不用跟使用静态内存那样要预先定义好一个全局的静态的栈空间，动态内存就是按需分配内存，随用随取。

## 4. 定义任务控制块指针

使用动态内存时候，不用跟使用静态内存那样要预先定义好一个全局的静态的任务控制块空间。任务控制块是在任务创建的时候分配内存空间创建，任务创建函数会返回一个指针，用于指向任务控制块，所以要预先为任务栈定义一个任务控制块指针，也是我们常说的任务句柄

```c
/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
 /* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED任务句柄 */
static TaskHandle_t LED_Task_Handle = NULL;
```

## 5. 动态创建任务

使用静态内存时，使用 xTaskCreateStatic()来创建一个任务，而使用动态内存的时，则使用 xTaskCreate()函数来创建一个任务，两者的函数名不一样，具体的形参也有区别，具体见代码

```c
// 创建AppTaskCreate任务
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,// 任务入口函数 
                        (const char*)"AppTaskCreate", // 任务名字 
                        (uint16_t)512,                // 任务栈大小
                        (void*)NULL,                  // 任务入口函数参数
                        (UBaseType_t)1,               // 任务的优先级 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// 任务控制块指针 
```

- 任务入口函数，即任务函数的名称，需要我们自己定义并且实现

- 任务名字，字符串形式， 最大长度由 FreeRTOSConfig.h 中定义的configMAX_TASK_NAME_LEN 宏指定，多余部分会被自动截掉，这里任务名字最好要与任务函数入口名字一致，方便进行调试。

- 任务堆栈大小，单位为字，在 32 位的处理器下（STM32），一个字等于 4 个字节，那么任务大小就为 128 * 4 字节。

- 任务入口函数形参，不用的时候配置为 0 或者 NULL 即可。

- 任务的优先级。优先级范围根据 FreeRTOSConfig.h 中的宏configMAX_PRIORITIES 决定， 如果使能 configUSE_PORT_OPTIMISED_TASK_SELECTION，这个宏定义，则最多支持 32 个优先级；如果不用特殊方法查找下一个运行的任务，那么则不强制要求限制最大可用优先级数目。在 FreeRTOS中， 数值越大优先级越高， 0 代表最低优先级。

- 任务控制块指针，在使用内存的时候，需要给任务初始化函数xTaskCreateStatic()传递预先定义好的任务控制块的指针。在使用动态内存的时候，任务创建函数 xTaskCreate()会返回一个指针指向任务控制块，该任务控制块是 xTaskCreate()函数里面动态分配的一块内存。

## 6. 启动任务

当任务创建好后，是处于任务就绪（Ready） ，在就绪态的任务可以参与操作系统的调度。但是此时任务仅仅是创建了，还未开启任务调度器，也没创建空闲任务与定时器任务（如果使能了 configUSE_TIMERS 这个宏定义），那这两个任务就是在启动任务调度器中实现，每个操作系统，任务调度器只启动一次，之后就不会再次执行了， FreeRTOS 中启动任务调度器的函数是 vTaskStartScheduler()，并且启动任务调度器的时候就不会返回，从此任务管理都由FreeRTOS管理，此时才是真正进入实时操作系统中的第一步，具体见代码

```c
// 为了方便管理，所有的任务创建函数都放在这个函数里面
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  taskENTER_CRITICAL(); // 进入临界区
  // 创建LED_Task任务
  xReturn = xTaskCreate((TaskFunction_t)LED_Task, // 任务入口函数 
                        (const char*)"LED_Task",  // 任务名字 
                        (uint16_t)512,            // 任务栈大小 
                        (void*)NULL,	            // 任务入口函数参数 
                        (UBaseType_t)2,	          // 任务的优先级 
                        (TaskHandle_t*)&LED_Task_Handle);// 任务控制块指针
  if(pdPASS == xReturn)
    printf("创建LED_Task任务成功!\r\n");  
  vTaskDelete(AppTaskCreate_Handle);// 删除AppTaskCreate任务
  taskEXIT_CRITICAL();              // 退出临界区
}
```
