#include "Utilities/Receiver.h"

Receiver::Receiver() {}

void Receiver::update_data(float duty1) {
  stick_throttle_.get_now_duty(duty1);
}
