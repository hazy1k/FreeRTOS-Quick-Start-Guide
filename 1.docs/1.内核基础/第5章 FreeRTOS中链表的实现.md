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

将节点插入到链表的尾部（可以理解为头部） 就是将一个新的节点插入到一个空的链表

```c
// 将节点插入到链表的尾部
void vListInsertEnd(List_t * const pxList, ListItem_t * const pxNewListItem)
{
    ListItem_t * const pxIndex = pxList->pxIndex.pxNext; // 指向链表尾节点
    pxNewListItem->pxNext = pxIndex; // 新节点指向链表尾节点
    pxNewListItem->pxPrevious = pxIndex->pxPrevious; // 新节点指向前一个节点
    pxIndex->pxPrevious->pxNext = pxNewListItem; // 前一个节点指向新节点
    // 记住该节点所在的链表
    pxNewListItem->pvContainer = (void*)pxList;
    pxList->uxNumberOfItems++; // 链表节点计数器加1
}
```

![屏幕截图 2025-02-22 170311.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/22-17-03-19-屏幕截图%202025-02-22%20170311.png)

### 2.4 将节点按照升序排序插入到链表中

将节点按照升序排列插入到链表，如果有两个节点的值相同，则新节点在旧节点的后面插入

```c
// 将节点按照升序排列插入到链表中
void vListInsert(List_t * const pxList, ListItem_t * const pxNewListItem)
{
    ListItem_t *pxIterator; // 迭代器
    const TickType_t xValueOfNewListItem = pxNewListItem->xItemValue; // 获取节点的排序辅助值
    /* 寻找节点要插入的位置 */
    if(xValueOfInsertion == portMAX_DELAY) // 新节点排序值最大, 插入到链表尾部
    {
        pxIterato = pxList->xListEnd.pxPrevious; // 指向链表尾节点的前一个节点
    }
    else
    {
        // 遍历链表, 寻找插入位置
        for(pxIterator = (ListItem_t *)&(pxList->xListEnd); 
        pxIterator->pxNext->xItemValue < xValueOfNewListItem; 
        pxIterator = pxIterator->pxNext)
    }
    /* 根据升序排序，插入节点 */
    pxNewListItem->pxNext = pxIterator->pxNext; 
    pxNewListItem->pxNext->pxPrevious = pxNewListItem;
    pxNewListItem->pxPrevious = pxIterator;
    pxIterator->pxNext = pxNewListItem; 
    // 记住该节点所在的链表
    pxNewListItem->pvContainer = (void*)pxList;
    (pxList->uxNumberOfItems)++; // 链表节点计数器加1
}
```

![屏幕截图 2025-02-22 170940.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/22-17-09-43-屏幕截图%202025-02-22%20170940.png)

### 2.5 将节点从链表删除

```c
// 将节点从链表中删除
UBaseType_t uxListRemove(ListItem_t * const pxItemToRemove)
{
     List_t * const pxList = (List_t*)pxItemToRemove->pvContainer; // 获取节点所在的链表
     /* 将指定的节点从链表中删除 */
     pxItemToRemove->pxNext->pxPrevious = pxItemToRemove->pxPrevious;
     pxItemToRemove->pxPrevious->pxNext = pxItemToRemove->pxNext;
     if(pxList->pxIndex.pxNext == pxItemToRemove) // 调整链表的节点索引指针
     {
         pxList->pxIndex = pxItemToRemove->pxPrevious; // 指向前一个节点
     }
     pxItemToRemove->pvContainer = NULL; // 节点不再属于任何链表
     pxList->uxNumberOfItems--; // 链表节点计数器减1
     return pxList->uxNumberOfItems; // 返回链表节点计数器
} 
```

![屏幕截图 2025-02-22 171407.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/22-17-14-11-屏幕截图%202025-02-22%20171407.png)

### 2.6 节点带参宏小函数

在 list.h 中，还定义了各种各样的带参宏，方便对节点做一些简单的操作

```c
/* 初始化节点的拥有者 */
#define listSET_LIST_ITEM_OWNER(pxListItem, pxOwner) ((pxListItem)->pvOwner = (void*)(pxOwner))
/* 获取节点拥有者 */
#define listGET_LIST_ITEM_OWNER(pxListItem) ((void*)(pxListItem)->pvOwner)
/* 设置节点排序值 */
#define listSET_LIST_ITEM_VALUE(pxListItem, xValue) ((pxListItem)->xItemValue = (xValue))
/* 获取节点排序值 */
#define listGET_LIST_ITEM_VALUE(pxListItem) ((pxListItem)->xItemValue)
/* 获取链表根节点的节点计数器的值 */
#define listGET_ITEM_VALUE_OF_HEAD_ENTRY(pxList) ((pxList)->pxIndex.pxNext->xItemValue)
/* 获取链表的入口节点 */
#define listGET_HEAD_ENTRY(pxList) ((pxList)->pxIndex.pxNext)
/* 获取链表的尾节点 */
#define listGET_END_ENTRY(pxList) ((ListItem_t *)&(pxList)->xListEnd)
/* 获取节点的下一个节点 */
#define listGET_NEXT(pxListItem) ((pxListItem)->pxNext)
/* 判断链表是否为空 */
#define listLIST_IS_EMPTY(pxList) ((pxList)->uxNumberOfItems == (UBaseType_t)0U)
/* 获取链表的节点数 */
#define listCURRENT_LIST_LENGTH(pxList) ((pxList)->uxNumberOfItems)
/* 获取链表的第一个节点的OWMER,即TCB */
#define listGET_OWNER_OF_NEXT_ENTRY(pxTCB, pxList)
{
    List_t * const pxConstList = (pxList); \
    (pxConstList)->pxIndex = (pxConstList)->pxIndex.pxNext; \
    if((void*)(pxConstList)->pxIndex == (void*)&(pxConstList)->xListEnd) \
    {
        (pxConstList)->pxIndex = (pxConstList)->pxIndex.pxNext; 
    }
    (pxTCB) = (pxConstList)->pxIndex.pxOwner;
}
```

## 3. 链表插入实验

我们新建一个根节点（也可以理解为链表）和三个普通节点，然后将这三个普通节点按照节点的排序辅助值做升序排列插入到链表中

```c
#include "list.h"

struct xLIST List_Test; // 定义链表根节点

// 定义三个节点
struct xLIST_ITEM List_Item1;
struct xLIST_ITEM List_Item2;
struct xLIST_ITEM List_Item3;

int main(void)
{	
  vListInitialise(&List_Test); // 初始化链表根节点

  vListInitialiseItem(&List_Item1); // 初始化节点1
  List_Item1.xItemValue = 1; // 辅助值，用来排序
    
  
  vListInitialiseItem(&List_Item2);
  List_Item2.xItemValue = 2;
    
  vListInitialiseItem(&List_Item3);
  List_Item3.xItemValue = 3;
    
  // 插入节点到链表中
  // 运行代码后，节点会根据辅助值进行升序排序1->2->3
  vListInsert(&List_Test, &List_Item2);    
  vListInsert(&List_Test, &List_Item1);
  vListInsert(&List_Test, &List_Item3);    
  for(;;)
	{
		/* 啥事不干 */
	}
}

```

将程序编译好之后，点击调试按钮，然后全速运行，再然后把 List_Test、 List_Item1 、List_Item2 和 List_Item3 这四个全局变量添加到观察窗口

![屏幕截图 2025-02-22 174033.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/22-17-40-39-屏幕截图%202025-02-22%20174033.png)
