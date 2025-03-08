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

static TaskHandle_t Take_Task_Handle = NULL;
static TaskHandle_t Give_Task_Handle = NULL;

/* 内核对象句柄 */
SemaphoreHandle_t CountSem_Handle = NULL;
/* 全局变量声明 */
/* 宏定义 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void Take_Task(void* pvParameters);
static void Give_Task(void* pvParameters);

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
    printf("这一个FreeRTOS任务通知代替计数信号量的例子\r\n");
    printf("车位默认10个，按下KEY1申请车位，按下KEY2释放车位\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )Take_Task,
                        (const char*)"Take_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2,
                        (TaskHandle_t*)&Take_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("创建Take_Task成功\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )Give_Task,
                        (const char*)"Give_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&Give_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("创建Give_Task成功\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

// Taske_Task函数，申请车位
static void Take_Task(void* pvParameters)
{
  uint32_t take_num = pdTRUE;
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      // 获取任务通知，没获取到则不等待
      take_num = ulTaskNotifyTake(pdFALSE, 0);
      if(take_num > 0)
      {
        printf("KEY1被按下，成功申请到车位。当前车位为 %d\r\n", take_num-1);
      }
      else
      {
        printf("KEY1被按下，但没有车位可用。\r\n");
      }
    }
    vTaskDelay(20);
  }
}

static void Give_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
  while(1)
  {
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xTaskNotifyGive(Take_Task_Handle); // 通知Take_Task任务，有车位可用
      if(xReturn == pdPASS)
      {
        printf("KEY2被按下，成功释放车位。\r\n");
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
