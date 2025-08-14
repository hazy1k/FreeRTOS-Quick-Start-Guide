# 第四章 FreeRTOS列表与列表项

## 1. 列表与列表项

| 函数                      | 描述        |
| ----------------------- | --------- |
| `vListInitialise()`     | 初始化列表     |
| `vListInitialiseItem()` | 初始化列表项    |
| `vListInsertEnd()`      | 列表末尾插入列表项 |
| `vListInsert()`         | 列表插入列表项   |
| `uxListRemove()`        | 列表移除列表项   |

### 1.1 初始化列表

`vListInitialise()` 函数用于初始化一个 FreeRTOS 列表。在使用任何列表操作之前，必须先调用此函数对列表进行初始化。

**函数原型：**

```c
void vListInitialise( List_t * const pxList );
```

**参数说明：**

- `pxList`: 指向要初始化的 `List_t` 结构体变量的指针。

**注意事项：**

- `List_t` 结构体通常在外部定义，例如在任务控制块（TCB）或事件组中。
- 此函数将列表的成员（如链表头指针、列表项计数器等）设置为其初始状态，使其成为一个空列表。

### 1.2 初始化列表项

`vListInitialiseItem()` 函数用于初始化一个 FreeRTOS 列表项。每个要插入到 FreeRTOS 列表中的数据结构都必须包含一个 `ListItem_t` 或 `MiniListItem_t` 类型的列表项成员，并且在使用前需要通过此函数进行初始化。

**函数原型：**

```c
void vListInitialiseItem( ListItem_t * const pxListItem );
```

**参数说明：**

- `pxListItem`: 指向要初始化的 `ListItem_t` 结构体变量的指针。

**注意事项：**

- `ListItem_t` 或 `MiniListItem_t` 通常是用户定义结构体的一部分，用于将该结构体链接到 FreeRTOS 列表中。
- 此函数将列表项的成员（如指向其所属列表的指针、节点值等）设置为其初始状态。

### 1.3 列表末尾插入列表项

`vListInsertEnd()` 函数用于将一个已初始化的列表项插入到指定列表的末尾。

**函数原型：**

```c
void vListInsertEnd( List_t * const pxList, ListItem_t * const pxNewListItem );
```

**参数说明：**

- `pxList`: 指向目标列表的 `List_t` 结构体变量的指针。
- `pxNewListItem`: 指向要插入的列表项的 `ListItem_t` 结构体变量的指针。

**注意事项：**

- 此函数不会考虑列表项的值，而是简单地将其添加到列表的末尾。

### 1.4 列表插入列表项（按排序顺序）

`vListInsert()` 函数用于将一个已初始化的列表项插入到指定列表的正确位置，以保持列表的排序顺序。列表项的排序基于其“值”（`xItemValue` 成员）。

**函数原型：**

```c
void vListInsert( List_t * const pxList, ListItem_t * const pxNewListItem );
```

**参数说明：**

- `pxList`: 指向要插入列表项的 `List_t` 结构体变量的指针。
- `pxNewListItem`: 指向要插入的 `ListItem_t` 结构体变量的指针。列表项的 `xItemValue` 成员将用于排序。

**注意事项：**

- 此函数通常在 FreeRTOS 内部使用，例如将一个任务添加到就绪列表或阻塞列表中，并根据任务优先级进行排序。
- 应用程序通常不需要直接调用此函数。

### 1.5 列表移除列表项

`uxListRemove()` 函数用于将一个列表项从其当前所在的列表中移除。

**函数原型：**

```c
UBaseType_t uxListRemove( ListItem_t * const pxItemToRemove );
```

**参数说明：**

- `pxItemToRemove`: 指向要移除的 `ListItem_t` 结构体变量的指针。

**返回值：**

- `UBaseType_t`: 返回移除列表项后，列表中剩余的列表项数量。

**注意事项：**

- 此函数通常在 FreeRTOS 内部使用，例如当任务从阻塞状态变为就绪状态时，将其从阻塞列表中移除。
- 应用程序通常不需要直接调用此函数

## 2. FreeRTOS列表项的插入与删除测试

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

### 2.2 列表配置

```c
/*----------------列表配置区------------------*/
List_t TestList;      // 测试列表
ListItem_t ListItem1; // 测试列表项1
ListItem_t ListItem2; // 测试列表项2
ListItem_t ListItem3; // 测试列表项3
/*-------------------------------------------*/
```

### 2.3 任务实现

```c
/*------------------任务实现区----------------*/
void freertos_demo(void)
{
    lcd_show_string(10,47,220,24,24,"List Insert Demo",RED);
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
    // 初始化列表和列表项
    vListInitialise(&TestList);
    vListInitialiseItem(&ListItem1);
    vListInitialiseItem(&ListItem2);
    vListInitialiseItem(&ListItem3);

    // 打印列表和列表项的地址
    printf("-----------------打印列表和列表项的地址-------------------\r\n");
    printf("项目\t\t\t地址\r\n");
    printf("TestList\t\t0x%p\t\r\n", &TestList);
    printf("TestList->pxIndex\t0x%p\t\r\n", TestList.pxIndex); // 列表头
    printf("TestList->xListEnd\t0x%p\t\r\n", (&TestList.xListEnd)); // 列表尾
    printf("ListItem1\t\t0x%p\t\r\n", &ListItem1);
    printf("ListItem2\t\t0x%p\t\r\n", &ListItem2);
    printf("ListItem3\t\t0x%p\t\r\n", &ListItem3);
    printf("-----------------------STEP1 END------------------------\r\n");
    printf("按下KEY0以继续\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // 列表项1插入列表中
    printf("--------------------列表项1插入列表中---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem1);
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("-----------------------STEP2 END------------------------\r\n");
    printf("按下KEY0以继续\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // 列表项2插入列表中
    printf("--------------------列表项2插入列表中---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem2);
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("-----------------------STEP3 END------------------------\r\n");
    printf("按下KEY0以继续\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // 列表项3插入列表中
    printf("--------------------列表项3插入列表中---------------------\r\n");
    vListInsert((List_t*)&TestList, (ListItem_t*)&ListItem3);
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("-----------------------STEP4 END------------------------\r\n");
    printf("按下KEY0以继续\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

    // 移除列表项2
    printf("------------------------移除列表项2-----------------------\r\n");
    uxListRemove((ListItem_t*)&ListItem2);
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("按下KEY0以继续\r\n");
    printf("-----------------------STEP5 END------------------------\r\n");
    while(key_scan(0) != KEY0_PRES)
    {
        vTaskDelay(10);
    }

   // 在列表末尾添加列表项2
    printf("-------------------在列表末尾添加列表项2-------------------\r\n");
    vListInsertEnd((List_t*)&TestList, (ListItem_t*)&ListItem2);
    printf("项目\t\t\t\t地址\r\n");
    printf("TestList->pxIndex\t\t0x%p\r\n", TestList.pxIndex);
    printf("TestList->xListEnd->pxNext\t0x%p\r\n", (TestList.xListEnd.pxNext));
    printf("ListItem1->pxNext\t\t0x%p\r\n", (ListItem1.pxNext));
    printf("ListItem2->pxNext\t\t0x%p\r\n", (ListItem2.pxNext));
    printf("ListItem3->pxNext\t\t0x%p\r\n", (ListItem3.pxNext));
    printf("TestList->xListEnd->pxPrevious\t0x%p\r\n", (TestList.xListEnd.pxPrevious));
    printf("ListItem1->pxPrevious\t\t0x%p\r\n", (ListItem1.pxPrevious));
    printf("ListItem2->pxPrevious\t\t0x%p\r\n", (ListItem2.pxPrevious));
    printf("ListItem3->pxPrevious\t\t0x%p\r\n", (ListItem3.pxPrevious));
    printf("-----------------------------END-------------------------\r\n");
    while(1)
    {
        vTaskDelay(10);
    }
}
/*-------------------------------------------*/
```

### 2.4 主函数

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
