# 第七章 FreeRTOS队列

## 1. 队列简介

队列是一种任务到任务、任务到中断、中断到任务数据交流的一种机制。在队列中可以存储数量有限、大小固定的多个数据，队列中的每一个数据叫做队列项目，队列能够存储队列项目的最大数量称为队列的长度，在创建队列的时候，就需要指定所创建队列的长度及队列项目的大小。因为队列是用来在任务与任务或任务于中断之间传递消息的一种机制，因此队列也叫做消息队列。

基于队列， FreeRTOS 实现了多种功能，其中包括队列集、互斥信号量、计数型信号量、二值信号量、 递归互斥信号量，因此很有必要深入了解 FreeRTOS 的队列。

1. 数据存储

队列通常采用 FIFO（先进先出）的存储缓冲机制，当有新的数据被写入队列中时，永远都是写入到队列的尾部，而从队列中读取数据时，永远都是读取队列的头部数据。但同时 FreeRTOS的队列也支持将数据写入到队列的头部， 并且还可以指定是否覆盖先前已经在队列头部的数据。

2. 多任务访问

队列不属于某个特定的任务，可以在任何的任务或中断中往队列中写入消息，或者从队列中读取消息。

3. 队列读取阻塞

在任务从队列读取消息时，可以指定一个阻塞超时时间。如果任务在读取队列时，队列为空，这时任务将被根据指定的阻塞超时时间添加到阻塞态任务列表中进行阻塞，以等待队列中有可用的消息。当有其他任务或中断将消息写入队列中， 因等待队列而阻塞任务将会被添加到就绪态任务列表中，并读取队列中可用的消息。如果任务因等待队列而阻塞的时间超过指定的阻塞超时时间，那么任务也将自动被转移到就绪态任务列表中，但不再读取队列中的数据。

因为同一个队列可以被多个任务读取，因此可能会有多个任务因等待同一个队列，而被阻塞，在这种情况下，如果队列中有可用的消息，那么也只有一个任务会被解除阻塞并读取到消息，并且会按照阻塞的先后和任务的优先级，决定应该解除哪一个队列读取阻塞任务。

4. 队列写入阻塞

与队列读取一样，在任务往队列写入消息时，也可以指定一个阻塞超时时间。如果任务在写入队列时，队列已经满了，这时任务将被根据指定的阻塞超时时间添加到阻塞态任务列表中进行阻塞，以等待队列有空闲的位置可以写入消息。指定的阻塞超时时间为任务阻塞的最大时间，如果在阻塞超时时间到达之前，队列有空闲的位置，那么队列写入阻塞任务将会解除阻塞，并往队列中写入消息，如果达到指定的阻塞超时时间，队列依旧没有空闲的位置写入消息，那么队列写入阻塞任务将会自动转移到就绪态任务列表中，但不会往队列中写入消息。

因为同一个队列可以被多个任务写入， 因此可有会有多个任务因等待统一个任务，而被阻塞，在这种情况下，如果队列中有空闲的位置，那么也之后一个任务会被解除阻塞并往队列中写入消息，并且会按照阻塞的先后和任务的优先级，决定应该解除哪一个队列写入阻塞任务。

5. 队列操作

下面简单介绍一下队列操作的过程，包括创建队列、往队列中写入消息、从队列中读取消息等操作。

### 1.1 创建队列

![屏幕截图 2025-08-16 135840.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-13-58-47-屏幕截图%202025-08-16%20135840.png)

创建了一个用于任务 A 与任务 B 之间“沟通交流”的队列，这个队列最大可容纳 5 个队列项目，即队列的长度为 5。刚创建的队列是不包含内容的，因此这个队列为空。

### 1.2 往队列写入第一个消息

![屏幕截图 2025-08-16 135926.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-13-59-30-屏幕截图%202025-08-16%20135926.png)

任务 A 将一个私有变量写入队列的尾部。由于在写入队列之前，队列是空的，因此新写入的消息，既是是队列的头部，也是队列的尾部。

### 1.3 往队列中写入第二个消息

![屏幕截图 2025-08-16 135947.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-13-59-52-屏幕截图%202025-08-16%20135947.png)

任务 A 改变了私有变量的值，并将新值写入队列。现在队列中包含了队列 A写入的两个值，其中第一个写入的值在队列的头部，而新写入的值在队列的尾部。 这时队列还有 3 个空闲的位置。

### 1.4 从队列读取第一个消息

![屏幕截图 2025-08-16 140017.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/08/16-14-00-21-屏幕截图%202025-08-16%20140017.png)

任务 B 从队列中读取消息，任务 B 读取的消息是处于队列头部的消息，这是任务 A 第一次往队列中写入的消息。在任务 B 从队列中读取消息后，队列中任务 A 第二次写入的消息，变成了队列的头部，因此下次任务 B 再次读取消息时，将读取到这个消息。此时队列中剩余 4 个空闲的位置。

## 2. FreeRTOS队列相关API函数

### 2.1 创建队列

| 函数                     | 描述       |
| ---------------------- | -------- |
| `xQueueCreate()`       | 动态方式创建队列 |
| `xQueueCreateStatic()` | 静态方式创建队列 |

#### 2.1.1 动态方式创建队列

`xQueueCreate()` 函数用于动态地创建 FreeRTOS 队列。这意味着队列所需的内存（包括队列结构体和存储数据的缓冲区）将从 FreeRTOS 堆中分配。

**函数原型：**

```c
QueueHandle_t xQueueCreate( UBaseType_t uxQueueLength, UBaseType_t uxItemSize );
```

**参数说明：**

- `uxQueueLength`: 队列可以存储的最大项目数量。
- `uxItemSize`: 队列中每个项目的大小，以字节为单位。如果队列用于存储指针，则将其设置为 `sizeof( Type_t * )`，其中 `Type_t` 是指针指向的数据类型。

**返回值：**

- `QueueHandle_t`: 如果队列成功创建，则返回一个队列句柄（`QueueHandle_t` 类型），该句柄用于后续的队列操作。
- `NULL`: 如果由于内存不足而无法创建队列，则返回 `NULL`。

**注意事项：**

- 要使此函数可用，`configSUPPORT_DYNAMIC_ALLOCATION` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 使用 `xQueueCreate()` 创建的队列在不再使用时应该通过 `vQueueDelete()` 函数进行删除，以释放占用的内存，防止内存泄漏。
- 队列可以存储固定大小的项目（通过值传递），也可以存储指针（通过引用传递）。当存储指针时，用户需要自己管理指针指向的数据的生命周期。

#### 2.1.2 静态方式创建队列

`xQueueCreateStatic()` 函数用于静态地创建 FreeRTOS 队列。这意味着队列结构体和数据存储缓冲区所需的内存由用户在编译时或运行时提供，而不是从 FreeRTOS 堆中动态分配。这对于对内存管理有严格要求、需要确定性行为或禁止使用动态内存分配的系统非常有用。

**函数原型：**

```c
QueueHandle_t xQueueCreateStatic( UBaseType_t uxQueueLength,
                                  UBaseType_t uxItemSize,
                                  uint8_t *pucQueueStorageBuffer,
                                  StaticQueue_t *pxQueueBuffer );
```

**参数说明：**

- `uxQueueLength`: 队列可以存储的最大项目数量。
- `uxItemSize`: 队列中每个项目的大小，以字节为单位。
- `pucQueueStorageBuffer`: 指向用户提供的 `uint8_t` 数组的指针，该数组将用作队列的数据存储缓冲区。数组的大小应至少为 `( uxQueueLength * uxItemSize )` 字节。
- `pxQueueBuffer`: 指向用户提供的 `StaticQueue_t` 结构体变量的指针，该结构体将用作队列的控制块。

**返回值：**

- `QueueHandle_t`: 如果队列成功创建，则返回一个队列句柄。
- `NULL`: 如果 `pucQueueStorageBuffer` 或 `pxQueueBuffer` 为 `NULL`，则返回 `NULL`。

**注意事项：**

- 要使此函数可用，`configSUPPORT_STATIC_ALLOCATION` 必须在 `FreeRTOSConfig.h` 中定义为 1。
- 用户需要自行管理 `pucQueueStorageBuffer` 和 `pxQueueBuffer` 所指向的内存生命周期。这些内存通常定义为全局变量或静态变量，以确保它们在队列的整个生命周期内都有效。
- 静态创建的队列不需要调用 `vQueueDelete()` 来释放内存，因为内存是由应用程序静态分配的。

### 2.2 队列写入消息

| 函数                           | 描述                            |
| ---------------------------- | ----------------------------- |
| `xQueueSend()`               | 往队列的尾部写入消息                    |
| `xQueueSendToBack()`         | 同 `xQueueSend()`              |
| `xQueueSendToFront()`        | 往队列的头部写入消息                    |
| `xQueueOverwrite()`          | 覆盖写入队列消息 (只用于队列长度为 1 的情况)     |
| `xQueueSendFromISR()`        | 在中断中往队列的尾部写入消息                |
| `xQueueSendToBackFromISR()`  | 同 `xQueueSendFromISR()`       |
| `xQueueSendToFrontFromISR()` | 在中断中往队列的头部写入消息                |
| `xQueueOverwriteFromISR()`   | 在中断中覆盖写入队列消息 (只用于队列长度为 1 的情况) |

#### 2.2.1 任务上下文中的队列写入函数

这些函数可以在任务中调用，并且可以在队列满时选择阻塞任务等待。

- **`xQueueSend()`**
  
  - **描述：** 往队列的尾部（末端）写入一个消息。
  - **函数原型：** `BaseType_t xQueueSend( QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvItemToQueue`: 指向要发送到队列的数据的指针。该数据将被复制到队列中。
    - `xTicksToWait`: 如果队列已满，任务愿意阻塞等待的节拍数。设置为 `portMAX_DELAY` 表示无限期等待，设置为 `0` 表示不等待。
  - **返回值：**
    - `pdPASS`: 消息成功发送到队列。
    - `errQUEUE_FULL`: 队列已满，且等待时间已到或为 `0` 未等待。
  - **注意事项：** 这是最常用的发送函数。

- **`xQueueSendToBack()`**
  
  - **描述：** 功能与 `xQueueSend()` 完全相同，也是往队列的尾部写入消息。这是一个为了提高代码可读性而提供的别名。
  - **函数原型：** `BaseType_t xQueueSendToBack( QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait );`
  - **参数、返回值及注意事项** 与 `xQueueSend()` 相同。

- **`xQueueSendToFront()`**
  
  - **描述：** 往队列的头部（前端）写入一个消息。这使得新发送的消息成为下一个被读取的消息。
  - **函数原型：** `BaseType_t xQueueSendToFront( QueueHandle_t xQueue, const void *pvItemToQueue, TickType_t xTicksToWait );`
  - **参数、返回值及注意事项** 与 `xQueueSend()` 类似，只是消息被放置在队列的头部。

- **`xQueueOverwrite()`**
  
  - **描述：** 仅用于长度为 1 的队列（即邮箱）。如果队列已满，新消息将覆盖旧消息。如果队列为空，则直接写入。此函数不会导致任务阻塞。
  - **函数原型：** `BaseType_t xQueueOverwrite( QueueHandle_t xQueue, const void *pvItemToQueue );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvItemToQueue`: 指向要发送到队列的数据的指针。
  - **返回值：**
    - `pdPASS`: 消息成功发送或覆盖。
  - **注意事项：** 通常用于实现“最新值”机制，例如传感器数据的最新读取值。

#### 2.2.2 中断服务例程 (ISR) 中的队列写入函数

这些函数是任务上下文函数的“FromISR”版本，它们可以在 ISR 中安全地调用。它们不会阻塞，而是通过 `pxHigherPriorityTaskWoken` 参数指示是否需要进行上下文切换。

- **`xQueueSendFromISR()`**
  
  - **描述：** 在中断中往队列的尾部写入一个消息。
  - **函数原型：** `BaseType_t xQueueSendFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvItemToQueue`: 指向要发送到队列的数据的指针。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。此变量应初始化为 `pdFALSE`。如果调用此函数导致更高优先级的任务解除阻塞（即需要进行上下文切换），则该变量将被设置为 `pdTRUE`。
  - **返回值：**
    - `pdPASS`: 消息成功发送到队列。
    - `errQUEUE_FULL`: 队列已满。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`（或类似的宏）来触发上下文切换。

- **`xQueueSendToBackFromISR()`**
  
  - **描述：** 功能与 `xQueueSendFromISR()` 完全相同，也是在中断中往队列的尾部写入消息。
  - **函数原型：** `BaseType_t xQueueSendToBackFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 与 `xQueueSendFromISR()` 相同。

- **`xQueueSendToFrontFromISR()`**
  
  - **描述：** 在中断中往队列的头部写入一个消息。
  - **函数原型：** `BaseType_t xQueueSendToFrontFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 与 `xQueueSendFromISR()` 类似，只是消息被放置在队列的头部。

- **`xQueueOverwriteFromISR()`**
  
  - **描述：** 仅用于长度为 1 的队列（邮箱），在中断中覆盖写入消息。
  - **函数原型：** `BaseType_t xQueueOverwriteFromISR( QueueHandle_t xQueue, const void *pvItemToQueue, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数、返回值及注意事项** 与 `xQueueSendFromISR()` 类似，但用于覆盖写入。

### 2.3 队列读取消息

| 函数                       | 描述                  |
| ------------------------ | ------------------- |
| `xQueueReceive()`        | 从队列头部读取消息，并删除消息     |
| `xQueuePeek()`           | 从队列头部读取消息           |
| `xQueueReceiveFromISR()` | 在中断中从队列头部读取消息，并删除消息 |
| `xQueuePeekFromISR()`    | 在中断中从队列头部读取消息       |

#### 2.3.1 任务上下文中的队列读取函数

这些函数可以在任务中调用，并且可以在队列为空时选择阻塞任务等待。

- **`xQueueReceive()`**
  
  - **描述：** 从队列的头部读取一个消息，并将该消息从队列中删除。如果队列为空，调用任务可以选择阻塞，直到有消息可用或超时。
  - **函数原型：** `BaseType_t xQueueReceive( QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvBuffer`: 指向一个缓冲区的指针，用于存储从队列中接收到的消息。该缓冲区的大小必须至少与队列创建时指定 `uxItemSize` 相同。
    - `xTicksToWait`: 如果队列为空，任务愿意阻塞等待的节拍数。设置为 `portMAX_DELAY` 表示无限期等待，设置为 `0` 表示不等待。
  - **返回值：**
    - `pdPASS`: 消息成功从队列接收。
    - `errQUEUE_EMPTY`: 队列为空，且等待时间已到或为 `0` 未等待。
  - **注意事项：** 这是最常用的队列读取函数。

- **`xQueuePeek()`**
  
  - **描述：** 从队列的头部读取一个消息，但 **不** 将其从队列中删除。这意味着消息仍然留在队列中，可以被其他任务或后续调用再次读取。如果队列为空，调用任务可以选择阻塞，直到有消息可用或超时。
  - **函数原型：** `BaseType_t xQueuePeek( QueueHandle_t xQueue, void *pvBuffer, TickType_t xTicksToWait );`
  - **参数、返回值及注意事项** 与 `xQueueReceive()` 类似，但主要区别在于消息不会被删除。

#### 2.3.2 中断服务例程 (ISR) 中的队列读取函数

这些函数是任务上下文函数的“FromISR”版本，它们可以在 ISR 中安全地调用。它们不会阻塞，而是通过 `pxHigherPriorityTaskWoken` 参数指示是否需要进行上下文切换。

- **`xQueueReceiveFromISR()`**
  
  - **描述：** 在中断中从队列的头部读取一个消息，并将该消息从队列中删除。
  - **函数原型：** `BaseType_t xQueueReceiveFromISR( QueueHandle_t xQueue, void *pvBuffer, BaseType_t *pxHigherPriorityTaskWoken );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvBuffer`: 指向一个缓冲区的指针，用于存储从队列中接收到的消息。
    - `pxHigherPriorityTaskWoken`: 指向 `BaseType_t` 变量的指针。此变量应初始化为 `pdFALSE`。如果调用此函数导致更高优先级的任务解除阻塞（即需要进行上下文切换），则该变量将被设置为 `pdTRUE`。
  - **返回值：**
    - `pdPASS`: 消息成功从队列接收。
    - `pdFALSE`: 队列为空。
  - **注意事项：** 在 ISR 结束时，应检查 `*pxHigherPriorityTaskWoken` 的值。如果为 `pdTRUE`，则调用 `portYIELD_FROM_ISR()`（或类似的宏）来触发上下文切换。

- **`xQueuePeekFromISR()`**
  
  - **描述：** 在中断中从队列的头部读取一个消息，但 **不** 将其从队列中删除。
  - **函数原型：** `BaseType_t xQueuePeekFromISR( QueueHandle_t xQueue, void *pvBuffer );`
  - **参数：**
    - `xQueue`: 目标队列的句柄。
    - `pvBuffer`: 指向一个缓冲区的指针，用于存储从队列中接收到的消息。
  - **返回值：**
    - `pdPASS`: 消息成功从队列读取。
    - `pdFALSE`: 队列为空。
  - **注意事项：** 此函数不会影响任务的阻塞状态，因此不需要 `pxHigherPriorityTaskWoken` 参数。

## 3. FreeRTOS队列操作测试

### 3.1 队列配置

```c
/*----------------队列配置区-----------------*/
QueueHandle_t xQueue; // 队列句柄
#define QUEUE_LENGTH 1 // 队列长度
#define QUEUE_ITEM_SIZE sizeof(uint8_t) // 队列中每条信息的大小
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
    lcd_show_string(10, 47, 220, 24, 24, "Message Queue", RED);
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
    // 创建队列
    xQueue = xQueueCreate(QUEUE_LENGTH, QUEUE_ITEM_SIZE);
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
    uint8_t key_value =0;
    while(1)
    {
        key_value = key_scan(0);
        if(key_value != 0)
        {
            xQueueSend(xQueue, &key_value, portMAX_DELAY); // 发送数据到队列
        }
        vTaskDelay(10);
    }
}

// task2函数实现
void task2(void *pvParameters)
{
    uint8_t queue_recv = 0; // 接收队列数据
    uint32_t task2_num = 0; // 任务2运行次数
    while(1)
    {
        xQueueReceive(xQueue, &queue_recv, portMAX_DELAY); // 接收队列数据
        switch(queue_recv)
        {
            case KEY0_PRES:
            {
                lcd_fill(6,131,233,313,lcd_discolor[++task2_num%11]);
                break;
            }
            case KEY1_PRES:
            {
                LED0_TOGGLE();
                break;
            }
            default:break;
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

---
