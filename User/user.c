/** 
 ******************************************************************************
  * @file    user.c
  * @author  정두원
  * @date    2020-04-10
  * @brif    메인 구동 함수
  */

/**
  * @mainpage 생활정보기 
    @section intro 소개
    - 생활정보기
  * @section info 정보
    - 생활정보기 제어 API
  * @section  CREATEINFO      작성정보
    - 작성자      :   정두원
    - 작성일      :   2020-04-21
  * @section  MODIFYINFO      수정정보
    - 2020-04-19    :    문서화
    - 2020-04-21    :    Flash 제어 추가
    - 2020-04-22    :    Flash 삭제 및 QSPI 플래쉬 Memory MAP 사용으로 변경
  
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "user.h"
#include "touch.h"
#include "sound.h"
#include "lcd.h"
#include "led.h"
#include "message.h"

#include "IMG_01.h"
#include "IMG_02.h"
#include "IMG_03.h"
#include "IMG_04.h"
#include "IMG_05.h"
#include "IMG_06.h"

#include "SOUND_01.h"

uint16_t ltdcBuffer[130560] = {
    0,
}; /* LCD 버퍼 */

int leftTime = 0;
/**
  * @brief  타이머 인터럽트
  * @retval None
  */
void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6)
  {
    soundTimerCallback(htim);
    leftTime++;
    if (leftTime == 8000)
    {
      LED_Toggle(LED5);
      leftTime = 0;
    }
  }
  if (htim->Instance == TIM7)
  {
  }
}

/**
  * @brief  사용자 시작 초기화
  * @retval None
  */
void userStart(void)
{
  printf("\r\nSTART\r\n");
  // GPIO LED
  LED_Off(LED1);
  LED_Off(LED2);
  LED_Off(LED3);
  LED_Off(LED4);
  LED_Off(LED5);

  touchInit();
  soundInit();
  messageInit();

  playSound((uint8_t *)SOUND_01, sizeof(SOUND_01));

  LCD_Init();
  LCD_SelectLayer(0);
  LCD_LayerInit(0, (uint32_t)&ltdcBuffer);
}

/**
  * @brief  사용자 계속 사용
  * @retval None
  */
void userWhile(void)
{
  uint8_t touchValue = getTouchValue();
  printf("touch 0x%x\r\n", touchValue);
  for (int i = 0; i < 4; i++)
  {
    if (((touchValue >> i) & 0x01) == 1)
    {
      LED_Toggle((Led_TypeDef)i);
    }
  }

  uint32_t arrImgAddr[6] = {(uint32_t)&IMG_01, (uint32_t)&IMG_02, (uint32_t)&IMG_03, (uint32_t)&IMG_04, (uint32_t)&IMG_05, (uint32_t)&IMG_06};
  for (int i = 1; i < 6; i++)
  {
    LCD_SetBackImage(arrImgAddr[0]);
    HAL_Delay(2000);
    LCD_SetBackImage(arrImgAddr[i]);
    HAL_Delay(2000);
  }
}
