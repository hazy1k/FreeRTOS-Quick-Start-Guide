# 第一章 移植FreeRTOS到STM32

## 1. FreeRTOS文件夹内容简介

### 1.1 FreeRTOS文件夹

FreeRTOS 包含 Demo 例程和内核源码（比较重要，我们就需要提取该目录下的大部分

文件），FreeRTOS 文件夹下的 Source 文件夹里面包含的是 FreeRTOS 内核的源代码，我们移植 FreeRTOS 的时候就需要这部分源代码； FreeRTOS 文件夹下的Demo 文件夹里面包含了 FreeRTOS 官方为各个单片机移植好的工程代码， FreeRTOS 为了推广自己，会给各种半导体厂商的评估板写好完整的工程程序，这些程序就放在 Demo 这个目录下，这部分 Demo 非常有参考价值。我们把 FreeRTOS 到 STM32 的时候， FreeRTOSConfig.h 这个头文件就是从这里拷贝过来的，下面我们对 FreeRTOS 的文件夹进行分析说明。

![屏幕截图 2024 10 10 140006](https://img.picgo.net/2024/10/10/-2024-10-10-1400069f3daebf8a9fed11.png)

#### 1.1.1 Source 文件夹

这里我们再重点分析下 FreeRTOS/ Source 文件夹下的文件，具体见图，编号①和③包含的是 FreeRTOS 的通用的头文件和 C 文件，这两部分的文件试用于各种编译器和处理器，是通用的。需要移植的头文件和 C 文件放在编号②portblle 这个文件夹。

![屏幕截图 2024 10 10 155818](https://img.picgo.net/2024/10/10/-2024-10-10-155818d7f3732fe991b788.png)

我们打开 portblle 这个文件夹，可以看到里面很多与编译器相关的文件夹， 在不同的编译器中使用不同的支持文件。编号①中的 KEIL 就是我们就是我们使用的编译器，当年打开 KEIL 文件夹的时候，你会看到一句话“See-also-the-RVDS-directory.txt”，其实 KEIL里面的内容跟 RVDS 里面的内容一样，所以,我们只需要编号③RVDS 文件夹里面的内容即可。而编号②MemMang 文件夹下存放的是跟内存管理相关的，稍后具体介绍， portblle 文件夹内容具体见图

![屏幕截图 2024 10 10 163230](https://img.picgo.net/2024/10/10/-2024-10-10-1632300f9c23420fcb19fd.png)

打开 RVDS 文件夹， 下面包含了各种处理器相关的文件夹，从文件夹的名字我们就非常熟悉了，我们学习的 STM32 有 M0、 M3、 M4 等各种系列， FreeRTOS 是一个软件，单片机是一个硬件， FreeRTOS 要想运行在一个单片机上面，它们就必须关联在一起，那么怎

么关联？还是得通过写代码来关联，这部分关联的文件叫接口文件，通常由汇编和 C 联合编写。这些接口文件都是跟硬件密切相关的，不同的硬件接口文件是不一样的，但都大同小异。编写这些接口文件的过程我们就叫移植，移植的过程通常由 FreeRTOS 和 mcu 原厂的人来负责，移植好的这些接口文件就放在 RVDS 这个文件夹的目录下：

![屏幕截图 2024 10 10 163433](https://img.picgo.net/2024/10/10/-2024-10-10-16343359bb699997244786.png)

FreeRTOS 为我们提供了 cortex-m0、 m3、 m4 和 m7 等内核的单片机的接口文件，只要是使用了这些内核的 mcu 都可以使用里面的接口文件。通常网络上出现的叫“移植某某某RTOS 到某某某 MCU”的教程，其实准确来说，不能够叫移植，应该叫使用官方的移植，因为这些跟硬件相关的接口文件， RTOS 官方都已经写好了，我们只是使用而已。我们本章讲的移植也是使用 FreeRTOS 官方的移植，关于这些底层的移植文件我们已经在第一部分有非常详细的讲解，这里我们直接使用即可。

我们这里以 ARM_CM3 这个文件夹为例，看看里面的文件，里面只有“port.c”与“portmacro.h”两个文件， port.c 文件里面的内容是由 FreeRTOS 官方的技术人员为 Cortex-M3 内核的处理器写的接口文件，里面核心的上下文切换代码是由汇编语言编写而成，对技术员的要求比较高，我们刚开始学习的之后只需拷贝过来用即可，深入的学习可以放在后面的日子；portmacro.h 则是 port.c 文件对应的头文件，主要是一些数据类型和宏定义，具体见图

![屏幕截图 2024 10 10 164409](https://img.picgo.net/2024/10/10/-2024-10-10-164409a4a009a8e078b26d.png)

编号②MemMang 文件夹下存放的是跟内存管理相关的，总共有五个 heap 文件以及一个 readme 说明文件，这五个 heap 文件在移植的时候必须使用一个，因为 FreeRTOS 在创建内核对象的时候使用的是动态分配内存，而这些动态内存分配的函数则在这几个文件里面实现，不同的分配算法会导致不同的效率与结果，后面在内存管理中我们会讲解每个文件的区别，由于现在是初学，所以我们选用 heap4.c 即可。

![屏幕截图 2024 10 10 164610](https://img.picgo.net/2024/10/10/-2024-10-10-1646101f5c4a305c3e6e22.png)

#### 1.1.2 Demo文件夹

这个目录下内容就是 Deme 例程，我们可以直接打开里面的工程文件， 各种开发平台的完整 Demo，开发者可以方便的以此搭建出自己的项目，甚至直接使用。 FreeRTOS 当然也为 ST 写了很多 Demo，其中就有 F1、 F4、 F7 等工程，这样子对我们学习 FreeRTOS 是非常方便的，当遇到不懂的直接就可以参考官方的 Demo：

![屏幕截图 2024 10 10 164709](https://img.picgo.net/2024/10/10/-2024-10-10-164709c9a33d27ab3fbf1b.png)

#### 1.1.3 License文件夹

这里面只有一个许可文件“license.txt”，用 FreeRTOS 做产品的话就需要看看这个文件，但是我们是学习 FreeRTOS，所以暂时不需要理会这个文件。

### 1.2 FreeRTOS-Plus 文件夹

FreeRTOS-Plus 文件夹里面包含的是第三方的产品，一般我们不需要使用， FreeRTOSPlus 的预配置演示项目组件（组件大多数都要收费） ，大多数演示项目都是在 Windows 环境中运行的，使用 FreeRTOS windows 模拟器，所以暂时不需要关注这个文件夹。

### 1.3 HTML文件

一些直接可以打开的网页文件，里面包含一些关于 FreeRTOS 的介绍，是 FreeRTOS官方人员所写，所以都是英文的，有兴趣可以打开看看，具体相关内容可以看 HTML 文件名称。

## 2. 往裸机工程添加FreeRTOS源码

### 2.1 提取 FreeRTOS 最简源码

在前一章节中，我们看到了 FreeRTOS 源码中那么多文件，一开始学我们根本看不过来那么多文件，我们需要提取源码中的最简洁的部分代码，方便同学们学习，更何况我们学习的只是 FreeRTOS 的实时内核中的知识，因为这才是 FreeRTOS 的核心，那些 demo 都是基于此移植而来的， 而其他无关的我们暂时不需要学习，下面提取源码的操作过程。

1. 首先在我们的 STM32 裸机工程模板根目录下新建一个文件夹，命名为“FreeRTOS”，并且在 FreeRTOS 文件夹下新建两个空文件夹，分别命名为“src”与“port”， src 文件夹用于保存 FreeRTOS 中的核心源文件，也就是我们常说的‘.c 文件’， port 文件夹用于保存内存管理以及处理器架构相关代码，这些代码FreeRTOS 官方已经提供给我们的，直接使用即可，在前面已经说了， FreeRTOS是软件，我们的开发版是硬件，软硬件必须有桥梁来连接，这些与处理器架构相关的代码， 可以称之为 RTOS 硬件接口层，它们位于 FreeRTOS/Source/Portable 文件夹下。

2. 打开 FreeRTOS V9.0.0 源码，在“FreeRTOSv9.0.0\FreeRTOS\Source”目录下找到所有的‘.c 文件’，将它们拷贝到我们新建的 src 文件夹中

![屏幕截图 2024 10 10 165115](https://img.picgo.net/2024/10/10/-2024-10-10-1651154fdcb44562568716.png)

3. 打开 FreeRTOS V9.0.0 源码，在“FreeRTOSv9.0.0\FreeRTOS\Source\portable”目录下找到“ MemMang”文件夹与“ RVDS”文件夹，将它们拷贝到我们新建的port 文件夹中，具体见：

![屏幕截图 2024 10 10 165211](https://img.picgo.net/2024/10/10/-2024-10-10-165211eab52b7e239c80b7.png)

4. 打开 FreeRTOS V9.0.0 源码，在“FreeRTOSv9.0.0\ FreeRTOS\Source”目录下找到“include”文件夹，它是我们需要用到 FreeRTOS 的一些头文件，将它直接拷贝到我们新建的 FreeRTOS 文件夹中，完成这一步之后就可以看到我们新建的FreeRTOS 文件夹已经有 3 个文件夹，这 3个文件夹就包含 FreeRTOS 的核心文件，至此， FreeRTOS 的源码就提取完成

![屏幕截图 2024 10 10 165315](https://img.picgo.net/2024/10/10/-2024-10-10-16531509d6f7a6a95ce8bd.png)

### 2.2 拷贝FreeRTOS到裸机工程根目录

鉴于 FreeRTOS 容量很小，我们直接将刚刚提取的整个 FreeRTOS 文件夹拷贝到我们的 STM32 裸机工程里面，让整个 FreeRTOS 跟随我们的工程一起发布，使用这种方法打包的 FreeRTOS 工程，即使是将工程拷贝到一台没有安装 FreeRTOS 支持包（ MDK 中有FreeRTOS 的支持包）的电脑上面都是可以直接使用的，因为工程已经包含了 FreeRTOS 的源码。

![屏幕截图 2024 10 10 165435](https://img.picgo.net/2024/10/10/-2024-10-10-1654354e03a0560a344e36.png)

图中 FreeRTOS 文件夹下就是我们提取的 FreeRTOS 的核心代码，该文件夹下的具体内容作用在前面就已经描述的很清楚了，这里就不再重复赘述。

### 2.3 拷贝FreeRTOSConfig.h文件到user文件夹

FreeRTOSConfig.h 文件是 FreeRTOS 的工程配置文件，因为 FreeRTOS 是可以裁剪的实时操作内核，应用于不同的处理器平台，用户可以通过修改这个 FreeRTOS 内核的配置头文件来裁剪 FreeRTOS 的功能，所以我们把它拷贝一份放在 user 这个文件夹下面。

打开 FreeRTOSv9.0.0 源码，在“ FreeRTOSv9.0.0\FreeRTOS\Demo”文件夹下面找到“ CORTEX_STM32F103_Keil ” 这个文件夹，双击打开，在其根目录下找到这个“FreeRTOSConfig.h”文件，然后拷贝到我们工程的 user 文件夹下即可，等下我们需要对这个文件进行修改。 user 文件夹，见名知义我们就可以知道里面存放的文件都是用户自己编写的。

### 2.4 添加 FreeRTOS 源码到工程组文件夹

在上一步我们只是将 FreeRTOS 的源码放到了本地工程目录下，还没有添加到开发环境里面的组文件夹里面， FreeRTOS 也就没有移植到我们的工程中去。

#### 2.4.1 新建 FreeRTOS/src 和 FreeRTOS/port 组

接下来我们在开发环境里面新建 FreeRTOS/src 和 FreeRTOS/port 两个组文件夹，其中FreeRTOS/src 用于存放 src 文件夹的内容， FreeRTOS/port 用于存放 port\MemMang 文件夹与 port\RVDS\ARM_CM？文件夹的内容，“？”表示 3、 4 或者 7，具体选择哪个得看你使用的是野火哪个型号的 STM32 开发板.

然后我们将工程文件中 FreeRTOS 的内容添加到工程中去，按照已经新建的分组添加我们的 FreeRTOS 工程源码。

在 FreeRTOS/port 分组中添加 MemMang 文件夹中的文件只需选择其中一个即可，我们选择“heap_4.c”，这是 FreeRTOS 的一个内存管理源码文件。同时，需要根据自己的开发板型号在

FreeRTOS\port\RVDS\ARM_CM?中选择，“？”表示 3、 4 或者 7，具体选择哪个得看你使用的是野火哪个型号的 STM32 开发板。

然后在 user 分组中添加我们 FreeRTOS 的配置文件“FreeRTOSConfig.h”，因为这是头文件（ .h），所以需要在添加时选择文件类型为“ All files (*.*)”，至此我们的FreeRTOS 添加到工程中就已经完成，完成的效果具体见图

![屏幕截图 2024 10 10 165900](https://img.picgo.net/2024/10/10/-2024-10-10-1659001c39526be3268980.png)

#### 2.4.2 指定 FreeRTOS 头文件的路径

FreeRTOS 的源码已经添加到开发环境的组文件夹下面，编译的时候需要为这些源文件指定头文件的路径，不然编译会报错。 FreeRTOS 的源码里面只有 FreeRTOS\include 和FreeRTOS\port\RVDS\ARM_CM？这两个文件夹下面有头文件，只需要将这两个头文件的路径在开发环境里面指定即可。同时我们还将 FreeRTOSConfig.h 这个头文件拷贝到了工程根目录下的 user 文件夹下，所以 user 的路径也要加到开发环境里面。 FreeRTOS 头文件的路径添加完成后的效果具体见图

![屏幕截图 2024 10 10 170036](https://img.picgo.net/2024/10/10/-2024-10-10-170036a959bc347ee1d8e5.png)

至此， FreeRTOS 的整体工程基本移植完毕，我们需要修改 FreeRTOS 配置文件，按照我们的需求来进行修改。

### 2.5 修改FreeRTOSConfig.h

FreeRTOSConfig.h 是直接从 demo 文件夹下面拷贝过来的，该头文件对裁剪整个FreeRTOS 所需的功能的宏均做了定义，有些宏定义被使能，有些宏定义被失能，一开始我们只需要配置最简单的功能即可。要想随心所欲的配置 FreeRTOS 的功能，我们必须对这些宏定义的功能有所掌握，下面我们先简单的介绍下这些宏定义的含义，然后再对这些宏定义进行修改。

注意：此 FreeRTOSConfig.h 文件内容与我们从 demo 移植过来的 FreeRTOSConfig.h 文件不一样，因为这是我们野火修改过的 FreeRTOSConfig.h 文件，并不会影响 FreeRTOS 的功能，我们只是添加了一些中文注释，并且把相关的头文件进行分类，方便查找宏定义已经阅读，仅此而已。强烈建议使用我们修加工过的 FreeRTOSConfig.h 文件。

#### 2.5.1 FreeRTOSConfig.h 文件内容讲解

```c
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "stm32f10x.h"
#include "bsp_usart.h"


// 针对不同的编译器调用不同的stdint.h文件
#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    #include <stdint.h>
    extern uint32_t SystemCoreClock;
#endif

//断言
#define vAssertCalled(char,int) printf("Error:%s,%d\r\n",char,int)
#define configASSERT(x) if((x)==0) vAssertCalled(__FILE__,__LINE__)

/************************************************************************
 *               FreeRTOS基础配置配置选项 
 *********************************************************************/
/* 置1：RTOS使用抢占式调度器；置0：RTOS使用协作式调度器（时间片）
 * 
 * 注：在多任务管理机制上，操作系统可以分为抢占式和协作式两种。
 * 协作式操作系统是任务主动释放CPU后，切换到下一个任务。
 * 任务切换的时机完全取决于正在运行的任务。
 */
#define configUSE_PREEMPTION                    1

//1使能时间片调度(默认式使能的)
#define configUSE_TIME_SLICING                    1        

/* 某些运行FreeRTOS的硬件有两种方法选择下一个要执行的任务：
 * 通用方法和特定于硬件的方法（以下简称“特殊方法”）。
 * 
 * 通用方法：
 *      1.configUSE_PORT_OPTIMISED_TASK_SELECTION 为 0 或者硬件不支持这种特殊方法。
 *      2.可以用于所有FreeRTOS支持的硬件
 *      3.完全用C实现，效率略低于特殊方法。
 *      4.不强制要求限制最大可用优先级数目
 * 特殊方法：
 *      1.必须将configUSE_PORT_OPTIMISED_TASK_SELECTION设置为1。
 *      2.依赖一个或多个特定架构的汇编指令（一般是类似计算前导零[CLZ]指令）。
 *      3.比通用方法更高效
 *      4.一般强制限定最大可用优先级数目为32
 * 一般是硬件计算前导零指令，如果所使用的，MCU没有这些硬件指令的话此宏应该设置为0！
 */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION            1                       

/* 置1：使能低功耗tickless模式；置0：保持系统节拍（tick）中断一直运行
 * 假设开启低功耗的话可能会导致下载出现问题，因为程序在睡眠中,可用以下办法解决
 * 
 * 下载方法：
 *      1.将开发版正常连接好
 *      2.按住复位按键，点击下载瞬间松开复位按键
 *     
 *      1.通过跳线帽将 BOOT 0 接高电平(3.3V)
 *      2.重新上电，下载
 *    
 *             1.使用FlyMcu擦除一下芯片，然后进行下载
 *            STMISP -> 清除芯片(z)
 */
#define configUSE_TICKLESS_IDLE                                                    0   

/*
 * 写入实际的CPU内核时钟频率，也就是CPU指令执行频率，通常称为Fclk
 * Fclk为供给CPU内核的时钟信号，我们所说的cpu主频为 XX MHz，
 * 就是指的这个时钟信号，相应的，1/Fclk即为cpu时钟周期；
 */
#define configCPU_CLOCK_HZ                          (SystemCoreClock)

//RTOS系统节拍中断的频率。即一秒中断的次数，每次中断RTOS都会进行任务调度
#define configTICK_RATE_HZ                          (( TickType_t )1000)

//可使用的最大优先级
#define configMAX_PRIORITIES                      (32)

//空闲任务使用的堆栈大小
#define configMINIMAL_STACK_SIZE                ((unsigned short)128)

//任务名字字符串长度
#define configMAX_TASK_NAME_LEN                    (16)

 //系统节拍计数器变量数据类型，1表示为16位无符号整形，0表示为32位无符号整形
#define configUSE_16_BIT_TICKS                    0                      

//空闲任务放弃CPU使用权给其他同优先级的用户任务
#define configIDLE_SHOULD_YIELD                    1           

//启用队列
#define configUSE_QUEUE_SETS                      0    

//开启任务通知功能，默认开启
#define configUSE_TASK_NOTIFICATIONS    1   

//使用互斥信号量
#define configUSE_MUTEXES                            0    

//使用递归互斥信号量                                            
#define configUSE_RECURSIVE_MUTEXES            0   

//为1时使用计数信号量
#define configUSE_COUNTING_SEMAPHORES        0

/* 设置可以注册的信号量和消息队列个数 */
#define configQUEUE_REGISTRY_SIZE                10                                 

#define configUSE_APPLICATION_TASK_TAG          0                       


/*****************************************************************
              FreeRTOS与内存申请有关配置选项                                               
*****************************************************************/
//支持动态内存申请
#define configSUPPORT_DYNAMIC_ALLOCATION        1    
//支持静态内存
#define configSUPPORT_STATIC_ALLOCATION                    0                    
//系统所有总的堆大小
#define configTOTAL_HEAP_SIZE                    ((size_t)(36*1024))    


/***************************************************************
             FreeRTOS与钩子函数有关的配置选项                                            
**************************************************************/
/* 置1：使用空闲钩子（Idle Hook类似于回调函数）；置0：忽略空闲钩子
 * 
 * 空闲任务钩子是一个函数，这个函数由用户来实现，
 * FreeRTOS规定了函数的名字和参数：void vApplicationIdleHook(void )，
 * 这个函数在每个空闲任务周期都会被调用
 * 对于已经删除的RTOS任务，空闲任务可以释放分配给它们的堆栈内存。
 * 因此必须保证空闲任务可以被CPU执行
 * 使用空闲钩子函数设置CPU进入省电模式是很常见的
 * 不可以调用会引起空闲任务阻塞的API函数
 */
#define configUSE_IDLE_HOOK                        0      

/* 置1：使用时间片钩子（Tick Hook）；置0：忽略时间片钩子
 * 
 * 
 * 时间片钩子是一个函数，这个函数由用户来实现，
 * FreeRTOS规定了函数的名字和参数：void vApplicationTickHook(void )
 * 时间片中断可以周期性的调用
 * 函数必须非常短小，不能大量使用堆栈，
 * 不能调用以”FromISR" 或 "FROM_ISR”结尾的API函数
 */
 /*xTaskIncrementTick函数是在xPortSysTickHandler中断函数中被调用的。因此，vApplicationTickHook()函数执行的时间必须很短才行*/
#define configUSE_TICK_HOOK                        0           

//使用内存申请失败钩子函数
#define configUSE_MALLOC_FAILED_HOOK            0 

/*
 * 大于0时启用堆栈溢出检测功能，如果使用此功能 
 * 用户必须提供一个栈溢出钩子函数，如果使用的话
 * 此值可以为1或者2，因为有两种栈溢出检测方法 */
#define configCHECK_FOR_STACK_OVERFLOW            0   


/********************************************************************
          FreeRTOS与运行时间和任务状态收集有关的配置选项   
**********************************************************************/
//启用运行时间统计功能
#define configGENERATE_RUN_TIME_STATS            0             
 //启用可视化跟踪调试
#define configUSE_TRACE_FACILITY                      0    
/* 与宏configUSE_TRACE_FACILITY同时为1时会编译下面3个函数
 * prvWriteNameToBuffer()
 * vTaskList(),
 * vTaskGetRunTimeStats()
*/
#define configUSE_STATS_FORMATTING_FUNCTIONS    1                       


/********************************************************************
                FreeRTOS与协程有关的配置选项                                                
*********************************************************************/
//启用协程，启用协程以后必须添加文件croutine.c
#define configUSE_CO_ROUTINES                       0                 
//协程的有效优先级数目
#define configMAX_CO_ROUTINE_PRIORITIES       ( 2 )                   


/***********************************************************************
                FreeRTOS与软件定时器有关的配置选项      
**********************************************************************/
 //启用软件定时器
#define configUSE_TIMERS                            0                              
//软件定时器优先级
#define configTIMER_TASK_PRIORITY                (configMAX_PRIORITIES-1)        
//软件定时器队列长度
#define configTIMER_QUEUE_LENGTH                10                               
//软件定时器任务堆栈大小
#define configTIMER_TASK_STACK_DEPTH          (configMINIMAL_STACK_SIZE*2)    

/************************************************************
            FreeRTOS可选函数配置选项                                                     
************************************************************/
#define INCLUDE_xTaskGetSchedulerState       1                       
#define INCLUDE_vTaskPrioritySet                 1
#define INCLUDE_uxTaskPriorityGet                 1
#define INCLUDE_vTaskDelete                           1
#define INCLUDE_vTaskCleanUpResources           1
#define INCLUDE_vTaskSuspend                       1
#define INCLUDE_vTaskDelayUntil                     1
#define INCLUDE_vTaskDelay                           1
#define INCLUDE_eTaskGetState                       1
#define INCLUDE_xTimerPendFunctionCall         0
//#define INCLUDE_xTaskGetCurrentTaskHandle       1
//#define INCLUDE_uxTaskGetStackHighWaterMark     0
//#define INCLUDE_xTaskGetIdleTaskHandle          0


/******************************************************************
            FreeRTOS与中断有关的配置选项                                                 
******************************************************************/
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS               __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS               4                  
#endif
//中断最低优先级
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY            15     

//系统可管理的最高中断优先级
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5 

#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )    /* 240 */

#define configMAX_SYSCALL_INTERRUPT_PRIORITY     ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )


/****************************************************************
            FreeRTOS与中断服务函数有关的配置选项                         
****************************************************************/
#define xPortPendSVHandler     PendSV_Handler
#define vPortSVCHandler     SVC_Handler


/* 以下为使用Percepio Tracealyzer需要的东西，不需要时将 configUSE_TRACE_FACILITY 定义为 0 */
#if ( configUSE_TRACE_FACILITY == 1 )
#include "trcRecorder.h"
#define INCLUDE_xTaskGetCurrentTaskHandle               1   // 启用一个可选函数（该函数被 Trace源码使用，默认该值为0 表示不用）
#endif


#endif /* FREERTOS_CONFIG_H */
```

---

以下讲解根据代码从上往下顺序讲解

1. 针对不同的编译器调用不同的 stdint.h 文件，在 MDK 中，我们默认的是__CC_ARM。

2. 断言简介： 在使用 C 语言编写工程代码时，我们总会对某种假设条件进行检查，断言就是用于在代码中捕捉这些假设，可以将断言看作是异常处理的一种高级形式。断言表示为一些布尔表达式，程序员相信在程序中的某个特定表达式值为真。可以在任何时候启用和禁用断言验证，因此可以在测试时启用断言，而在发布时禁用断言。同样，程序投入运行后，最终用户在遇到问题时可以重新启用断言。它可以快速发现并定位软件问题，同时对系统错误进行自动报警。断言可以对在系统中隐藏很深，用其它手段极难发现的问题可以用断言来进行定位，从而缩短软件问题定位时间，提高系统的可测性。实际应用时，可根据具体情况灵活地设计断言。 这里只是使用宏定义实现了断言的功能，断言作用很大，特别是在调试的时候，而 FreeRTOS 中使用了很多断言接口 configASSERT，所以我们需要实现断言，把错误信息打印出来从而在调试中快速定位，打印信息的内容是xxx 文件 xxx 行(__FILE__,__LINE__)。

3. 置 1： FreeRTOS 使用抢占式调度器；置 0： FreeRTOS 使用协作式调度器（时间片）。抢占式调度： 在这种调度方式中，系统总是选择优先级最高的任务进行调度，并且 一旦高优先级的任务准备就绪之后，它就会马上被调度而不等待低优先级的任务主动放弃 CPU，高优先级的任务抢占了低优先级任务的 CPU 使用权，这就是抢占，在实习操作系统中，这样子的方式往往是最适用的。而协作式调度则是由任务主动放弃CPU，然后才进行任务调度。（注意：在多任务管理机制上，操作系统可以分为抢占式和协作式两种。 协作式操作系统是任务主动释放 CPU 后，切换到下一个任务。任务切换的时机完全取决于正在运行的任务）

4. 使能时间片调度(默认式使能的)。当优先级相同的时候，就会采用时间片调度，这意味着 RTOS 调度器总是运行处于最高优先级的就绪任务，在每个FreeRTOS 系统节拍中断时在相同优级的多个任务间进行任务切换。如果宏 configUSE_TIME_SLICING 设置为 0， FreeRTOS 调度器仍然总是运行处于最高优先级的就绪任务，但是当 RTOS 系统节拍中断发生时，相同优先级的多个任务之间不再进行任务切换，而是在执行完高优先级的任务之后才进行任务切换。一般来说， FreeRTOS 默认支持32 个优先级，很少情况会把 32 个优先级全用完，所以，官方建议采用抢占式调度。

5. FreeRTOS 支持两种方法选择下一个要执行的任务： 一个是软件方法扫描就绪链表，这种方法我们称为通用方法，configUSE_PORT_OPTIMISED_TASK_SELECTION 为 0 或者硬件不支持特殊方法，才使用通用方法获取下一个即将运行的任务，通用方法可以用于所有 FreeRTOS 支持的硬件平台，因为这种方法是完全用 C 语言实现， 所以效率略低于特殊方法，但不强制要求限制最大可用优先级数目；另一个是硬件方式查找下一个要运行的任务，必须将 configUSE_PORT_OPTIMISED_TASK_SELECTION 设置为 1，因为是必须依赖一个或多个特定架构的汇编指令（一般是类似计算前导零[CLZ]指令，在 M3、 M4、 M7 内核中都有，这个指令是用来计算一个变量从最高位开始的连续零的个数） ，所以效率略高于通用方法，但受限于硬件平台， 一般强制限定最大可用优先级数目为 32，这也是 FreeRTOS 官方为什么推荐使用 32 位优先级的原因。

6. 低功耗 tickless 模式。 置 1：使能低功耗 tickless 模式；置 0：保持系统节拍（tick）中断一直运行，如果不是用于低功耗场景，我们一般置 0 即可。

7. 配置 CPU 内核时钟频率，也就是 CPU 指令执行频率，通常称为Fclk ， Fclk 为供给 CPU 内核的时钟信号，我们所说的 cpu 主频为 XX MHz，就是指的这个时钟信号，相应的， 1/Fclk 即为 CPU 时钟周期，在野火 STM32 霸道开发板上系统时钟为SystemCoreClock = SYSCLK_FREQ_72MHz，也就是 72MHz。

8. FreeRTOS 系统节拍中断的频率。 表示操作系统每 1 秒钟产生多少个 tick， tick 即是操作系统节拍的时钟周期， 时钟节拍就是系统以固定的频率产生中断（时基中断），并在中断中处理与时间相关的事件，推动所有任务向前运行。时钟节拍需要依赖于硬件定时器，在 STM32 裸机程序中经常使用的 SysTick 时钟是 MCU 的内核定时器， 通常都使用该定时器产生操作系统的时钟节拍。 在 FreeRTOS 中， 系统延时和阻塞时间都是以 tick 为单位，配置 configTICK_RATE_HZ 的值可以改变中断的频率，从而间接改变了 FreeRTOS 的时钟周期（T=1/f） 。我们设置为 1000，那么 FreeRTOS 的时钟周期为 1ms，过高的系统节拍中断频率也意味着 FreeRTOS 内核占用更多的 CPU 时间，因此会降低效率，一般配置为 100~1000 即可。

9. 可使用的最大优先级，默认为 32 即可，官方推荐的也是 32。每一个任务都必须被分配一个优先级，优先级值从 0~ （configMAX_PRIORITIES - 1）之间。低优先级数值表示低优先级任务。空闲任务的优先级为 0（tskIDLE_PRIORITY），因此它是最低优先级任务。 FreeRTOS 调度器将确保处于就绪态的高优先级任务比同样处于就绪状态的低优先级任务优先获取处理器时间。换句话说， FreeRTOS 运行的永远是处于就绪态的高优先级任务。处于就绪状态的相同优先级任务使用时间片调度机制共享处理器时间。

10. 空闲任务默认使用的堆栈大小，默认为 128 字即可（在 M3、 M4、M7 中为 128*4 字节）， 堆栈大小不是以字节为单位而是以字为单位的，比如在 32 位架构下，栈大小为 100 表示栈内存占用 400 字节的空间。

11. 任务名字字符串长度， 这个宏用来定义该字符串的最大长度。这里定义的长度包括字符串结束符’ \0’ 。

12. 系统节拍计数器变量数据类型， 1 表示为 16 位无符号整形， 0 表示为 32 位无符号整形， STM32 是 32 位机器，所以默认使用为 0 即可，这个值位数的大小决定了能计算多少个 tick，比如假设系统以 1ms 产生一个 tick 中断的频率计时，那么 32 位无符号整形的值则可以计算 4294967295 个 tick，也就是系统从 0 运行到 4294967.295 秒的时候才溢出，转换为小时的话，则能运行 1193 个小时左右才溢出，当然，溢出就会重置时间，这点完全不用担心；而假如使用 16 位无符号整形的值，只能计算 65535 个 tick，在65.535 秒之后就会溢出，然后重置。

13. 控制任务在空闲优先级中的行为， 空闲任务放弃 CPU 使用权给其他同优先级的用户任务。仅在满足下列条件后，才会起作用， 1：启用抢占式调度； 2：用户任务优先级与空闲任务优先级相等。一般不建议使用这个功能，能避免尽量避免， 1：设置用户任务优先级比空闲任务优先级高， 2：这个宏定义配置为 0。

14. 启用消息队列，消息队列是 FreeRTOS 的 IPC 通信的一种，用于传递消息。

15. 开启任务通知功能，默认开启。 每个 FreeRTOS 任务具有一个 32位的通知值， FreeRTOS 任务通知是直接向任务发送一个事件， 并且接收任务的通知值是可以选择的， 任务通过接收到的任务通知值来解除任务的阻塞状态（假如因等待该任务通知而进入阻塞状态）。相对于队列、二进制信号量、计数信号量或事件组等 IPC 通信，使用任务通知显然更灵活。 官方说明： 相比于使用信号量解除任务阻塞，使用任务通知可以快45%（使用 GCC 编译器， -o2优化级别） ，并且使用更少的 RAM。

16. 使用互斥信号量。

17. 使用递归互斥信号量。

18. 使用计数信号量。

19. 设置可以注册的信号量和消息队列个数，用户可以根据自己需要修改即可， RAM 小的芯片尽量裁剪得小一些。

20. 支持动态分配申请，一般在系统中采用的内存分配都是动态内存分配。 FreeRTOS 同时也支持静态分配内存，但是常用的就是动态分配了。

21. FreeRTOS 内核总计可用的有效的 RAM 大小，不能超过芯片的RAM 大小，一般来说用户可用的内存大小会小于 configTOTAL_HEAP_SIZE 定义的大小，因为系统本身就需要内存。每当创建任务、队列、互斥量、软件定时器或信号量时， FreeRTOS内核会为这些内核对象分配RAM，这里的RAM都属于configTOTAL_HEAP_SIZE 指定的内存区。

22. 配置空闲钩子函数，钩子函数是类似一种回调函数，在任务执行到某个点的时候，跳转到对应的钩子函数执行，这个宏定义表示是否启用空闲任务钩子函数，这个函数由用户来实现，但是 FreeRTOS 规定了函数的名字和参数： void vApplicationIdleHook(void)，我们自定义的钩子函数不允许出现阻塞的情况。

23. 配置时间片钩子函数，与空闲任务钩子函数一样。这个宏定义表示是否启用时间片钩子函数，这个函数由用户来实现，但是 FreeRTOS 规定了函数的名字和参数： void vApplicationTickHook(void)，我们自定义的钩子函数不允许出现阻塞的情况。同时需要知道的是 xTaskIncrementTick 函数在 xPortSysTickHandler 中断函数中被调用的。因此， vApplicationTickHook()函数执行的时间必须很短才行，同时不能调用任何不是以” FromISR"或 "FROM_ISR”结尾的 API 函数。

24. 使用内存申请失败钩子函数。

25. 这个宏定义大于 0 时启用堆栈溢出检测功能，如果使用此功能，用户必须提供一个栈溢出钩子函数，如果使用的话，此值可以为 1 或者 2，因为有两种栈溢出检测方法。 使用该功能，可以分析是否有内存越界的情况。

26. 不启用运行时间统计功能。

27. 启用可视化跟踪调试。

28. 启用协程，启用协程以后必须添加文件 croutine.c，默认不使用，因为 FreeRTOS 不对协程做支持了。

29. 协程的有效优先级数目，当 configUSE_CO_ROUTINES 这个宏定义有效的时候才有效，默认即可。

30. 启用软件定时器。

31. 配置软件定时器任务优先级为最高优先级(configMAX_PRIORITIES-1) 。

32. 软件定时器队列长度，也就是允许配置多少个软件定时器的数量，其实 FreeRTOS 中理论上能配置无数个软件定时器，因为软件定时器是不基于硬件的。

33. 配置软件定时器任务堆栈大小，默认为(configMINIMAL_STACK_SIZE*2)。

34. 必须将 INCLUDE_XTaskGetSchedulerState 这个宏定义必须设置为 1 才能使用 xTaskGetSchedulerState()这个 API 函数接口。

35. INCLUDE_VTaskPrioritySet 这个宏定义必须设置为 1 才能使vTaskPrioritySet()这个 API 函数接口。

36. INCLUDE_uxTaskPriorityGet 这个宏定义必须设置为 1 才能使uxTaskPriorityGet()这个 API 函数接口。

37. INCLUDE_vTaskDelete 这 个宏 定义必 须设 置为 1 才能 使vTaskDelete()这个 API 函数接口。 其他都是可选的宏定义，根据需要自定义即可。

38. 定义__NVIC_PRIO_BITS 表示配置 FreeRTOS 使用多少位作为中断优先级，在 STM32 中使用 4位作为中断的优先级。

39. 如果没有定义，那么默认就是 4 位。

40. 配置中断最低优先级是15（一般配置为15）。configLIBRARY_LOWEST_INTERRUPT_PRIORITY 是用于配置 SysTick 与 PendSV 的。注意了：这里是中断优先级， 中断优先级的数值越小，优先级越高。而 FreeRTOS 的任务优先级是，任务优先级数值越小，任务优先级越低。

41. 配置系统可管理的最高中断优先级为5，configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 是用于配置 basepri 寄存器的，当 basepri 设置为某个值的时候，会让系统不响应比该优先级低的中断，而优先级比之更高的中断则不受影响。就是说当这个宏定义配置为 5 的时候，中断优先级数值在 0、 1、 2、 3、4 的这些中断是不受 FreeRTOS 管理的，不可被屏蔽，也不能调用 FreeRTOS 中的 API 函数接口，而中断优先级在 5 到 15 的这些中断是受到系统管理，可以被屏蔽的。

42. 对需要配置的 SysTick 与 PendSV 进行偏移（因为是高 4 位才有效），在 port.c 中会用到 configKERNEL_INTERRUPT_PRIORITY 这个宏定义来配置SCB_SHPR3（系统处理优先级寄存器，地址为： 0xE000 ED20） ，具体见图

![屏幕截图 2024 10 10 173649](https://img.picgo.net/2024/10/10/-2024-10-10-1736491181e11d42d32006.png)

43. configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY 是用于配置 basepri 寄存器的，让 FreeRTOS 屏蔽优先级数值大于这个宏定义的中断（数值越大，优先级越低），而 basepri 的有效位为高 4 位，所以需要进行偏移，因为 STM32 只使用了优先级寄存器中的 4 位，所以要以最高有效位对齐，具体见图：

还需要注意的是： 中断优先级 0（具有最高的逻辑优先级）不能被 basepri 寄存器屏蔽，因此， configMAX_SYSCALL_INTERRUPT_PRIORITY 绝不可以设置成 0。

![屏幕截图 2024 10 10 173823](https://img.picgo.net/2024/10/10/-2024-10-10-173823d3ed8b6901007de6.png)

为什么要屏蔽中断?

先了解一下什么是临界段！临界段用一句话概括就是一段在执行的时候不能被中断的代码段。在 FreeRTOS 里面，这个临界段最常出现的就是对全局变量的操作，全局变量就好像是一个枪把子，谁都可以对他开枪，但是我开枪的时候，你就不能开枪，否则就不知道是谁命中了靶子。

那么什么情况下临界段会被打断？一个是系统调度，还有一个就是外部中断。在FreeRTOS 中，系统调度，最终也是产生 PendSV中断，在 PendSV Handler 里面实现任务的切换，所以还是可以归结为中断。既然这样， FreeRTOS 对临界段的保护就很有必要了，在必要的时候将中断屏蔽掉，但是又必须保证某些特别紧急的中断的处理，比如像无人机的碰撞检测。

PRIMASK 和 FAULTMAST 是 Cortex-M 内核里面三个中断屏蔽寄存器中的两个，还有一个是 BASEPRI，有关这三个寄存器的详细用法见表格：

![屏幕截图 2024 10 10 173943](https://img.picgo.net/2024/10/10/-2024-10-10-1739431077596ab6c2b719.png)

44. configUSE_TRACE_FACILITY 这个宏定义是用于 FreeRTOS 可视化调试软件 Tracealyzer 需要的东西，我们现在暂时不需要，将configUSE_TRACE_FACILITY定义为 0 即可。

#### 2.5.2 FreeRTOSConfig.h 文件修改

FreeRTOSConfig.h 头文件的内容修改的不多，具体是：修改与对应开发板的头文件 ，如果是使用野火 STM32F1 的开发板，则包含 F1 的头文件#include "stm32f10x.h"，同理是使用了其它系列的开发板，则包含与开发板对应的头文件即可，当然还需要包含我们的串口的头文件“bsp_usart.h”，因为在我们 FreeRTOSConfig.h 中实现了断言操作，需要打印一些信息。其他根据需求修改即可， 具体见代码：

虽然 FreeRTOS 中默认是打开很多宏定义的，但是用户还是要根据需要选择打开与关闭，因为这样子的系统会更适合用户需要，更严谨与更加节省系统资源。

```c
#include "stm32f10x.h"
#include "bsp_usart.h"
```

## 3. 修改 stm32f10x_it.c

SysTick 中断服务函数是一个非常重要的函数， FreeRTOS 所有跟时间相关的事情都在里面处理， SysTick 就是 FreeRTOS 的一个心跳时钟，驱动着 FreeRTOS 的运行，就像人的心跳一样，假如没有心跳，我们就相当于“死了”，同样的， FreeRTOS 没有了心跳，那么它就会卡死在某个地方，不能进行任务调度，不能运行任何的东西，因此我们需要实现一个 FreeRTOS 的心跳时钟， FreeRTOS 帮我们实现了 SysTick 的启动的配置：在 port.c 文件中已经实现 vPortSetupTimerInterrupt()函数，并且 FreeRTOS 通用的 SysTick 中断服务函数也实现了：在 port.c 文件中已经实现 xPortSysTickHandler()函数，所以移植的时候只需要我们在 stm32f10x_it.c 文件中实现我们对应（STM32）平台上的 SysTick_Handler()函数即可。

FreeRTOS 为开发者考虑得特别多， PendSV_Handler()与 SVC_Handler()这两个很重要的函数都帮我们实现了，在 port.c 文件中已经实现 xPortPendSVHandler()与 vPortSVCHandler()函数，防止我们自己实现不了，那么在 stm32f10x_it.c 中就需要我们注释掉 PendSV_Handler() 与 SVC_Handler()这两个函数了，具体实现见代码：

```c
#include "stm32f10x_it.h"

#include "FreeRTOS.h"                    //FreeRTOS使用          
#include "task.h" 

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
//void SVC_Handler(void)
//{
//}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
//void PendSV_Handler(void)
//{
//}

///**
//  * @brief  This function handles SysTick Handler.
//  * @param  None
//  * @retval None
//  */
extern void xPortSysTickHandler(void);
//systick中断服务函数
void SysTick_Handler(void)
{    
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
      {
    #endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
    #if (INCLUDE_xTaskGetSchedulerState  == 1 )
      }
    #endif  /* INCLUDE_xTaskGetSchedulerState */
}
```

## 4. 修改main.c

我们将原来裸机工程里面 main.c 的文件内容全部删除，新增如下内容：

```c
#include "FreeRTOS.h"
#include "task.h"
// 开发板硬件bsp头文件 
#include "bsp_led.h"
#include "bsp_usart.h"

int main(void)
{    

}
```

目前我们还没有在 main 任务里面创建应用任务，但是系统是已经跑起来了，只有默认的空闲任务和 main 任务。要想看现象，得自己在 main 创建里面应用任务，如果创建任务，请看下一章“创建任务”。
