#ifndef USER_H__
#define USER_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

static const uint16_t *IMG_01 = ((uint16_t *)(0x90000000));
static const uint16_t *IMG_02 = ((uint16_t *)(0x90050000));
static const uint16_t *IMG_03 = ((uint16_t *)(0x900A0000));
static const uint16_t *IMG_04 = ((uint16_t *)(0x90100000));
static const uint16_t *IMG_05 = ((uint16_t *)(0x90150000));
static const uint16_t *IMG_06 = ((uint16_t *)(0x901A0000));

void userStart(void);
void userWhile(void);

const static uint8_t *WAVE = ((uint8_t *)(0x90200000));

extern uint16_t ltdcBuffer[130560];



void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);




#ifdef __cplusplus
}
#endif


#endif
