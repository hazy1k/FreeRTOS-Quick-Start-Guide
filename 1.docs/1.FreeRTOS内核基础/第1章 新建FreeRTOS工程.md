# 第一章 新建FreeRTOS工程

在开始写 FreeRTOS 内核之前，我们先新建一个 FreeRTOS 的工程， Device 选择Cortex-M3（Cortex-M4 或 Cortex-M7） 内核的处理器，调试方式选择软件仿真， 然后我们再开始一步一步地教大家把 FreeRTOS 内核从 0 到 1 写出来，让大家彻底搞懂 FreeRTOS 的内部实现和设计的哲学思想。最后我们再把 FreeRTOS 移植到STM32 开发板上，到了最后的移植其实已经非常简单，只需要换一下启动文件和添加 bsp 驱动就行。

## 1. 新建工程

首先打开 KEIL5 软件，新建一个工程，工程文件放在目录 Project 下面，名称可以随便取，但是必须是英文，不能是中文，切记。

当命名好工程名称，点击确定之后会弹出 Select Device for Target 的选项框，让我们选择处理器，这里我们选择 ARMCM3（ARMCM4 或 ARMCM7）

![屏幕截图 2025-06-28 150157.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/28-15-02-03-屏幕截图%202025-06-28%20150157.png)

选择好处理器，点击 OK 按钮后会弹出 Manage Run-Time Environment 选项框。这里我们在 CMSIS 栏选中 CORE 和 Device 栏选中 Startup 这两个文件即可

![屏幕截图 2025-06-28 150224.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/06/28-15-02-27-屏幕截图%202025-06-28%20150224.png)

其实这两个文件刚开始都是存放在 KEIL 的安装目录下，当我们配置 Manage RunTime Environment 选项框之后，软件就会把选中好的文件从 KEIL 的安装目录拷贝到我们的工程目录： Project\RTE\Device\ARMCM3 （ ARMCM4 或 ARMCM7 ） 下 面 。 其 中startup_ARMCM3.s（startup_ARMCM4.s 或 startup_ARMCM7.s）是汇编编写的启动文件， system_ARMCM3.c（startup_ARMCM4.c 或 startup_ARMCM7.c）是 C 语言编写的跟时钟相关的文件。更加具体的可直接阅读这两个文件的源码。只要是 Cortex-M3（ ARMCM4 或ARMCM7）内核的单片机，这两个文件都适用。


