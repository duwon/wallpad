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
    - 작성일      :   2020-04-13
  * @section  MODIFYINFO      수정정보
    - 2020-04-19    :    문서화
    - 2020-04-21    :    Flash 제어 추가
    - 2020-04-29    :    QSPI Memory-map 방식 사용
    - 2020-05-13    :    LED1~5 순서 재정렬, 내부 클럭 사용 설정
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
#include "qspi.h"
#include "tim.h"
#include "message.h"

uint16_t ltdcBuffer[130560] = {
    0,
}; /* LCD 버퍼 */


/**
  * @brief  타이머 인터럽트
  * @retval None
  */
void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM6) /* 8000Hz */
  {
    soundTimerCallback(htim);
  }
  if (htim->Instance == TIM7) /* 2Hz */
  {
    LED_Toggle(LED5);
  }
}

/**
  * @brief  사용자 시작 초기화
  * @retval None
  */
void userStart(void)
{
  printf("\r\nSTART\r\n");
  HAL_TIM_Base_Start_IT(&htim7); /* timer7 start - 2Hz */
	
  // GPIO LED
  LED_On(LED1);
  LED_On(LED2);
  LED_On(LED3);
  LED_On(LED4);
  LED_On(LED5);
  
  touchInit();
  soundInit();
  QSPI_EnableMemoryMapped(); /* QSPI Flash을 메모리 맵 방식으로 사용 설정 */
  messageInit(); /* UART 인터럽트 시작 함수 호출 */
  
  playSound(0x90200000, 1440000); //playSound((uint32_t)&WAVE, 1440000);
  
  LCD_Init();
  LCD_SelectLayer(0);
  LCD_LayerInit(0, (uint32_t)&ltdcBuffer);

  LCD_SetBackImage(0x90000000);
}

/**
  * @brief  사용자 계속 사용
  * @retval None
  */
void userWhile(void)
{
  procMessage(); 
}
