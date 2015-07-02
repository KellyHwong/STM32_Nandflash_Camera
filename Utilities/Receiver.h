#ifndef RECEIVER_H_
#define RECEIVER_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "Utilities/Stick.h"

class Receiver {
 private:

 public:
  Stick stick_throttle_; // 油门
  Stick stick_pitch_;
  Stick stick_roll_; // 油门
  Stick stick_yaw_; // 油门
  
  Receiver();
  void update_data(float duty1);
}; // class Stick

#ifdef __cplusplus
}
#endif

#endif
