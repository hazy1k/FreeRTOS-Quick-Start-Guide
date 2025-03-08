/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t LED_Task_Handle = NULL; // LED任务句柄
static TaskHandle_t RAM_Task_Handle = NULL; // 内存管理任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */
uint8_t *Test_Ptr = NULL;
/* 宏定义 */

/* 任务函数声明 */
static void AppTaskCreate(void); // 创建任务函数

static void LED_Task(void* pvParameters);
static void RAM_Task(void* pvParameters);

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
    printf("FreeRTOS内存管理实验\r\n");
    printf("按下KEY1申请内存，按下KEY2释放内存。\r\n");
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

  xReturn = xTaskCreate((TaskFunction_t )LED_Task,
                        (const char*)"LED_Task",
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)2, 
                        (TaskHandle_t*)&LED_Task_Handle); 
  if(xReturn == pdPASS)
  {
    printf("LED任务创建成功。\r\n");
  }
  xReturn = xTaskCreate((TaskFunction_t )RAM_Task,
                        (const char*)"RAM_Task",  
                        (uint16_t)512,  
                        (void*)NULL,
                        (UBaseType_t)3,
                        (TaskHandle_t*)&RAM_Task_Handle);
  if(xReturn == pdPASS)
  {
    printf("内存管理任务创建成功。\r\n");
  }                       

  vTaskDelete(AppTaskCreate_Handle);
  taskEXIT_CRITICAL(); // 退出临界区
}

static void LED_Task(void* pvParameters)
{
  while(1)
  {
    LED1_ON();
    vTaskDelay(1000);
    LED1_OFF();
    vTaskDelay(1000);
  }
}

static void RAM_Task(void* pvParameters)
{
  uint32_t g_memsize; // 内存大小
  while(1)
  {
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      if(Test_Ptr == NULL) // 内存未申请
      {
        g_memsize = xPortGetFreeHeapSize(); // 获取空闲堆内存大小
        printf("系统当前内存大小为: %d 字节, 申请内存...\r\n", g_memsize);
        Test_Ptr = pvPortMalloc(1024); // 申请1024字节内存
        if(Test_Ptr != NULL)
        {
          printf("内存申请成功。\r\n");
          printf("申请到的内存地址为%#x\n",(int)Test_Ptr);
          g_memsize = xPortGetFreeHeapSize(); // 获取空闲堆内存大小
          printf("系统当前内存大小为(申请完后): %d 字节。\r\n", g_memsize);
          // 向Test_Ptr写入数据：当前系统时间
          sprintf((char*)Test_Ptr, "当前系统时间TickCount: %d\n", xTaskGetTickCount());
          printf("写入数据成功。\r\n");
          printf("数据内容为：%s\r\n", (char*)Test_Ptr);
        }
      }
      else
      {
        printf("按下KEY2释放内存。\r\n");
      }
    }
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      if(Test_Ptr != NULL)
      {
        printf("释放内存...\r\n");
        vPortFree(Test_Ptr); // 释放内存
        Test_Ptr = NULL;
        printf("内存释放成功。\r\n");
        g_memsize = xPortGetFreeHeapSize(); // 获取空闲堆内存大小
        printf("系统当前内存大小为(释放后): %d 字节。\r\n", g_memsize);
      }
      else
      {
        printf("按下KEY1申请内存。\r\n");
      }
    }
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
