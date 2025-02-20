# 第五章 FreeRTOS中链表的实现

reeRTOS 中与链表相关的操作均在 list.h 和 list.c 这两个文件中实现， list.h 第一次使用需要在 include 文件夹下面新建然后添加到工程 freertos/source 这个组文件， list.c 第一次使用需要在 freertos 文件夹下面新建然后添加到工程 freertos/source 这个组文件。

## 1. 实现链表节点

### 1.1 定义链表节点数据结构

```c
// FreeRTOS中链表节点数据结构定义
struct xLIST_ITEM
{
    TickType_t xItemValue; // 节点值, 用于排序
    struct xLIST_ITEM *pxNext; // 指向下一个节点的指针
    struct xLIST_ITEM *pxPrevious; // 指向前一个节点的指针
    void *pvOwner; // 指向拥有该节点的内核对象
    void *pvContainer; // 指向该节点所在的链表
};
typedef struct xLIST_ITEM ListItem_t; // 链表节点类型定义
```

![屏幕截图 2025-02-20 205050.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/20-20-50-58-屏幕截图%202025-02-20%20205050.png)

### 1.2 链表节点初始化

链表节点初始化函数在 list.c 中实现

```c
// 链表节点初始化
void vListInitialiseItem(ListItem * const pxItem)
{
    pxItem->pvContainer = NULL; // 初始化该节点所在的链表为空，代表改节点没有插入任何链表
}
```

链表节点 ListItem_t 总共有 5 个成员，但是初始化的时候只需将pvContainer 初始化为空即可， 表示该节点还没有插入到任何链表。一个初始化好的节点示意图具体见图

![屏幕截图 2025-02-20 205926.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/20-20-59-29-屏幕截图%202025-02-20%20205926.png)

## 2. 实现链表根节点

### 2.1 根节点数据结构定义

链表根节点的数据结构在 list.h 中定义

```c
// FreeRTOS链表根节点定义
typedef struct xLIST
{
    UBaseType_t uxNumberOfItems; // 链表节点计数器
    ListItem_t pxIndex; // 链表节点索引
    MiniListItem_t xListEnd; // 链表尾节点  
};List_t;
```

![屏幕截图 2025-02-20 210224.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/20-21-02-27-屏幕截图%202025-02-20%20210224.png)

我们知道，链表是首尾相连的，是一个圈，首就是尾，尾就是首，这里从字面上理解就是链表的最后一个节点，实际也就是链表的第一个节点，我们称之为生产者。该生产者的数据类型是一个精简的节点， 也在 list.h 中定义

```c
// 生产者节点结构体
struct xMINT_LIST_ITEM
{
    TickType_t xItemValue; // 辅助节点排序值
    struct xMINT_LIST_ITEM *pxNext; // 指向下一个节点的指针
    struct xMINT_LIST_ITEM *pxPrevious; // 指向前一个节点的指针
};
typedef struct xMINT_LIST_ITEM ListItem_t; // 生产者节点类型定义
```

### 2.2 链表根节点初始化

链表节点初始化函数在 list.c 中实现

```c
// 链表根节点初始化
void vListInitialise(List_t * const pxList)
{
    pxList->pxIndex.pxNext = (ListItem_t *)&pxList->xListEnd; // 指向链表尾节点
    pxList->xLisEnd.xItemValue = portMAX_DELAY; // 链表尾节点排序值初始化为最大值, 确保该节点就算链表的最后节点
    pxList->xListEnd.pxNext = (ListItem_t *)&(pxList->xListEnd); // 指向自己, 确保该节点就算链表的最后节点
    pxList->xListEnd.pxPrevious = (ListItem_t *)&(pxList->xListEnd); // 指向自己, 确保该节点就算链表的最后节点
    pxList->uxNumberOfItems = (UBaseType_t)0U; // 链表节点计数器初始化为0
}
```

![屏幕截图 2025-02-20 211051.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/20-21-10-54-屏幕截图%202025-02-20%20211051.png)

### 2.3 将节点插入到链表的尾部

```c

```
