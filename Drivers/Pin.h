/**
* @Filename:
* @Author: Kelly Hwong
* @Update: 2015.7.1
* @Description: stm32引脚类
*/
#ifndef PIN_H_
#define PIN_H_

#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "misc.h"

typedef enum {
  PA0 = 0,
  PA1 = 1,
  PA2 = 2,
  PA3 = 3,
  PA4 = 4,
  PA5 = 5,
  PA6 = 6,
  PA7 = 7,
  PA8 = 8,
  PA9 = 9,
  PA10 = 10,
  PA11 = 11,
  PA12 = 12,
  PA13 = 13,
  PA14 = 14,
  PA15 = 15,
  PB0,
  PB1,
  PB2,
  PB3,
  PB4,
  PB5,
  PB6,
  PB7,
  PB8,
  PB9,
  PB10,
  PB11,
  PB12,
  PB13,
  PB14,
  PB15,
  PC0,
  PC1,
  PC2,
  PC3,
  PC4,
  PC5,
  PC6,
  PC7,
  PC8,
  PC9,
  PC10,
  PC11,
  PC12,
  PC13,
  PC14,
  PC15,
  PD0,
  PD1,
  PD2,
  PD3,
  PD4,
  PD5,
  PD6,
  PD7,
  PD8,
  PD9,
  PD10,
  PD11,
  PD12,
  PD13,
  PD14,
  PD15,
  PE0,
  PE1,
  PE2,
  PE3,
  PE4,
  PE5,
  PE6,
  PE7,
  PE8,
  PE9,
  PE10,
  PE11,
  PE12,
  PE13,
  PE14,
  PE15,
  PF0,
  PF1,
  PF2,
  PF3,
  PF4,
  PF5,
  PF6,
  PF7,
  PF8,
  PF9,
  PF10,
  PF11,
  PF12,
  PF13,
  PF14,
  PF15,
  PG0,
  PG1,
  PG2,
  PG3,
  PG4,
  PG5,
  PG6,
  PG7,
  PG8,
  PG9,
  PG10,
  PG11,
  PG12,
  PG13,
  PG14,
  PG15,
  PH0,
  PH1,
  PH2,
  PH3,
  PH4,
  PH5,
  PH6,
  PH7,
  PH8,
  PH9,
  PH10,
  PH11,
  PH12,
  PH13,
  PH14,
  PH15,
  PI0,
  PI1,
  PI2,
  PI3,
  PI4,
  PI5,
  PI6,
  PI7,
  PI8,
  PI9,
  PI10,
  PI11,
  PI12,
  PI13,
  PI14,
  PI15
} PinTypedef;

class Pin {
 private:
  // Data
  PinTypedef pin;
  uint16_t GPIO_Pin_; //eg: GPIO_Pin_1
  GPIO_TypeDef * GPIO_; // eg: GPIOA
  uint8_t GPIO_No_;
  uint8_t GPIO_PinSource_; // eg: GPIO_PinSource1，范围1~15，故也可以当编号用
  uint8_t GPIO_AF_TIM_; //eg: GPIO_AF_TIM5
  uint32_t RCC_Periph_GPIO_; //eg: RCC_AHB1Periph_GPIOA
  // Funcs
  void eval_properties(PinTypedef pin);

 public:
  Pin(PinTypedef pin);
  void mode_pwm_input(uint8_t GPIO_AF_TIM);
  void mode_pwm_output(uint8_t GPIO_AF_TIM);
}; // class Pin

#endif
