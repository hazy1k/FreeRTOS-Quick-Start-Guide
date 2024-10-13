# 第四章 创建多任务-SRAM动态内存

## 1. 代码分析

创建多任务只需要按照创建单任务的套路依葫芦画瓢即可，接下来我们创建两个任务，任务 1 让一个 LED 灯闪烁，任务 2 让另外一个 LED 闪烁，两个 LED 闪烁的频率不一样，下面看代码：

### 1.1 创建任务句柄

```c
// 创建任务句柄
static TaskHandle_t AppTaskCreate_Handle = NULL;
// LED1任务句柄
static TaskHandle_t LED1_Task_Handle = NULL;
// LED2任务句柄
static TaskHandle_t LED2_Task_Handle = NULL;
```

### 1.2 函数声明

```c
// 函数声明
static void AppTaskCreate(void); // 用于创建任务
static void LED1_Task(void* pvParameters);// LED1_Task任务实现 
static void LED2_Task(void* pvParameters);// LED2_Task任务实现 
static void BSP_Init(void);  // 用于初始化板载相关资源
```

### 1.3 任务创建函数

```c
// 任务创建函数
// 为了方便管理，所有的任务创建函数都放在这个函数里面
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  taskENTER_CRITICAL();        // 进入临界区
  // 创建LED1_Task任务
  xReturn = xTaskCreate((TaskFunction_t)LED1_Task,// 任务入口函数 
                        (const char*)"LED1_Task", // 任务名字 
                        (uint16_t)512,   // 任务栈大小 
                        (void*)NULL,	   // 任务入口函数参数 
                        (UBaseType_t)2,	 // 任务的优先级 
                        (TaskHandle_t*)&LED1_Task_Handle);// 任务控制块指针
  if(pdPASS == xReturn)
    printf("创建LED1_Task任务成功!\r\n");
	// 创建LED2_Task任务
  xReturn = xTaskCreate((TaskFunction_t)LED2_Task,// 任务入口函数 
                        (const char*)"LED2_Task", // 任务名字 
                        (uint16_t)512,   // 任务栈大小
                        (void*)NULL,	   // 任务入口函数参数 
                        (UBaseType_t)3,	 // 任务的优先级 
                        (TaskHandle_t*)&LED2_Task_Handle);// 任务控制块指针 
  if(pdPASS == xReturn)
    printf("创建LED2_Task任务成功!\r\n");
  vTaskDelete(AppTaskCreate_Handle); // 删除AppTaskCreate任务
  taskEXIT_CRITICAL();            //退出临界区
}
```

### 1.4 任务具体实现

```c
// LED1_Task任务实现 
static void LED1_Task(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        vTaskDelay(500);
        printf("LED1_Task Running,LED1_ON\r\n");
        LED1_OFF;     
        vTaskDelay(500);    		
        printf("LED1_Task Running,LED1_OFF\r\n");
    }
}

// LED2_Task任务实现 
static void LED2_Task(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        vTaskDelay(500);   
        printf("LED2_Task Running,LED2_ON\r\n");
        LED2_OFF;     
        vTaskDelay(500);   	 		
        printf("LED2_Task Running,LED2_OFF\r\n");
    }
}
```

### 1.5 板载资源初始化

```c
// 板载相关资源初始化函数
static void BSP_Init(void)
{

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);	
	// LED 初始化 
	LED_GPIO_Config();
	// 串口初始化
	USART_Config();
}
```

### 1.6 主函数

```c
// 主函数
int main(void)
{	
  BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
  // 开发板硬件初始化
  BSP_Init();
  printf("这是一个[野火]-STM32全系列开发板-FreeRTOS-动态创建多任务实验!\r\n");
  // 创建AppTaskCreate任务
  xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,// 任务入口函数 
                        (const char*)"AppTaskCreate", // 任务名字 
                        (uint16_t)512,  // 任务栈大小
                        (void*)NULL,    // 任务入口函数参数 
                        (UBaseType_t)1, // 任务的优先级 
                        (TaskHandle_t*)&AppTaskCreate_Handle);// 任务控制块指针  
  // 启动任务调度           
  if(pdPASS == xReturn)
    vTaskStartScheduler();// 启动任务，开启调度 
  else
    return -1;  
  while(1);   /* 正常不会执行到这里 */    
}
```

目前多任务我们只创建了两个，如果要创建 3 个、 4 个甚至更多都是同样的套路，容易忽略的地方是任务栈的大小，每个任务的优先级。大的任务，栈空间要设置大一点，重要的任务优先级要设置的高一点。


