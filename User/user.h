#ifndef USER_H__
#define USER_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------- Select AGENCY 
//#define AGENCY_HYUNDAI
//#define AGENCY_CVNET
//#define AGENCY_SDS
//#define AGENCY_KOCOM
//#define AGENCY_COMMAX
//#define AGENCY_ICONTROLS
#define AGENCY_KOCOM




#define		UART_SPEED			9600




#include "main.h"

extern RTC_HandleTypeDef hrtc;
extern RTC_TimeTypeDef  sTime;
extern RTC_DateTypeDef  sDate;
extern uint16_t ltdcBuffer[130560];


extern uint16_t An_Delay_Timer;					// 딜레이 전용 타이머
extern uint8_t  User_10ms_Count;
extern uint16_t User_Timer_0;
extern uint16_t User_Timer_1;
extern uint16_t User_Timer_2;
extern uint16_t User_Timer_3;
extern uint32_t  Timer_100ms;
extern uint32_t  Timer_1sec;

void user_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void userStart(void);
void userWhile(void);

void Set_Rtc_Time (int8_t Y, int8_t M, int8_t D, int8_t h, int8_t m, int8_t s);
void Set_Relay (char flag);
void Agency_Kocom ();
void An_Delay (uint16_t msec);
uint32_t Get_Address (uint32_t Base, uint16_t no, uint32_t *sz);


#define		Flash_Image_Base	0x90000000

#define		_ON				1
#define		_OFF			0





#ifdef __cplusplus
}
#endif


#endif
