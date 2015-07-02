#include "Aircraft.h"
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "misc.h"
#include "stm32f4_discovery.h"

#include "Drivers/Timer.h"
#include "Misc/Formatter.h"
#include "Misc/Delay.h"
#include "Utilities/Motor.h"
#include "Utilities/Receiver.h"
#include "Utilities/Stick.h"
#define _DEBUG_WITH_BLUEBOOTH

#define _DEBUG_WITH_LCD

#ifdef _DEBUG_WITH_LCD
#include "Utilities/LCD.h"
#endif
#define DELAY_1S 40000000
#define PWM_FREQ 50
// 宏定义
// 信号接收定时器，引脚
#define THROTTLE_TIM 2
#define PITCH_TIM    3
#define YAW_TIM      4
#define ROLL_TIM     5
#define SCH_TIM      8
#define MOTOR1_PWM_TIM 9
#define MOTOR2_PWM_TIM 9
#define MOTOR3_PWM_TIM 10
#define MOTOR4_PWM_TIM 11
#define THROTTLE_PIN PB3
#define PITCH_PIN    PC7
#define YAW_PIN      PD13
#define ROLL_PIN     PA1

#define MOTOR1_PWM_CH  2
#define MOTOR1_PWM_PIN  PA3

#define MOTOR2_PWM_CH  1
#define MOTOR2_PWM_PIN  PA2

#define MOTOR3_PWM_CH  1
#define MOTOR3_PWM_PIN  PB8

#define MOTOR4_PWM_CH  1
#define MOTOR4_PWM_PIN  PB9

// 三个通道捕获计时器
Timer tim_throttle(THROTTLE_TIM); // ch3
Timer tim_pitch(PITCH_TIM); // ch1
Timer tim_yaw(YAW_TIM); // ch2
Timer tim_roll(ROLL_TIM);
// Motors
Motor motor1;
Motor motor2;
Motor motor3;
Motor motor4;
// 调度器
Timer tim_sch(SCH_TIM);
// 通道捕获器和解释器
Receiver receiver; 
//receiver.stick_pitch = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
//receiver.stick_roll = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门
//receiver.stick_yaw = Stick(5.020,9.999,0,NON_DIRECTIONAL_MINUS); // 油门

int main(void)
{
  Aircraft_Init();
  while (1)
  {
    Delay(DELAY_1S);
    GPIO_ToggleBits(LED4_GPIO_PORT, LED4_PIN);
	}
  return -1;
}

uint8_t init_flag = 0;
void Aircraft_Init(void) {
  GPIO_InitTypeDef  GPIO_InitStructure;
  // LCD
#ifdef _DEBUG_WITH_LCD
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_ResetBits(GPIOD , GPIO_Pin_7);    //CS=0;
  LCD_Initializtion();
  LCD_Clear(Blue);
#endif
  // 捕获器
  tim_throttle.mode_pwm_input(THROTTLE_PIN);
  tim_pitch.mode_pwm_input(PITCH_PIN);
  tim_yaw.mode_pwm_input(YAW_PIN);
  tim_roll.mode_pwm_input(ROLL_PIN);
  // 调度器
  tim_sch.mode_sch();
  // 接收机
  receiver.stick_throttle_ = Stick(0.0502,0.0999,0,NEGATIVE_LOGIC);
  // 油门最高点，听到确认音 
  motor1 = Motor(PWM_FREQ,MAX_DUTY,MOTOR1_PWM_TIM,MOTOR1_PWM_CH,MOTOR1_PWM_PIN);
  motor2 = Motor(PWM_FREQ,MAX_DUTY,MOTOR2_PWM_TIM,MOTOR2_PWM_CH,MOTOR2_PWM_PIN);
  motor3 = Motor(PWM_FREQ,MAX_DUTY,MOTOR3_PWM_TIM,MOTOR3_PWM_CH,MOTOR3_PWM_PIN);
  motor4 = Motor(PWM_FREQ,MAX_DUTY,MOTOR4_PWM_TIM,MOTOR4_PWM_CH,MOTOR4_PWM_PIN);
  // 电调接上电池，等待2S
  Delay(DELAY_1S);
  Delay(DELAY_1S);  
  
  // 以防万一，再次延迟2S
  // Delay(DELAY_1S);
  // Delay(DELAY_1S);
  // 油门推到最低，等待1S
  motor1.set_duty(MIN_DUTY);
  motor2.set_duty(MIN_DUTY);
  motor3.set_duty(MIN_DUTY);
  motor4.set_duty(MIN_DUTY);
  Delay(DELAY_1S);
  Delay(DELAY_1S);
  Delay(DELAY_1S);
  // 油门最低点确认音，可以起飞
  
  // LED4
  RCC_AHB1PeriphClockCmd(LED4_GPIO_CLK, ENABLE);
  GPIO_InitStructure.GPIO_Pin = LED4_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(LED4_GPIO_PORT, &GPIO_InitStructure);
  GPIO_ResetBits(LED4_GPIO_PORT,LED4_PIN);
  init_flag = 1;
}

// 调度器
uint32_t counter=0;
void TIM8_UP_TIM13_IRQHandler(void) {
  //
  TIM_ClearITPendingBit(tim_sch.TIM, TIM_IT_Update);
  //
  counter++;
  if ((counter%2000)==0) {
    //GPIO_ToggleBits(LED4_GPIO_PORT, LED4_PIN);
  }
  //
  receiver.update_data(tim_throttle.DutyCycle);
  if (init_flag) {
  motor1.set_duty(receiver.stick_throttle_.convert_duty_);
  motor2.set_duty(receiver.stick_throttle_.convert_duty_);
  motor3.set_duty(receiver.stick_throttle_.convert_duty_);
  motor4.set_duty(receiver.stick_throttle_.convert_duty_);
  }
}



// 接收机PWM占空比通过4个20mS中断读取
void TIM2_IRQHandler(void) {
  tim_throttle.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  uint8_t tmp3[80];
  float_to_string(tim_throttle.DutyCycle,tmp2);
  float_to_string(receiver.stick_throttle_.convert_duty_,tmp3);
  uint8_t tmp1[80] = "throttle:";
  GUI_Text(0,0,tmp1,White,Blue);
  GUI_Text(0,LCD_LINE_SPACE,tmp2,White,Blue);
  GUI_Text(0,2*LCD_LINE_SPACE,tmp3  ,White,Blue);
#endif
}
void TIM3_IRQHandler(void) {
  tim_pitch.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_pitch.DutyCycle,tmp2);
  uint8_t tmp1[80] = "pitch:";
  GUI_Text(0,2*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,3*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}
void TIM4_IRQHandler(void) {
  tim_yaw.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_yaw.DutyCycle,tmp2);
  uint8_t tmp1[80] = "yaw:";
  GUI_Text(0,4*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,5*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}
void TIM5_IRQHandler(void) {
  tim_roll.PWM_Input_Handler();
#ifdef _DEBUG_WITH_LCD
  uint8_t tmp2[80];
  float_to_string(tim_roll.DutyCycle,tmp2);
  uint8_t tmp1[80] = "roll:";
  GUI_Text(0,6*LCD_LINE_SPACE,tmp1,White,Blue);
  GUI_Text(0,7*LCD_LINE_SPACE,tmp2,White,Blue);
#endif
}  

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
