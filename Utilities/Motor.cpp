#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"
#include "Utilities/Motor.h"

Motor::Motor() {}

Motor::Motor(uint32_t freq, float duty, uint8_t TIM_No, uint8_t CH_No,PinTypedef pin) {
  pwm_ = PWM(freq, duty, TIM_No, CH_No, pin);
}

// 输入PWM占空比限幅
float Motor::limit_duty(float duty) {
  if (duty < MIN_DUTY)
    duty = MIN_DUTY;
  if (duty > MAX_DUTY)
    duty = MAX_DUTY;
  return duty;
}

void Motor::set_duty(float duty) {
  duty_ = limit_duty(duty);
  pwm_.set_duty(duty_);
}

float Motor::get_duty(void) {
  return duty_;
}
