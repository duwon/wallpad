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
#include "qspi.h"

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
  LED_On(LED1);
  LED_On(LED2);
  LED_On(LED3);
  LED_On(LED4);
  LED_On(LED5);

  touchInit();
  soundInit();
  //QSPI_Init();
  //testFunction();

  //messageInit();

  //playSound((uint8_t *)wave, sizeof(wave));

  //for (int i = 0; i < 130560; i++) /* 색 변경 도트 출력 설정 */
  //{
  //  ltdcBuffer[i] = (uint16_t)i;
  //}
  //memset(ltdcBuffer,0xff,sizeof(ltdcBuffer));
  LCD_Init();
  LCD_SelectLayer(0);
  LCD_LayerInit(0, (uint32_t)&ltdcBuffer);

  LCD_DisplayNumPicture(100, 100, 1);
  LCD_DisplayNumPicture(120, 100, 2);
  LCD_DisplayNumPicture(140, 100, 10);
  LCD_DisplayNumPicture(160, 100, 3);
  LCD_DisplayNumPicture(180, 100, 4);

  loadImage((uint8_t *)ltdcBuffer);

  //QSPI_Erase_Block(0x10000);
  ////QSPI_Write((uint8_t*)&ltdcBuffer,0x10000,0x10000);
  ////for (int index = 0; index < 0x200; index++)
  ////{
  ////    printf("%x ", ltdcBuffer[index]);
  ////}
	//printf("\r\n");
  //
  //QSPI_Read((uint8_t*)&ltdcBuffer,0,256);
	//for (int index = 0; index < 0x200; index++)
  //{
  //    printf("%c", ltdcBuffer[index]);
  //}
  //for (int index = 0; index < 0x200; index++)
  //{
  //    printf("%x ", ltdcBuffer[index]);
  //}
}

/**
  * @brief  사용자 계속 사용
  * @retval None
  */
void userWhile(void)
{
  uint8_t touchValue = getTouchValue();
  //printf("touch 0x%x\r\n", touchValue);
  for (int i = 0; i < 4; i++)
  {
    if (((touchValue >> i) & 0x01) == 1)
    {
      LED_Toggle((Led_TypeDef)i);
    }
  }

  HAL_Delay(500);
}
