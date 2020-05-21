#ifndef USER_H__
#define USER_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

void userStart(void);
void userWhile(void);

const static uint8_t *WAVE = ((uint8_t *)(0x90200000));

extern uint16_t ltdcBuffer[130560];
extern RTC_HandleTypeDef hrtc;


void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);




#ifdef __cplusplus
}
#endif


#endif
