#include <stdint.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "misc.h"
#include "Drivers/Pin.h"

Pin::Pin(PinTypedef pin) {
  this->eval_properties(pin); // 计算各个成员的值
  // 创造了它就要对它负责，让它可以启动
  RCC_AHB1PeriphClockCmd(this->RCC_Periph_GPIO_, ENABLE); // PORTA 时能
  this->GPIO_AF_TIM_ = 0xFF; // 不复用
}

// PWM输入通道模式，参数：复用设置
void Pin::mode_pwm_input(uint8_t GPIO_AF_TIM) {
  GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化器
  GPIO_InitStructure.GPIO_Pin = this->GPIO_Pin_; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; // 下拉
  GPIO_Init(this->GPIO_, &GPIO_InitStructure);
  GPIO_PinAFConfig(this->GPIO_, this->GPIO_PinSource_, GPIO_AF_TIM); // PA0复用TIM5
}

// PWM输出通道模式，参数：复用设置
void Pin::mode_pwm_output(uint8_t GPIO_AF_TIM) {
  GPIO_InitTypeDef GPIO_InitStructure; // GPIO初始化器
  GPIO_InitStructure.GPIO_Pin = this->GPIO_Pin_; //
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; // 下拉
  GPIO_Init(this->GPIO_, &GPIO_InitStructure);
  GPIO_PinAFConfig(this->GPIO_, this->GPIO_PinSource_, GPIO_AF_TIM); // PA0复用TIM5
}

// 根据PXX计算属性
void Pin::eval_properties(PinTypedef pin) {
  this->GPIO_PinSource_ = pin % 16;
  this->GPIO_No_ = pin / 16; // 0 for A, 1 for B, etc
  this->GPIO_Pin_ = (1 << this->GPIO_PinSource_);
  // 计算端口地址
  switch (this->GPIO_No_) {
    case (0): { this->GPIO_ = GPIOA; break; }
    case (1): { this->GPIO_ = GPIOB; break; }
    case (2): { this->GPIO_ = GPIOC; break; }
    case (3): { this->GPIO_ = GPIOD; break; }
    case (4): { this->GPIO_ = GPIOE; break; }
    case (5): { this->GPIO_ = GPIOF; break; }
    case (6): { this->GPIO_ = GPIOG; break; }
    case (7): { this->GPIO_ = GPIOH; break; }
    case (8): { this->GPIO_ = GPIOI; break; }
  }
  // 计算RCC地址
  switch (this->GPIO_No_) {
    case (0): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOA; break; }
    case (1): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOB; break; }
    case (2): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOC; break; }
    case (3): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOD; break; }
    case (4): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOE; break; }
    case (5): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOF; break; }
    case (6): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOG; break; }
    case (7): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOH; break; }
    case (8): {this->RCC_Periph_GPIO_ = RCC_AHB1Periph_GPIOI; break; }
  }
}
