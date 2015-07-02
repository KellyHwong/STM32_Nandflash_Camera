/**
* @Filename:
* @Author: Kelly Hwong
* @Update: 2015.7.1
* @Description: stm32定时器类
*/
#ifndef TIMER_H_
#define TIMER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"
#include "Drivers/Pin.h"

class Timer {
 private:
  // Datas
  uint8_t TIM_No;
  
  uint32_t RCC_Periph_TIM; // RCC
  IRQn_Type IRQn; // NVIC, eg: TIM5_IRQn
  float freq_;
  // Funcs
  uint8_t GPIO_AF_TIM(TIM_TypeDef * TIM);
  void config_IRQn(void);
  void config_Periph(void);
  void config_TimeBase(void);
 public:
  // Datas
  //void (Timer::*pFnIRQHandler)(Timer * tim); // IRQHandler
  // PWM Input params
  TIM_TypeDef * TIM; // eg: TIM5
  float DutyCycle;
  float Frequency;
  // Funcs
  // eg: Timer(TIM5, MODE_PWM_INPUT);
  Timer();
  Timer(uint8_t TIM_No);
  void mode_pwm_input(PinTypedef p);
  void PWM_Input_Handler(void);
  void mode_pwm_output(float freq, float duty, uint8_t CH_No, PinTypedef p);
  void mode_sch(void);
  void set_duty(float duty, uint8_t CH_No);
  //static void PWM_Input_Handler_Dispatch(Timer * tim);
  //void set_IRQHandler(void (Timer::*pFunction)(Timer * tim));
}; // class Timer

#ifdef __cplusplus
}
#endif

#endif
