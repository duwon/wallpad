/**
  ******************************************************************************
  * @file    sound.c
  * @author  정두원
  * @date    2020-04-10
  * @brief   사운드 재생
  * @details
  1. 사용 방법:
  --------------------------
    + 초기화
      o soundInit() 함수로 초기화
    + 사용방법
      o playSound() 함수 사용 
  */

#include <stdio.h>
#include <stdbool.h>
#include "sound.h"
#include "tim.h"
#include "dac.h"
#include "flash.h"

bool flagPlaySound = false;
uint32_t sampleRateIndex = 0;
uint32_t sampleRateMax = 0;
uint8_t* playSoundAddr;

/** @defgroup 생활정보기_SOUND 소리 재생 함수
  * @{
  */
 
/**
  * @brief  사운드 초기화 함수. 인터럽트 시작 및 DAC 설정
  * @retval None
  */

void soundInit(void)
{
  DAC_ChannelConfTypeDef sConfig;
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2);
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_8B_R, 0xFF);
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);

  HAL_TIM_Base_Start_IT(&htim6);
}

/**
  * @brief  사운드 재생
  * @param  soundAddr: 사운드 메모리 시작 주소
  * @param  soundLen: 샘플레이트 * 시간(초)
  * @retval None
  */
void playSound(uint8_t* soundAddr, uint32_t soundLen)
{
  playSoundAddr = soundAddr;
  sampleRateMax = soundLen;
  flagPlaySound = true;
  HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}

/**
  * @brief  사운드 재생을 위한 8000Hz 인터럽트
  * @param  htim: 타이머 인터럽트
  * @retval None
  */
void soundTimerCallback(TIM_HandleTypeDef *htim)
{
  if (flagPlaySound)
  {
    HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_8B_R, playSoundAddr[sampleRateIndex++]);
    if (sampleRateIndex == sampleRateMax)
    {
      sampleRateIndex = 0;
      flagPlaySound = false;
      HAL_DAC_Stop(&hdac, DAC_CHANNEL_2);
    }
  }
}

/**
  * @}
  */

