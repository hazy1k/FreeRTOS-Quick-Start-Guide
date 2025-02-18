# 第一章 新建FreeRTOS工程

## 1. 新建本地工程文件夹

![屏幕截图 2025-02-18 214555.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/18-21-46-26-屏幕截图%202025-02-18%20214555.png)

## 2. 在KEIL工程里添加文件

在工程里面添加好组之后，我们需要把本地工程里面新建好的文件添加到工程里面。具体为把 readme.txt 文件添加到 doc 组， main.c 添加到 user 组，至于 FreeRTOS 相关的文件我们还没有编写，那么 FreeRTOS 相关的组就暂时为空。

![屏幕截图 2025-02-18 215132.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/18-21-51-58-屏幕截图%202025-02-18%20215132.png)

![屏幕截图 2025-02-18 215146.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/18-21-52-02-屏幕截图%202025-02-18%20215146.png)

## 3. 调试配置

### 3.1 设置软件仿真

最后，我们再配置下调试相关的配置即可。为了方便，我们全部代码都用软件仿真，即不需要开发板也不需要仿真器，只需要一个 KEIL 软件即可

![屏幕截图 2025-02-18 215723.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/18-21-57-46-屏幕截图%202025-02-18%20215723.png)

### 3.2 修改时钟大小

在时钟相关文件 system_ARMCM3.c（system_ARMCM4.c 或 system_ARMCM7.c）的开头，有一段代码定义了系统时钟的大小为 25M。在软件仿真的时候，确保时间的准确性，代码里面的系统时钟跟软件仿真的时钟必须一致，所以 Optionsfor Target->Target 的时钟应该由默认的 12M 改成 25M，

![屏幕截图 2025-02-18 215910.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2025/02/18-21-59-14-屏幕截图%202025-02-18%20215910.png)




