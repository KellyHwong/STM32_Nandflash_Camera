/**
* @Filename: PID.cpp
* @Author: Kelly Hwong
* @Update: 2015.6.27
* @Description: 跨平台通用类，PID反馈控制器
*/
#ifndef PID_H_
#define PID_H_

class PID() {
 public:
  PID();
 private:
  unsigned char enabled_; // PID模块使能
  // 调度设置
  unsigned int interval_unit_; // 调度时间单位，in mS
  unsigned int interval_ticks_; // 调度间隔，in ticks
  unsigned int interval_counter_; // 调度计数
  // 控制设置
  float *ptr_con_var_; // 控制量接口
  float Kp_;
  float Ki_;
  float Kd_;
  // 反馈设置
  float setpoint_;
  float *ptr_measured_; // 测量接口
  float pre_e_; //上一个误差
  float e_; // 误差
  float d_; // 误差的差分
  float i_; // 误差的积分
}

#endif
