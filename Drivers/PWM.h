#ifndef PWM_H_
#define PWM_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include "stm32f4xx_tim.h"
#include "stm32f4xx_gpio.h"
#include "misc.h"

#include "Drivers/Timer.h"
#include "Drivers/Pin.h"
#include "Misc/Formatter.h"
#include "Utilities/LCD.h"

class PWM {
 private:
  float duty_;
  uint32_t freq_;
  Timer tim_;
  uint8_t CH_No_;
  PinTypedef pin_;
 public:
  PWM();
  PWM(uint32_t freq, float duty, uint8_t TIM_No, uint8_t CH_No,PinTypedef pin);
  void set_duty(float duty);
  float get_duty(void);
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
