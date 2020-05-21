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
    - 2020-05-18    :    UART3 RX Interrupt 방식으로 변경, 사운드 재생시 레벨 조절 기능 추가
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

void changeLCDImage(void);

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
  LCD_Init();
  LCD_SelectLayer(0);
  LCD_LayerInit(0, (uint32_t)&ltdcBuffer);

  LCD_SetBackImage(0x90000000);
  playSound(0x90200000, 20720, 5);
}

/**
  * @brief  사용자 계속 사용
  * @retval None
  */
void userWhile(void)
{
static uint8_t lcdno=0;
static uint16_t timer=0, tch=0;
uint32_t arrImgAddr[6] = {0x90000000, 0x90050000, 0x900A0000, 0x90100000, 0x90150000, 0x901A0000};
uint8_t ret, Buff[100];

RTC_TimeTypeDef  sTime;
RTC_DateTypeDef  sDate;

	sDate.Year = 20;
	sDate.Month = 5;
	sDate.Date  = 19;
	sTime.Hours = 17;
	sTime.Minutes = 00;
	sTime.Seconds = 00;

//	HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
//	HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BIN);

	printf("\r\nSTART\r\n");

//	__HL_IWDG_START (&hiwdg);


	while (1)
  {
			
//		_HAL_IWDG_RELOAD_COUNTER(&hiwdg);
		
		if (++timer > 100)
    {
			HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
			HAL_RTC_GetDate(&hrtc, &sDate, FORMAT_BIN);
			printf ("%d/%d/%d %d:%d:%d%c%c", sDate.Year,sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds, 0x0d, 0x0a);
			timer = 0;
    }
			
				
		HAL_Delay(1);

		if (tch == 0)
  {
//			Send_Data ("01234567890\x0d\x0a", 12);
			ret = getTouchValue();
			if (ret)
    {
				Led_Display (ret);
				tch = 500;
  }
    }
		else
			{
			tch --;		// 500 msec 
  }

		ret = Read_Data (Buff);
		if (ret) 
			{
			HAL_Delay(50);	
			Send_Data (Buff, ret);
			}
		}
}


void Led_Display(int v)
{
static uint8_t l[5] = {0,0,0,0,0};
uint8_t no;

	if (v & 0x01) no = 1;
	if (v & 0x02) no = 2;
	if (v & 0x04) no = 3;
	if (v & 0x08) no = 4;
	if (v & 0x10) no = 5;								

	Led_All_Off ();
	
	switch (no)
{
		case 5:  
			LED_On (LED5);
			LCD_SetBackImage(0x90000000);
			playSound(0x90200000, 20720, 3);
			break;
		case 4:  
			LED_On (LED4);
			Set_Relay (0);
			LCD_SetBackImage(0x90050000);
			playSound(0x90200000, 20720, 3);
			break;
		case 3:  
			LED_On (LED3);
			LCD_SetBackImage(0x900A0000);
			playSound(0x90200000, 20720, 3);
			break;
		case 2:  
			LED_On (LED2);
			LCD_SetBackImage(0x90100000);
			playSound(0x90200000, 20720, 3);
//			LCD_Sleep (1);
			HAL_Delay(1000);
//			LCD_ErasePicture(100,100,100,100);
//			LCD_DisplayString (100,100,"abcd123");
//			LCD_DisplayChar (100,0,'7');
			break;
		case 1:  
			LED_On (LED1);
			LCD_SetBackImage(0x90150000);
			playSound(0x90200000, 20720, 3);
//			LCD_Sleep (0);

			HAL_Delay(1000);
			LCD_DrawPicture(0,0,0x90000000,480,272,0);
			LCD_DrawPicture(0,0,0x90050000,480,272,0);
			LCD_DrawPicture(0,0,0x900a0000,480,272,0);
			LCD_DrawPicture(0,0,0x90100000,480,272,0);
			LCD_DrawPicture(0,0,0x901a0000,480,272,0);

			break;
		}

					


//  LCD_DrawPicture(100,100,0x90050000,200,100,0);
//  LCD_ErasePicture(100,100,200,100);
}

void Led_All_Off ()
{
	LED_Off (LED1);	
	LED_Off (LED2);	
	LED_Off (LED3);	
	LED_Off (LED4);	
	LED_Off (LED5);		
}
void Set_Relay (char flag)
	{
static int8_t sw=0;

	if (sw == 0) 
		{ sw = 1; flag = 1; }
	else	
		{ sw = 0; flag = 0; }
				
	if (!flag)
		{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_SET);
		HAL_Delay(40);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
		}
	else
		{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
		HAL_Delay(40);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
	}
 
}
