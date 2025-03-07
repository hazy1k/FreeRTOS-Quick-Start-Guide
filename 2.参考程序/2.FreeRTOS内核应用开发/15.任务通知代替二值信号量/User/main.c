/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t Receive1_Task_Handle = NULL; // 接收任务1句柄
static TaskHandle_t Receive2_Task_Handle = NULL; // 接收任务2句柄
static TaskHandle_t Send_Task_Handle = NULL; // 发送任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */
/* 宏定义 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void Receive1_Task(void* pvParameters); // 接收任务1函数
static void Receive2_Task(void* pvParameters); // 接收任务2函数
static void Send_Task(void* pvParameters); // 发送任务函数

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
    printf("这是一个FreeRTOS任务通知代替二值信号量的测试程序\r\n");
    printf("按下KEY1或者KEY2键进行任务与任务之间的同步\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )Receive1_Task,
                        (const char*)"Receive1_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&Receive1_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("创建接收任务1成功\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Receive2_Task,
                        (const char*)"Receive2_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&Receive2_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("创建接收任务2成功\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Send_Task,
                        (const char*)"Send_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)4,
                        (TaskHandle_t*)&Send_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("创建发送任务成功\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

// 接收任务1函数
static void Receive1_Task(void* pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待通知
    printf("接收任务1收到通知\r\n");
    LED1_TOGGLE();
  }
}

// 接收任务2函数
static void Receive2_Task(void* pvParameters)
{
  while(1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // 等待通知
    printf("接收任务2收到通知\r\n");
    LED2_TOGGLE();
  }
}

static void Send_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotifyGive(Receive1_Task_Handle); // 发送通知给接收任务1
      if(xReturn == pdPASS)
      {
        printf("发送任务给接收任务1成功\r\n");
      }
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotifyGive(Receive2_Task_Handle); // 发送通知给接收任务2
      if(xReturn == pdPASS)
      {
        printf("发送任务给接收任务2成功\r\n");
      }
    }
    vTaskDelay(20);
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
