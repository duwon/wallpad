#ifndef SOUND_H__
#define SOUND_H__ 1

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"

typedef enum
{
    OFF,
    ON
}muteTypeDef;

void soundInit(void); /* 초기화 함수 */
void playSound(uint32_t soundAddr, uint32_t soundLen, uint8_t soundLevel); /* Flash에 위치한 소리 재생 */
void soundTimerCallback(TIM_HandleTypeDef *htim); /* 소리 재생 인터럽트 */


#ifdef __cplusplus
}
#endif

#endif /* SOUND_H__ */
