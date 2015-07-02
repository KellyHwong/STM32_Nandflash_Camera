#ifndef AIRCRAFT_H_
#define AIRCRAFT_H_

#ifdef __cplusplus
 extern "C" {
#endif 
 
void Aircraft_Init(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);
void TIM5_IRQHandler(void);
void TIM8_UP_TIM13_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif
