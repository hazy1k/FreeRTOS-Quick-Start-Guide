/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
/* 其他头文件 */
#include "uart.h"
#include "led.h"
#include "key.h"
#include "limits.h"

/* 任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL; // 创建任务句柄

static TaskHandle_t Receive1_Task_Handle = NULL; // 接收任务1句柄
static TaskHandle_t Receive2_Task_Handle = NULL; // 接收任务2句柄
static TaskHandle_t Send_Task_Handle = NULL; // 发送任务句柄

/* 内核对象句柄 */
/* 全局变量声明 */
/* 宏定义 */
#define USE_CHAR 0 // 测试字符串的时候配置为1 测试变量的时候配置为0

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
    printf("这是一个FreeRTOS任务通知代替消息队列的测试程序\r\n");
    printf("按下KEY1或者KEY2向任务发送消息通知\r\n");
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
  BaseType_t xReturn = pdTRUE;
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while(1)
  {
    //获取任务通知 ,没获取到则一直等待
    xReturn = xTaskNotifyWait(0x0,     // 进入函数的时候不清除任务 bit
                              ULONG_MAX, // 退出函数的时候清除所有的 bit 
    #if USE_CHAR
                              (uint32_t *)&r_char, // 保存任务通知值
    #else
                              &r_num, // 保存任务通知值
    #endif
                              portMAX_DELAY); // 阻塞时间
    if(xReturn == pdTRUE)
    {
    #if USE_CHAR
      printf("Receive1_Task 任务通知为 %s \n",r_char);
    #else
      printf("Receive1_Task 任务通知为 %d \n",r_num);
    #endif
    }
    LED1_TOGGLE(); 
  }
}

// 接收任务2函数
static void Receive2_Task(void* pvParameters)
{
  BaseType_t xReturn = pdTRUE;
#if USE_CHAR
  char *r_char;
#else
  uint32_t r_num;
#endif
  while(1)
  {
    //获取任务通知 ,没获取到则一直等待
    xReturn = xTaskNotifyWait(0x0,     // 进入函数的时候不清除任务 bit
                              ULONG_MAX, // 退出函数的时候清除所有的 bit 
    #if USE_CHAR
                              (uint32_t *)&r_char, // 保存任务通知值
    #else
                              &r_num, // 保存任务通知值
    #endif
                              portMAX_DELAY); // 阻塞时间
    if(xReturn == pdTRUE)
    {
    #if USE_CHAR
      printf("Receive2_Task 任务通知为 %s \n",r_char);
    #else
      printf("Receive2_Task 任务通知为 %d \n",r_num);
    #endif
    }
    LED2_TOGGLE(); 
  }
}

// 发送任务函数
static void Send_Task(void* pvParameters)
{
  BaseType_t xReturn = pdPASS;
#if USE_CHAR
  char test_str1[] = "This is a test 1";
  char test_str2[] = "This is a test 2";
#else
  uint32_t test_num1 = 100;
  uint32_t test_num2 = 200;
#endif
  while(1)
  {
    /* KEY1被按下，向接收任务1发送通知 */
    if(Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotify(Receive1_Task_Handle, // 通知接收任务1
      #if USE_CHAR
                            (uint32_t)&test_str1, // 通知值
      #else
                            test_num1, // 通知值
      #endif 
                            eSetValueWithOverwrite); // 覆盖当前通知
      if(xReturn == pdPASS)
      {
        printf("Receive1_Task 任务通知释放成功!\r\n");
      }                     
    }
    /* KEY2被按下，向接收任务2发送通知 */
    if(Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
    {
      xReturn = xTaskNotify(Receive2_Task_Handle, // 通知接收任务2
      #if USE_CHAR
                            (uint32_t)&test_str2, // 通知值
      #else
                            test_num2, // 通知值
      #endif 
                            eSetValueWithOverwrite); // 覆盖当前通知
      if(xReturn == pdPASS)
      {
        printf("Receive2_Task 任务通知释放成功!\r\n");
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
