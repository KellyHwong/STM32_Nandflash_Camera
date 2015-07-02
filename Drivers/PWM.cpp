#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Drivers/PWM.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"
PWM::PWM() {
}
PWM::PWM(uint32_t freq, float duty, uint8_t TIM_No, uint8_t CH_No,PinTypedef pin) {
  tim_ = Timer(TIM_No);
  freq_ = freq;
  duty_ = duty;
  CH_No_ = CH_No;
  pin_ = pin;
  tim_.mode_pwm_output(freq, duty, CH_No, pin);
}

void PWM::set_duty(float duty) {
  duty_ = duty;
  tim_.set_duty(duty, CH_No_);
}

float PWM::get_duty(void) {
  return duty_;
}
