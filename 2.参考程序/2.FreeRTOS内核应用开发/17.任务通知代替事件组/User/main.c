/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"
#include "limits.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t LED_Task_Handle = NULL; // LED任务句柄
static TaskHandle_t KEY_Task_Handle = NULL; // 按键任务句柄

/* 内核对象句柄 */
static EventGroupHandle_t Event_Handle = NULL;
/* 全局变量声明 */
/* 宏定义 */
#define KEY1_EVENT (0x01 << 0) // 设置事件掩码的位0
#define KEY2_EVENT (0x01 << 1) // 设置事件掩码的位1

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void LED_Task(void* pvParameters);
static void KEY_Task(void* pvParameters);

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
    printf("这是一个FreeRTOS优先级翻转实验。\r\n");
    printf("按下KEY1或者KEY2发送事件任务通知。\r\n");
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

  Event_Handle = xEventGroupCreate(); // 创建事件组
  if(Event_Handle != NULL)
  {
    printf("事件组创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )LED_Task,
                        (const char*)"LED_Task",
                        (uint16_t)512,  
                        (void*)NULL,  
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LED_Task_Handle); // 创建LED任务
  if(xReturn == pdPASS)
  {
    printf("LED任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )KEY_Task,
                        (const char*)"KEY_Task",
                        (uint16_t)512,  
                        (void*)NULL,  
                        (UBaseType_t)3, 
                        (TaskHandle_t*)&KEY_Task_Handle); // 创建按键任务
  if(xReturn == pdPASS)
  {
    printf("按键任务创建成功。\r\n");
  }

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

static void LED_Task(void* pvParameters)
{
  uint32_t r_event;
  uint32_t last_event = 0;
  BaseType_t xReturn = pdTRUE;
  while(1)
  {
    xReturn = xTaskNotifyWait(0x0, ULONG_MAX, &r_event, portMAX_DELAY); // 等待事件通知
    if(xReturn == pdTRUE)
    {
      last_event |= r_event; // 记录事件
      if(last_event == (KEY1_EVENT | KEY2_EVENT))
      {
        last_event = 0; // 清空事件
        printf("KEY1和KEY2同时按下，翻转LED。\r\n");
        LED1_TOGGLE(); // 翻转LED
      }
      else
      {
        last_event = r_event;
      }
    }
  }
}

static void KEY_Task(void* pvParameters)
{
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      printf("KEY1按下。\r\n");
      xTaskNotify(LED_Task_Handle, (uint32_t)KEY1_EVENT, (eNotifyAction)eSetBits); // 发送KEY1事件通知
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      printf("KEY2按下。\r\n");
      xTaskNotify(LED_Task_Handle, (uint32_t)KEY2_EVENT, (eNotifyAction)eSetBits); // 发送KEY2事件通知
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
