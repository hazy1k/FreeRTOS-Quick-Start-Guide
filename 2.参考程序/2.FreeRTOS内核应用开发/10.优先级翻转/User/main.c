/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h" 
#include "semphr.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t LowPriority_Task_Handle = NULL; // 低优先级任务句柄
static TaskHandle_t MidPriority_Task_Handle = NULL; // 中优先级任务句柄
static TaskHandle_t HighPriority_Task_Handle = NULL; // 高优先级任务句柄

/* 内核对象句柄 */
SemaphoreHandle_t BinarySem_Handle = NULL; // 二值信号量句柄
/* 全局变量声明 */
/* 宏定义 */


/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void LowPriority_Task(void* pvParameters); // 低优先级任务函数
static void MidPriority_Task(void* pvParameters); // 中优先级任务函数
static void HighPriority_Task(void* pvParameters); // 高优先级任务函数

static void BSP_Init(void); // 板级初始化函数

// 主函数启动流程
/*
    1.BSP初始化
    2.创建APP任务
    3.启动FreeRTOS,开启调度
*/
int main(void)
{
    BaseType_t xReturn = pdPASS; // 定义一个创建信息返回值，默认为pdPASS
    BSP_Init(); // 板级初始化
    printf("FreeRTOS 优先级翻转实验\r\n");
    // 创建AppTaskCreate任务   
    xReturn = xTaskCreate((TaskFunction_t )AppTaskCreate,
                        (const char*)"AppTaskCreate",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)1, 
                        (TaskHandle_t*)&AppTaskCreate_Handle); 
    if(xReturn == pdPASS)
    {
        vTaskStartScheduler(); // 启动FreeRTOS
    }
    else
    {
        return -1;
    }
    while(1);
}

// 为了方便管理，所有任务创建函数都在AppTaskCreate中实现
static void AppTaskCreate(void)
{
  BaseType_t xReturn = pdPASS;
  taskENTER_CRITICAL(); // 进入临界区

  BinarySem_Handle = xSemaphoreCreateBinary(); // 创建二值信号量
  if(BinarySem_Handle != NULL)
  {
    printf("二值信号量创建成功！\r\n");
  }
  xReturn = xSemaphoreGive(BinarySem_Handle); // 释放二值信号量

  // 创建低优先级任务
  xReturn = xTaskCreate((TaskFunction_t )LowPriority_Task,
                        (const char*)"LowPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LowPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
      printf("低优先级任务创建成功！\r\n");
  }
  else
  {
    printf("低优先级任务创建失败！\r\n");
  }

  // 创建中优先级任务
  xReturn = xTaskCreate((TaskFunction_t )MidPriority_Task,
                        (const char*)"MidPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3, 
                        (TaskHandle_t*)&MidPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("中优先级任务创建成功！\r\n");
  }
  else
  {
    printf("中优先级任务创建失败！\r\n");
  }

  // 创建高优先级任务
  xReturn = xTaskCreate((TaskFunction_t )HighPriority_Task,
                        (const char*)"HighPriority_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)4, 
                        (TaskHandle_t*)&HighPriority_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("高优先级任务创建成功！\r\n");
  }
  else
  {
    printf("高优先级任务创建失败！\r\n");
  }
  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

// 低优先级任务函数
static void LowPriority_Task(void* pvParameters)
{
  static uint32_t i;
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    printf("低优先级获取信号量中...\r\n");
    xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY); // 低优先级任务获取信号量
    if(xReturn == pdTRUE)
    {
      printf("低优先级获取信号量成功！\r\n");
    }
    for(i = 0; i < 1000000; i++) // 模拟低优先级任务长时间占用信号量
    {
      taskYIELD(); // 任务调度
    }
    printf("低优先级释放信号量\r\n");
    xReturn = xSemaphoreGive(BinarySem_Handle); // 低优先级任务释放信号量
    LED1_TOGGLE();
    vTaskDelay(500);
  }
}

// 中优先级任务函数
static void MidPriority_Task(void* pvParameters)
{
  while(1)
  {
    printf("中优先级任务运行中...\r\n");
    vTaskDelay(500);
  }
}

// 高优先级任务函数
static void HighPriority_Task(void* pvParameters)
{
  BaseType_t xReturn = pdTRUE;
  while(1)
  {
    printf("高优先级申请获取信号量\r\n");
    xReturn = xSemaphoreTake(BinarySem_Handle, portMAX_DELAY); // 高优先级任务申请获取信号量
    if(xReturn == pdTRUE)
    {
      printf("高优先级获取信号量成功！\r\n");
    }
    LED1_TOGGLE();
    xReturn = xSemaphoreGive(BinarySem_Handle); // 高优先级任务释放信号量
    vTaskDelay(500);
  }
}

// 板级初始化函数
static void BSP_Init(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); // 设置中断优先级分组4
    LED_Init();
    USART_Config();
    Key_GPIO_Config();
}
