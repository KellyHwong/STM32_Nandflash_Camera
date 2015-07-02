/**
* @Filename: PID.cpp
* @Author: Kelly Hwong
* @Update: 2015.6.27
* @Description: 跨平台通用类，PID反馈控制器
*/
#include "PID.h"

PID::PID(addr_controlled_var, Kp, Ki, Kd, ) {
  this.ptr_controlled_var_ = addr_controlled_var;
  this.Kp_ = Kp;
  this.Ki_ = Ki;
  this.Kd_ = kd;

}

// PID控制例程
void PID::control(void) {
  e_ = setpoint_ - (*ptr_measured);
  i_ += e_ * interval_unit_ / 1000;
  d_ = (e_-pre_e_) / interval_unit_ * 1000;
  pre_e_ = e_;
  (*ptr_con_var_) = setpoint_ + Kp_*e_ + Ki_*i_ + Kd*d_;
}
