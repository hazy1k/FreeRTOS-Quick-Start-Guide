# 第一章 FreeRTOS的编程风格

学习一个 RTOS， 搞懂它的编程的风格很重要，这可以大大提供我们阅读代码的效率。下面我们就以 FreeRTOS 里面的数据类型、变量名、 函数名和宏这几个方面做简单介绍。

## 1. 数据类型

在 FreeRTOS 中， 使用的数据类型虽然都是标准 C 里面的数据类型，但是针对不同的处理器，对标准 C 的数据类型又进行了重定义，给它们取了一个新的名字， 比如 char 重新定义了一个名字 portCHAR， 这里面的 port 表示接口的意思，就是 FreeRTOS 要移植到这些处理器上需要这些接口文件来把它们连接在一起。但是用户在写程序的时候并非一定要遵循 FreeRTOS 的风格， 我们还是可以直接用 C 语言的标准类型。在 FreeRTOS 中， int 型从不使用， 只使用 short 和 long 型。 在 Cortex-M 内核的 MCU 中， short 为 16 位， long 为 32位

![屏幕截图 2024 10 02 145822](https://img.picgo.net/2024/10/02/-2024-10-02-1458226766236be69b5ea0.png)

```c
#define portCHAR char
#define portFLOAT float
#define portDOUBLE double
#define portLONG long
#define portSHORT short
#define portSTACK_TYPE uint32_t
#define portBASE_TYPE long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

#if(configUSE_16_BIT_TICKS == 1 )
typedef uint16_t TickType_t;
#define portMAX_DELAY (TickType_t) 0xffff
#else
typedef uint32_t TickType_t;
#define portMAX_DELAY (TickType_t) 0xffffffffUL
```

在编程的时候，如果用户没有明确指定 char 的符号类型， 那么编译器会默认的指定char 型的变量为无符号或者有符号。正是因为这个原因，在 FreeRTOS 中，我们都需要明确的指定变量 char 是有符号的还是无符号的。 在 keil 中， 默认 char 是无符号的，但是也可以配置为有符号的，如图

![屏幕截图 2024 10 02 150657](https://img.picgo.net/2024/10/02/-2024-10-02-150657996df0a97dd9c3aa.png)

## 2. 变量名

在 FreeRTOS 中，定义变量的时候往往会把变量的类型当作前缀加在变量上， 这样的好处是让用户一看到这个变量就知道该变量的类型。 比如 char 型变量的前缀是 c， short 型变量的前缀是 s， long 型变量的前缀是 l， portBASE_TYPE 类型变量的前缀是 x。 还有其他的数据类型，比如数据结构，任务句柄， 队列句柄等定义的变量名的前缀也是 x。

如果一个变量是无符号型的那么会有一个前缀 u， 如果是一个指针变量则会有一个前缀 p。因此，当我们定义一个无符号的 char 型变量的时候会加一个 uc 前缀， 当定义一个char 型的指针变量的时候会有一个 pc 前缀。

## 3. 函数名

函数名包含了函数返回值的类型、 函数所在的文件名和函数的功能，如果是私有的函数则会加一个 prv（private） 的前缀。特别的， 在函数名中加入了函数所在的文件名， 这大大的帮助了用户提高寻找函数定义的效率和了解函数作用的目的， 具体的举例如下：

1. vTaskPrioritySet()函数的返回值为 void 型， 在task.c 这个文件中定义。

2. xQueueReceive()函数的返回值为 portBASE_TYPE 型， 在 queue.c 这个文件中定义。

3. vSemaphoreCreateBinary()函数的返回值为 void 型， 在 semphr.h 这个文件中定义。

## 4. 宏定义

宏均是由大写字母表示，并配有小写字母的前缀， 前缀用于表示该宏在哪个头文件定义，部分举例具体见表格

![屏幕截图 2024 10 02 151242](https://img.picgo.net/2024/10/02/-2024-10-02-1512421ca60c1074622d4c.png)

这里有个地方要注意的是信号量的函数都是一个宏定义，但是它的函数的命名方法是遵循函数的命名方法而不是宏定义的方法。

在贯穿 FreeRTOS 的整个代码中，还有几个通用的宏定义我们也要注意下，都是表示 0 和 1 的宏

![屏幕截图 2024 10 02 151338](https://img.picgo.net/2024/10/02/-2024-10-02-1513384d36ec36d9996124.png)


