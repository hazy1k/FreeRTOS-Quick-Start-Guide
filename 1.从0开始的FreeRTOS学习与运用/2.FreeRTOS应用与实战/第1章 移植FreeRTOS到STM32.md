# 第一章 移植FreeRTOS到STM32

## 1. FreeRTOS文件夹内容简介

### 1.1 FreeRTOS文件夹

FreeRTOS 包含 Demo 例程和内核源码（比较重要，我们就需要提取该目录下的大部分

文件），FreeRTOS 文件夹下的 Source 文件夹里面包含的是 FreeRTOS 内核的源代码，我们移植 FreeRTOS 的时候就需要这部分源代码； FreeRTOS 文件夹下的Demo 文件夹里面包含了 FreeRTOS 官方为各个单片机移植好的工程代码， FreeRTOS 为了推广自己，会给各种半导体厂商的评估板写好完整的工程程序，这些程序就放在 Demo 这个目录下，这部分 Demo 非常有参考价值。我们把 FreeRTOS 到 STM32 的时候， FreeRTOSConfig.h 这个头文件就是从这里拷贝过来的，下面我们对 FreeRTOS 的文件夹进行分析说明。

![屏幕截图 2024 10 10 140006](https://img.picgo.net/2024/10/10/-2024-10-10-1400069f3daebf8a9fed11.png)
