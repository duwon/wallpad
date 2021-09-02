/** 
 ******************************************************************************
  * @file    user.c
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
#include "iwdg.h"
#include "message.h"

uint16_t ltdcBuffer[130560] = {    0,}; 	/* LCD 버퍼 */


uint8_t  User_10ms_Count=0;
uint16_t  An_Delay_Timer=0;					// 딜레이 전용 타이머
uint16_t  User_Timer_0 = 0;					// 485 타이머
uint16_t  User_Timer_1 = 0;					// 터치키 사용
uint16_t  User_Timer_2 = 0;					// 1초 간격 할일 
uint16_t  User_Timer_3 = 0;
uint32_t  Timer_100ms  = 0;
uint32_t  Timer_1sec   = 0;

/**----------------------------------------------------------------------------
  * @brief  타이머 인터럽트
  * @retval None
  */
void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	//  prescaler 25-1, period=500-1
	if (htim->Instance == TIM6) 	/* 8000Hz 				*/
		{
		soundTimerCallback(htim);
		}
	
	
	// 10 msec  prescaler 100-1,   period=10000-1
	// 1  sec   prescaler 10000-1, period=10000-1
	if (htim->Instance == TIM7) 	// 2Hz    1 sec				
		{
		if (++User_10ms_Count > 9) 
			{
			User_10ms_Count = 0;
			if ((++Timer_100ms % 10) == 0) ++Timer_1sec;
			if (Timer_100ms > 0xfffff000) while (1); 			// 한바귀 돌면 리셋  12년 소요
			}
		}
	
	//  prescaler 100-1, period=2000-1
	if (htim->Instance == TIM10) 	// 1kHz   1 msec
		{
		if (An_Delay_Timer) An_Delay_Timer --;		// 딜레이 전용 타이머
		if (User_Timer_0) User_Timer_0 --;			
		if (User_Timer_1) User_Timer_1 --;
		if (User_Timer_2) User_Timer_2 --;
		if (User_Timer_3) User_Timer_3 --;			
		}
}
/**----------------------------------------------------------------------------
  * @brief  사용자 시작 초기화
  * @retval None
  */
void userStart(void)
{
	HAL_TIM_Base_Start_IT(&htim7); 	/* timer7 start - 2Hz */
	HAL_TIM_Base_Start_IT(&htim10); /* timer7 start - 2Hz */

	touchInit();
	soundInit();
	QSPI_EnableMemoryMapped(); /* QSPI Flash을 메모리 맵 방식으로 사용 설정 */
	LCD_Init();
	LCD_SelectLayer(0);
	LCD_LayerInit(0, (uint32_t)&ltdcBuffer);
}
/**----------------------------------------------------------------------------
  * @brief  사용자 계속 사용
  * @retval None
  */
void userWhile(void)
{
	

	#ifdef AGENCY_KOCOM
		Agency_Kocom ();
	#endif


}
//----------------------------------------------------------------------------
void Set_Relay (char flag)
{
	if (flag == _ON)
		{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_SET);
		An_Delay(40);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_0, GPIO_PIN_RESET);
		}
	else
		{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_SET);
		An_Delay(40);
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_1, GPIO_PIN_RESET);
		}
}
//----------------------------------------------------------------------------
void Set_Rtc_Time (int8_t Y, int8_t M, int8_t D, int8_t h, int8_t m, int8_t s)
{
RTC_TimeTypeDef  sTime;
RTC_DateTypeDef  sDate;

	sDate.Year = Y;
	sDate.Month = M;
	sDate.Date  = D;
	sTime.Hours = h;
	sTime.Minutes = m;
	sTime.Seconds = s;

	HAL_RTC_SetTime(&hrtc, &sTime, FORMAT_BIN);
	HAL_RTC_SetDate(&hrtc, &sDate, FORMAT_BIN);	
}
//--------------------------------------------------------------------------		
void Led_All_Off(void)
{
	LED_Off(LED1);
	LED_Off(LED2);
	LED_Off(LED3);
	LED_Off(LED4);
	LED_Off(LED5);
}
//--------------------------------------------------------------------------		
// HAL_Delay 는 동작하는 동안 시계가 가지 않으므로 1msec 틱을 활용함
void An_Delay (uint16_t msec)
{
	An_Delay_Timer = msec;
	while (An_Delay_Timer) HAL_IWDG_Refresh(&hiwdg);
}


//-------------------------------------------------------------------------------
// 리틀엔디안으로 저장되어 있음
uint32_t Get_Address (uint32_t Base, uint16_t no, uint32_t *sz)
{
union { uint32_t l; uint8_t c[4]; } q; 
uint8_t *p, bb[1000];
uint32_t addr;

	p = (uint8_t *)Base;
	p += (no<<3);					// ID 당 8 바이트 (위치+사이즈)
	p --;							// 해당 번지를 읽으면 +1 된 번지의 값을 일어오므로 1을 빼줘야 됨  (하드웨어 오동작)

	//------------------------------ 사이즈 
	memcpy (q.c, p, 4);
	if (q.l == 0xffffffff) return 0;
	if (q.l == 0 || q.l > 500000) q.l = 0;		// 500K 이상
	*sz = q.l;

	//------------------------------ 위치 (Base+)
	memcpy (q.c, p+4, 4);
	addr = q.l;
	if (addr < 0x2000 || addr > 0xa00000) addr = 0;
	
//sprintf (bb, "no=%d  ADDR=%x sz=%d  %d  %c%c", no, Base + addr, *sz, q.l, 0x0d, 0x0a);
//Send_232 (bb);
	
	return (Base + addr);
}


