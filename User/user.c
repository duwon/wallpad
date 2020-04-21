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
    - 작성일      :   2020-04-19
  * @section  MODIFYINFO      수정정보
    - 2020-04-19    :    문서화
  
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
#include "flash.h"
#include "message.h"

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
    if(leftTime == 8000)
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
  FlashInit();
  messageInit();

  playSound((uint8_t *)wave, sizeof(wave));

  for (int i = 0; i < 130560; i++) /* 색 변경 도트 출력 설정 */
  {
    ltdcBuffer[i] = (uint16_t)i;
  }
  LCD_Init();
  LCD_SelectLayer(0);
  LCD_LayerInit(0, (uint32_t)&ltdcBuffer);

  LCD_DisplayNumPicture(100, 100, 1);
  LCD_DisplayNumPicture(120, 100, 2);
  LCD_DisplayNumPicture(140, 100, 10);
  LCD_DisplayNumPicture(160, 100, 3);
  LCD_DisplayNumPicture(180, 100, 4);
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

  HAL_Delay(1000);
  Flash_readImage((uint8_t *)ltdcBuffer, 10);
  HAL_Delay(1000);
  Flash_readImage((uint8_t *)ltdcBuffer, 11);
  HAL_Delay(1000);
  Flash_readImage((uint8_t *)ltdcBuffer, 12);
	HAL_Delay(1000);
	Flash_readImage((uint8_t *)ltdcBuffer, 13);
}
