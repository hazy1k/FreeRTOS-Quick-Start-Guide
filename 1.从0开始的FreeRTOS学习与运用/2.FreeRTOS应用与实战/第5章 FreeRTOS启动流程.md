# 第五章 FreeRTOS启动流程

在目前的 RTOS 中，主要有两种比较流行的启动方式，暂时还没有看到第三种，接下来我将通过伪代码的方式来讲解下这两种启动方式的区别，然后再具体分析下 FreeRTOS的启动流程。

## 1. 万事俱备，只欠东风

这种方法是在 main 函数中将硬件初始化， RTOS 系统初始化，所有任务的创建这些都弄好，这个我称之为万事都已经准备好。最后只欠一道东风，即启动 RTOS 的调度器，开始多任务的调度

```c
int main(void)
{
    HardWareInit(); // 硬件初始化
    RTOSInit(); // RTOS初始化
    RTOS_TaskCreate(Task1); // 创建任务1
    RTOS_TaskCreate(Task2); // 创建任务2
    /* ...... 其他任务创建...... */
    RTOS_Start(); // 启动RTOS
}

void Task1(void *arg)
{
 while(1);   
}
void Task2(void *arg)
{
 while(1);   
}
```

## 2. 小心翼翼，十分谨慎

这种方法是在 main 函数中将硬件和 RTOS 系统先初始化好，然后创建一个启动任务后就启动调度器，然后在启动任务里面创建各种应用任务，当所有任务都创建成功后，启动任务把自己删除

```c
int main(void)
{
    HardWareInit(); // 硬件初始化
    RTOSInit(); // RTOS初始化
    RTOS_TaskCreate(AppTaskCreate);
    RTOS_Start(); 
}

/* 起始任务，在里面创建任务 */
void AppTaskCreate(void *arg)
{
    /* 创建任务 1，然后执行 */
    RTOS_TaskCreate(Task1); 
    /* 当任务 1 阻塞时，继续创建任务 2，然后执行 */
    RTOS_TaskCreate(Task2);
    /* ......继续创建各种任务 */
    /* 当任务创建完成， 删除起始任务 */
    RTOS_TaskDelete(AppTaskCreate);
}

void Task1(void *arg)
{
    while (1)
    {
    /* 任务实体，必须有阻塞的情况出现 */
    }
}
```


