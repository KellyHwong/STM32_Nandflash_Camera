#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"

#define TIMER_CLOCK 1000000.0
#define PERIOD_1_MS 1000
#define PERIOD_0_5_MS 500
// assert(系统时钟为168MHz)
#define PRESCALER (84-1) //
// 挂载在APB1上，APB1默认分频4
// TIM2~7输入时钟为SYS/APB1*2（APB1!=1时）
// 系统时钟168MHz时，TIM2~7时钟频率84MHz（168/4*2）
// 计数预分频84，则计数频率1MHz，1uS计一次

// 挂载在APB2上，APB2默认分频2
#define PRESCALER_APB2 (168-1)
// 168/2*2 = 168

#define PERIOD (0xFFFFFFFF) // 32/16位，兼容各个计时器
Timer::Timer() {
}

Timer::Timer(uint8_t TIM_No) {
  this->TIM_No = TIM_No;
  switch (TIM_No) {
    case (1): { this->TIM = TIM1; break; }
    case (2): { this->TIM = TIM2; break; }
    case (3): { this->TIM = TIM3; break; }
    case (4): { this->TIM = TIM4; break; }
    case (5): { this->TIM = TIM5; break; }
    case (6): { this->TIM = TIM6; break; }
    case (7): { this->TIM = TIM7; break; }
    case (8): { this->TIM = TIM8; break; }
    case (9): { this->TIM = TIM9; break; }
    case (10): { this->TIM = TIM10; break; }
    case (11): { this->TIM = TIM11; break; }
    case (12): { this->TIM = TIM12; break; }
    case (13): { this->TIM = TIM13; break; }
    case (14): { this->TIM = TIM14; break; }
  };
  this->config_IRQn();
  this->config_Periph();
  //this->pFnIRQHandler = 0;
  this->Frequency = 0;
  this->DutyCycle = 0;
}

// 计时器设置为PWM捕获器，此时只能捕获1路
// eg: TIM5, CH2, 捕获PA1
void Timer::mode_pwm_input(PinTypedef p) {
  Pin pin(p); // 函数调用后变量就消失了
  uint8_t tmp = this->GPIO_AF_TIM(this->TIM);
  // 看看tmp有没有问题
  pin.mode_pwm_input(tmp);

  NVIC_InitTypeDef NVIC_InitStructure;  // 中断初始化器
  NVIC_InitStructure.NVIC_IRQChannel = this->IRQn;
  // 低频高优先级原则，20mS的捕获中断设为最高优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ 通道使能
  NVIC_Init(&NVIC_InitStructure);

  // 时钟预分频
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue; // 计时器分频初始化器
  TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER;
  TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER;
  switch (this->TIM_No) {
    case (1):
    case (8):
    case (9):
    case (10):
    case (11): {
      TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER_APB2;
      break;
    }
  }
  TIM_TimeBaseStructrue.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructrue.TIM_Period = PERIOD;
  TIM_TimeBaseStructrue.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(this->TIM, &TIM_TimeBaseStructrue);

  // 设置TIM5 CH2 pin为外部信号输入
  // CCR2测频率
  // CCR1测占空比
  TIM_ICInitTypeDef TIM_ICInitStructure; // 计时器模块初始化器
  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2; // input channel 2
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising; // 上升沿中断
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; // map IC1 to TI1
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1; // 输入分频，不分频
  TIM_ICInitStructure.TIM_ICFilter = 0x00; //滤波设置，经历几个周期跳变认定波形稳定0x0～0xF

  TIM_PWMIConfig(this->TIM, &TIM_ICInitStructure);

  /* Select the TIM5 Input Trigger: TI2FP2 */
  TIM_SelectInputTrigger(this->TIM, TIM_TS_TI2FP2);

  TIM_SelectSlaveMode(this->TIM,TIM_SlaveMode_Reset);//TIM从模式：触发信号的上升沿重新初始化计数器和触发寄存器的更新事件
  TIM_SelectMasterSlaveMode(this->TIM,TIM_MasterSlaveMode_Enable); //启动定时器的被动触发

  TIM_Cmd(this->TIM, ENABLE); // run TIM5
  // 设置中断ISR为PWM Input模式中断
  // this->set_IRQHandler(Timer::PWM_Input_Handler_Dispatch);
  if ((TIM_No==1)||(TIM_No==8))
    TIM_ITConfig(this->TIM, TIM_IT_Update, ENABLE); // 打开中断，不要
  else
    TIM_ITConfig(this->TIM, TIM_IT_CC2, ENABLE); // 打开中断，TIM_IT_Update不要
}

// 计时器设置为PWM发生器，可以四路输出
void Timer::mode_pwm_output(float freq, float duty, uint8_t CH_No, PinTypedef p) {
  freq_ = freq;
  Pin pin(p); // 函数调用后变量就消失了
  uint8_t tmp = this->GPIO_AF_TIM(this->TIM);
  // 看看tmp有没有问题
  pin.mode_pwm_output(tmp);
  
  // 时钟预分频
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue; // 计时器分频初始化器
  uint16_t Period;
  uint16_t Width;
  Period = (uint16_t) (TIMER_CLOCK/freq);
  Width = (uint16_t) (Period*duty);
  TIM_TimeBaseStructrue.TIM_Period = Period - 1;      //PWM周期
  TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER;
  switch (this->TIM_No) {
    case (1):
    case (8):
    case (9):
    case (10):
    case (11): {
      TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER_APB2;
      break;
    }
  }
  TIM_TimeBaseStructrue.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructrue.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(this->TIM, &TIM_TimeBaseStructrue);

  /* Enable TIM4 Preload register on ARR */
  TIM_ARRPreloadConfig(this->TIM, ENABLE);
  
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;     //配置为PWM模式1
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Width; // Period*duty    //设置跳变值，当计数器计数到这个值时，电平发生跳变
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;  //当定时器计数值小于CCR1_Val时为高电平
  switch (CH_No) {
    case (1):{
      TIM_OC1Init(this->TIM, &TIM_OCInitStructure);
      TIM_OC1PreloadConfig(this->TIM, TIM_OCPreload_Enable);
      break;
    }
    case (2):{
      TIM_OC2Init(this->TIM, &TIM_OCInitStructure);
      TIM_OC2PreloadConfig(this->TIM, TIM_OCPreload_Enable);
      break;
    }
    case (3):{
      TIM_OC3Init(this->TIM, &TIM_OCInitStructure);
      TIM_OC3PreloadConfig(this->TIM, TIM_OCPreload_Enable);
      break;
    }
    case (4):{
      TIM_OC4Init(this->TIM, &TIM_OCInitStructure);
      TIM_OC4PreloadConfig(this->TIM, TIM_OCPreload_Enable);
      break;
    }
  }
  
  TIM_Cmd(this->TIM, ENABLE);                   //使能定时器
  /* TIM1 Main Output Enable */
  TIM_CtrlPWMOutputs(this->TIM, ENABLE);
}

// scheduler
void Timer::mode_sch(void) {
  NVIC_InitTypeDef NVIC_InitStructure;  // 中断初始化器
  NVIC_InitStructure.NVIC_IRQChannel = this->IRQn;
  // 低频高优先级原则，20mS的捕获中断设为最高优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 比PWM捕获器小一级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; // IRQ 通道使能
  NVIC_Init(&NVIC_InitStructure);
  
  // 时钟预分频
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructrue; // 计时器分频初始化器
  TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER;
  switch (this->TIM_No) {
    case (1):
    case (8):
    case (9):
    case (10):
    case (11): {
      TIM_TimeBaseStructrue.TIM_Prescaler = PRESCALER_APB2;
      break;
    }
  }
  TIM_TimeBaseStructrue.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructrue.TIM_Period = PERIOD_0_5_MS;
  TIM_TimeBaseStructrue.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseInit(this->TIM, &TIM_TimeBaseStructrue);
  
  TIM_ITConfig(this->TIM, TIM_IT_Update, ENABLE);
  TIM_Cmd(this->TIM, ENABLE);
}


void Timer::set_duty(float duty, uint8_t CH_No) {
  uint16_t C_Val = (uint16_t) (TIMER_CLOCK / (this->freq_) * duty);
  if (CH_No==1) TIM_SetCompare1(this->TIM,C_Val);    //设置占空比
  if (CH_No==2) TIM_SetCompare2(this->TIM,C_Val);
  if (CH_No==3) TIM_SetCompare3(this->TIM,C_Val);
  if (CH_No==4) TIM_SetCompare4(this->TIM,C_Val);
}

// 计算引脚复用地址
uint8_t Timer::GPIO_AF_TIM(TIM_TypeDef * TIM) {
  switch (this->TIM_No) {
    case (1): return GPIO_AF_TIM1;
    case (2): return GPIO_AF_TIM2;
    case (3): return GPIO_AF_TIM3;
    case (4): return GPIO_AF_TIM4;
    case (5): return GPIO_AF_TIM5;
    case (8): return GPIO_AF_TIM8;
    case (9): return GPIO_AF_TIM9;
    case (10): return GPIO_AF_TIM10;
    case (11): return GPIO_AF_TIM11;
    default: return 0xFF; // 错误输入，待处理
  }
}

// 配置IRQ通道
void Timer::config_IRQn(void) {
  switch (this->TIM_No) {
    // Warning!!! TIM1/6/8有多个IRQn，或者其IRQn不为TIMx_IRQn
    case (2): {this->IRQn = TIM2_IRQn; break;}
    case (3): {this->IRQn = TIM3_IRQn; break;}
    case (4): {this->IRQn = TIM4_IRQn; break;}
    case (5): {this->IRQn = TIM5_IRQn; break;}

    case (7): {this->IRQn = TIM7_IRQn; break;}

    //
    case (1): {this->IRQn = TIM1_CC_IRQn; break;}
    case (8): {this->IRQn = TIM8_UP_TIM13_IRQn; break; }
  }
}

// 配置外设
void Timer::config_Periph(void) {
  switch (this->TIM_No) {
    case (1): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM1; break; }
    case (2): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM2; break; }
    case (3): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM3; break; }
    case (4): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM4; break; }
    case (5): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM5; break; }
    case (6): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM6; break; }
    case (7): { this->RCC_Periph_TIM = RCC_APB1Periph_TIM7; break; }
    case (8): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM8; break; }
    case (9): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM9; break; }
    case (10): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM10; break; }
    case (11): { this->RCC_Periph_TIM = RCC_APB2Periph_TIM11; break; }
  }
  switch (this->TIM_No) {
    // TIM2~7在APB1上
    case (2):
    case (3):
    case (4):
    case (5):
    case (6):
    case (7): {
      RCC_APB1PeriphClockCmd(this->RCC_Periph_TIM, ENABLE); // TIM 使能
      break;
    }
    // TIM1/8/9/10/11在APB2上
    case (1):
    case (8):
    case (9):
    case (10):
    case (11): {
      RCC_APB2PeriphClockCmd(this->RCC_Periph_TIM, ENABLE); // TIM 使能
      break;
    }
  }
}

/* void Timer::set_IRQHandler(void (Timer::*pFunction)(Timer * tim)) {
  pFnIRQHandler = pFunction;
} */

// PWM Input模式的中断处理函数
void Timer::PWM_Input_Handler(void) {
  uint32_t IC2Value;
  TIM_ClearITPendingBit(TIM, TIM_IT_CC2); //
  IC2Value = TIM_GetCapture2(TIM);
  if (IC2Value!=0) {
    // DutyCycle= (TIM_GetCapture1(TIM) * 100.0) / IC2Value;
    DutyCycle= TIM_GetCapture1(TIM) * 1.0 / IC2Value; // 小数表示，而不是百分数！！！
    Frequency= TIMER_CLOCK / IC2Value;
  }
  else {
    DutyCycle= 0;
    Frequency= 0;
  }
}

/* void Timer::PWM_Input_Handler_Dispatch(Timer * tim) {
  tim->PWM_Input_Handler();
} */
