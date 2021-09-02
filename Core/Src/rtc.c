/**
  ******************************************************************************
  * File Name          : RTC.c
  * Description        : This file provides code for the configuration
  *                      of the RTC instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rtc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

RTC_HandleTypeDef hrtc;

/* RTC init function */
void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /** Initialize RTC Only 
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 128-1;
  hrtc.Init.SynchPrediv = 256-1;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */
  if(HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR0) == 0xAAAA)
  	{
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);					
    HAL_RTC_WaitForSynchro(&hrtc);
    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);
    return;														// 0xaaaa 이면 초기화 과정을 생략한다.
  	}
  else
  	{
    HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR0, 0xAAAA);				// dR0~31 까지 있음
  	}
  
  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date 
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enable the Alarm A 
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

}

void HAL_RTC_MspInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspInit 0 */

  /* USER CODE END RTC_MspInit 0 */
    /* RTC clock enable */
    __HAL_RCC_RTC_ENABLE();
  /* USER CODE BEGIN RTC_MspInit 1 */

  /* USER CODE END RTC_MspInit 1 */
  }
}

void HAL_RTC_MspDeInit(RTC_HandleTypeDef* rtcHandle)
{

  if(rtcHandle->Instance==RTC)
  {
  /* USER CODE BEGIN RTC_MspDeInit 0 */

  /* USER CODE END RTC_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_RTC_DISABLE();
  /* USER CODE BEGIN RTC_MspDeInit 1 */

  /* USER CODE END RTC_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

// DR0 ~ DR31 까지 사용
// 4바이트 단위로 최대 31개 까지 저장한다.   DR0 는 초기화 해지 로 사용중
void HAL_RTC_Write_Config (uint8_t *Buff)
{
uint32_t *p;

	p = (uint32_t *)Buff;

	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR1, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR2, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR3, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR4, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR5, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR6, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR7, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR8, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR9, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR10, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR11, *p++);
	HAL_RTCEx_BKUPWrite(&hrtc,RTC_BKP_DR12, *p++);
}

void HAL_RTC_Read_Config (uint8_t *Buff)
{
uint32_t *p;

	p = (uint32_t *)Buff;

	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR1);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR2);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR3);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR4);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR5);			
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR6);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR7);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR8);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR9);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR10);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR11);
	*p++ = HAL_RTCEx_BKUPRead(&hrtc,RTC_BKP_DR12);
}

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
