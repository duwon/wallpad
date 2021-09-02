/*

버튼
	if (현재상태 == 복귀)
		- 세대기로 외출요청 보내기
	else (외출상태면)
		- 세대기로 복귀요청 보내기

세대기 
	외출요청
		- 현재상태가 외출상태면 무시
		- 전등 Off, 엘베요쳥, 가스닫힘요청, 외출버튼 ON
		- 현재상태를 외출로
	복귀요청
		- 현재상태가 복귀이면 무시
		- 전등 on, 엘베 off, 외출버튼 off
		- 현재상태를 복귀로
			










외출 시 
	- 세대기로 외출설정을 보내고, 결과값을 받은 다음에  (전등 Off, 가스닫힘, 엘베요청) 처리,   결과값이 안오면 안 누른것처럼 아무동작 안함.
	- 세대기로 패킷 보낼때 보안설정까지 요청
	
복귀 시 
	- 해재 시 세대기로에 보안해지는 하지 않음 (0x00)
	- 전등 on, 엘베취소
	


외출버튼
	- 외출시 가스off, 일괄off, 엘리베이터 호출   (LED 는 7초간 점멸후  릴레이Off 후 On)
	- 귀가시 일괄on
조명
	- on  LED off,  조명off 그림
	- off LED on	조명on 그림

가스 - 열림상태 LED Off, 닫힘 LED On
	- 차단상태 -> 문구(가스밸구 잠겨있습니다)+삑1회, LED On
	- 열린상태 -> 문구(닫습니다)+음성알림, LED 점멸
	- 닫힘수신 -> 문구가스밸구 잠겼습니다)+삑2회, LED On

Elv
	- 호출 시 LED 점멸 (도착시 까지)
	- 도착 시 LED ON

부가기능 (에너지)
	- 			


터치      터치				브링크				외출/상태수신시
gas		touch_gas  		Blink_Gas		Set_LED

               

엘리비에트 그림은 없다가  호출하면 아이콘 표시 ?		
부가기능 공지/방붐자 그림 고정?     				화면만 넘어감
sleep 상태에서 터치하면 LED 전등, 가스 외출 이 이전상태 on/off 로 유지해야하는가
*/
/*

LCD_SetBackImage		:	전체화면그림 출력     		이미지는 RGB565 로 변환한것으로 사용해야함     바탕화면이후 내부 그림을 출력하면 지워짐
LCD_DrawPicture			:	부분화면 출력 (부투명그림)	이미지는 RGB565 로 변환한것으로 사용해야함
LCD_DrawNum				:	부분화면 출력 (투명그림)	이미지는 ARGB8888 로 변환한것으로 사용해야함	
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
#include "agency_kocom.h"
#include "stm32f7xx_hal.h"
#include "define_image.h"

RTC_TimeTypeDef  sTime;
RTC_DateTypeDef  sDate;


struct Config_Struct
	{
	int8_t	Idx[2];
	int8_t	Id;
	
	int8_t	Volume;
	int8_t	Out;
	int8_t	Light;
	int8_t	Gas_Valve;

	uint8_t Service_OGL;				// 서비스 외출=0x10, 가스=0x02, 전등=0x01
	uint8_t Service_PDE;				// 서비스 주차=0x04, 택배=0x02, 엘배=0x01
	uint8_t Energy_List;				// 지원 에너지 종수    전기=0x01, 수도=0x02, 가스=0x04, 온수=0x08, 난방=0x10
	
	int8_t	Weather;
	int8_t	Temp;
	
	uint8_t	Dummy[30];
	};

	
struct Status_Struct
	{
	uint8_t		Now_Screen;				// 현채 화면
	//------------------------------------------------------	
	uint8_t 	Lcd_Power;				// LCD 동작 상태
	uint32_t	Lcd_Sleep_100ms;		// 15초후 Sleep
	//------------------------------------------------------	
	uint8_t 	Light;					// 현재 상태 0=off, 1=on
	uint8_t   	Light_Blink_Count;		// 점멸횟수 시간 5초
	uint32_t	Light_100ms;			// 500mse 간격 점멸
	//------------------------------------------------------	
	uint8_t  	Elv_Pos1;				// Elv 위치	B, L
	uint8_t  	Elv_Pos2;				// Elv 위치
	uint8_t  	Elv_Way;				// Elv 방향
	uint8_t	  	Elv_Blink_Count;
	uint32_t   	Elv_100ms;
	//------------------------------------------------------	
	uint8_t 	Gas_Valve;				// 가스 밸브상태
	uint8_t	  	Gas_Blink_Count;
	uint32_t   	Gas_100ms;
	//------------------------------------------------------	
	uint8_t 	Out;					// 외출설정 상태
	
	//------------------------------------------------------	
	uint8_t   	Temp;					// 현재온도
	uint8_t   	Weather;				// 현재 시간날씨
	uint16_t   	Finedust_Value;			// 미세먼지 농도
	uint8_t   	Finedust_Stat;			// 미세먼지 상태
	int8_t		Parking[10];			// 주차위치
	//------------------------------------------------------	
	uint16_t	Power;					// 금월
	uint16_t	Power_1;				// 전월
	uint16_t	Power_2;				// 전전월
	uint16_t	Sudo;	
	uint16_t	Sudo_1;	
	uint16_t	Sudo_2;		
	uint16_t	Gas;	
	uint16_t	Gas_1;	
	uint16_t	Gas_2;		
	uint16_t	Onsu;	
	uint16_t	Onsu_1;	
	uint16_t	Onsu_2;		
	uint16_t	Warm;	
	uint16_t	Warm_1;	
	uint16_t	Warm_2;		
	//-----------------------------------------------------
	uint8_t		Delivery;
	};

struct Uart_Struct
	{
	uint8_t  Rx_Buffer [32];
	uint8_t  Tx_Buffer [32];
	uint8_t  Rx_Length;
	uint8_t  Ack_Wait_Count;
	uint8_t  Ack_Wait_Opcode;
	uint8_t  Ack_Wait_Device;
	uint32_t Ack_Wait_100ms;
	};

struct Event_Struct
	{
	int8_t	Out;	
	int8_t	Light;	
	int8_t	Gas;
	int8_t	Elv;
	};


struct Config_Struct	Cfg;
struct Status_Struct	Stat    = { _OFF, };
struct Uart_Struct 		Uart    = { _OFF, };
struct Event_Struct 	Evt     = { _OFF, };

uint16_t Version = 0;


int8_t msg[256];
//-------------------------------------------------------------------
void Agency_Kocom ()
{
int8_t cnt=0;
uint8_t Before_Time;
uint32_t Timer_1sec_Back=0xff;

//	Set_Rtc_Time (20,6,23,12,0,0);

	Init_Start ();
	Sleep_LCD (_ON);
	LCD_Home_View ();

	Led_onoff ();
	
	while (1)
		{
//		An_Delay (1);
		Uart_Proc ();
		
				
		if (Before_Time == User_10ms_Count) continue;
		Before_Time = User_10ms_Count;	

		Check_Touch ();							// 이전에 보내고 있는 패킷이 있으면 검사안함

		switch (User_10ms_Count)						// 각 case 문 마다  100 msec 간격
			{
			case 0 : //------------------------------------ Touch Check
//				if (Uart.Ack_Wait_Count) break;			// 순서대로 전송하므로 필요없을듯
//				Check_Touch ();							// 이전에 보내고 있는 패킷이 있으면 검사안함
				break;
			
			case 2 : //------------------------------------ Clock  
				HAL_IWDG_Refresh(&hiwdg);
				if (Timer_100ms % 10) break;			// 1초에 한번씩
				Display_Time (0);
				break;
			
			case 4 : //------------------------------------ Sleep Mode
				if (Stat.Lcd_Sleep_100ms > Timer_100ms) break;
				if (Stat.Light_Blink_Count || Stat.Gas_Blink_Count || Stat.Elv_Blink_Count) break;		// 브링크 중에는 슬립안함
#if 1
				Sleep_LCD (_OFF);
#endif				
				break;

			case 6 : //------------------------------------ LED/Image Blink
				Blink_Led_Image ();
				break;

			case 7 : //------------------------------------ Ack Wait  	
				if (Uart.Ack_Wait_Count==0) break;
				if (Uart.Ack_Wait_100ms > Timer_100ms) break;
//				sprintf (msg, "Ack_Wait=%d, Ack_Wait_100ms=%d", Uart.Ack_Wait_Count, Uart.Ack_Wait_100ms);
//				Send_232 (msg);
				
				if (--Uart.Ack_Wait_Count) 
					{
					Remake_Pkt ();
					Uart.Ack_Wait_100ms = Timer_100ms + Wait_Ack_100ms;
					Send_485 (Tx_Buff, Pkt_Size);
					Print_Pkt ("Tx", Tx_Buff);
					}
				break;
				
			case 8 :	
				if (Timer_1sec_Back == Timer_1sec) break;
				Timer_1sec_Back = Timer_1sec;
				if (Uart.Ack_Wait_Count) break;
				Todo_List ();
				break;
				
			case 9 :
				break; 	
			}
		}
}
//--------------------------------------------------------------------------				
void Init_Start ()
{
int8_t buf[128];
	
	HAL_RTC_Read_Config ((uint8_t *)&Cfg);			// 36 바이트 읽어옴
	
	if (memcmp (Cfg.Idx, "GR", 2) != 0)
		{
		Cfg.Idx[0] 		= 'G';
		Cfg.Idx[1] 		= 'R';
		Cfg.Id			= 77;
		Cfg.Volume      = 3;
		Cfg.Out			= Stat_Out_Goingout;		// 외출로 설정
		Cfg.Light       = Stat_Light_Off;
		Cfg.Gas_Valve   = Stat_Gas_Open;	
		Cfg.Weather		= 1;
		Cfg.Temp		= 0;
		
		Cfg.Service_OGL = 0x13;			// 외출, 가스, 전등 기능
		Cfg.Service_PDE = 0x07;			// 주차, 택배, 엘베 기능		
		Cfg.Energy_List = 0x07;			// 전기, 수도, 가스 
		
		HAL_RTC_Write_Config ((uint8_t *)&Cfg);		// 36 바이트 저장
		Send_232 ("Factory");
		}

	sprintf (buf, "Vol=%d, ID=%d, light=%x, Gas=%x", Cfg.Volume, Cfg.Id, Cfg.Light, Cfg.Gas_Valve);
	Send_232 (buf);
	
	
	Stat.Out   		= Cfg.Out;					// 이전상태 복원
	Stat.Light 		= Cfg.Light;
	Stat.Gas_Valve 	= Cfg.Gas_Valve;
	Stat.Weather	= Cfg.Weather;
	Stat.Temp		= Cfg.Temp;

	strcpy (Stat.Parking, "B1-0000\x00");
}
//------------------------------------------------------------------------
void Cfg_Write ()
{
	Cfg.Out		  = Stat.Out;		
	Cfg.Light     = Stat.Light;	
	Cfg.Gas_Valve = Stat.Gas_Valve;
	Cfg.Weather	  = Stat.Weather;	
	Cfg.Temp	  = Stat.Temp;
	
	HAL_RTC_Write_Config ((uint8_t *)&Cfg);		// 36 바이트 저장
}
/*------------------------------------------------------------------------
- 1초 간격  : 전송할 데이타 확인
- 1시간 간격 : 날씨정보 확인  (기동시 처음 50초후)
- 2시간 간격 : 에너지 사용량 조회 (기동 시 처음 1분후)
- 2시간 견격 : 미세먼지

------------------------------------------------------------------------*/
void Todo_List ()
{
static uint16_t tcnt=0;	

	if (++tcnt > 3600) tcnt = 0;			// 1hour

	switch (tcnt)
		{
		case 30 : //----------------------- 날씨정보  , 이후는 1시간 간격
			Send_Packet (CC_Ack_Request, OP_Weather, 0, 0,0,0,0,0,0,0);
			return;
		case 40 : //------------------------ 미세먼지 , 이후는 1시간 간격 
			Send_Packet (CC_Ack_Request, OP_Finedust, 0, 0,0,0,0,0,0,0);
			return;
		case 50 : //------------------------ 에너지 사량량, 이후는 1시간 간격	
			if (Enargy_Use)		Send_Packet (CC_Ack_Request, OP_Energy_Use, Energy_Power,0xff,0,0,0,0,0,0);		// 전기 평균/사용량 전체요청 
			return;
		case 60 : //------------------------ 에너지 사량량, 이후는 1시간 간격	
			if (Enargy_Use)		Send_Packet (CC_Ack_Request, OP_Energy_Use, Energy_Sudo,0xff,0,0,0,0,0,0);		// 수도 평균/사용량 전체요청 
			return;
		case 70 : //------------------------ 에너지 사량량, 이후는 1시간 간격	
			if (Enargy_Use)		Send_Packet (CC_Ack_Request, OP_Energy_Use, Energy_Gas,0xff,0,0,0,0,0,0);		// 가스 평균/사용량 전체요청 
			return;
		case 80 : //------------------------ 에너지 사량량, 이후는 1시간 간격	
			if (Enargy_Use)		Send_Packet (CC_Ack_Request, OP_Energy_Use, Energy_Onsu,0xff,0,0,0,0,0,0);		// 온수 평균/사용량 전체요청 
			return;
		case 90 : //------------------------ 에너지 사량량, 이후는 1시간 간격	
			if (Enargy_Use)		Send_Packet (CC_Ack_Request, OP_Energy_Use, Energy_Warm,0xff,0,0,0,0,0,0);		// 난방 평균/사용량 전체요청 
			return;

		case 100 : //-----------------------  에너지 종수 요청
			Send_Packet (CC_Ack_Request, OP_Enargy_List,  0,0,0,0,0,0,0,0);
//			Send_Packet (CC_Ack_Request, OP_Request_Park, 0,0,0,0,0,0,0,0);
			return;
		}

	if (Evt.Out)
		{
		Send_Packet (CC_Ack_Request, OP_Out, Evt.Out, 0, 0,0,0,0,0,0);				// 보안설정 해지
		Evt.Out = Stat_None;
		return;
		}
		
	if (Evt.Light)
		{
		Send_Packet (CC_Ack_Request, OP_Control, Device_Light, 0xff, Evt.Light, 0,0,0,0,0);
		Evt.Light = Stat_None;
		return;
		}
		
	if (Evt.Gas)
		{
		Send_Packet (CC_Ack_Request, OP_Control, Device_Gas, 0, Stat_Gas_Close, 0,0,0,0,0);	// D2 2=닫기, 1=열림,  0x02로 닫기요청
		Evt.Gas = Stat_None;
		return;
		}

	if (Evt.Elv)
		{
		Send_Packet (CC_Ack_Request, OP_Control, Device_Elv, 0,0,0,0,0,0,0);		// 호출
		Evt.Elv = Stat_None;
		return;
		}
}
//------------------------------------------------------------------------
void Led_onoff ()
{
		
	if (Stat.Out == Stat_Out_Goingout)									// 0x11=외출,  0x12=복귀
		LED_On (Touch_Out);	else LED_Off (Touch_Out);		

	if (Stat.Light == Stat_Light_On)							// 0x65=일괄소등,  0x66=일괄점등
		LED_Off (Touch_Light);	else LED_On (Touch_Light);

	if (Stat.Gas_Valve == Stat_Gas_Close)
		LED_On (Touch_Gas);	else LED_Off (Touch_Gas);			//2=닫힘, 1=열림
			
	LED_Off (Touch_Elv);
	LED_Off (Touch_Odd);
}
//--------------------------------------------------------------------------				
void Display_Elv_Floor ()
{
static uint8_t w=0, be=0;
int8_t i;

	if (Stat.Now_Screen != Touch_Elv || Stat.Lcd_Power != _ON) return;
	
	if (Stat.Elv_Way == 1 || Stat.Elv_Way == 2)	be = Stat.Elv_Way;	// 전 상태	
		
	if (++w >= 3) w = 0;
				
	for (i=0; i<2; i++)
		{
		if (Stat.Elv_Pos1 > 10)
			LCD_DrawNum (51,  94, Stat.Elv_Pos1,     Img_Main_Elv, 25,  43);		// L or B
		else	
			LCD_DrawNum (51,  94, Stat.Elv_Pos1+133, Img_Main_Elv, 25,  43);
		LCD_DrawNum (92,  94, Stat.Elv_Pos2+133, Img_Main_Elv, 25,  43);

		switch (Stat.Elv_Way)
			{
			case 0 :
				break;
			case 1 :
				LCD_DrawNum (363, 81, w+173, Img_Main_Elv, 53, 66);	    // ^
				break;
			case 2 :	
				LCD_DrawNum (363, 81, w+176, Img_Main_Elv, 53, 66);		// v
				break;
			case 3 :
				break;
			case 4 :		
				LCD_DrawNum(130,  232, 171, Img_Main_Elv, 234, 18);		// 도착
				if (be == 1)
					LCD_DrawNum(363,  81, 175, Img_Main_Elv, 53, 66);		// ^3개	방향 3개로 표시토록	
				else	
					LCD_DrawNum(363,  81, 178, Img_Main_Elv, 53, 66);		// v개   방향 3개로 표시토록	
				break;
			}
		An_Delay (20);
		}
}
//--------------------------------------------------------------------------				
void Sleep_LCD (int8_t flag)
{
	Stat.Lcd_Power = flag;
	Stat.Now_Screen  = Touch_Out;			// 초기화면

	if (flag == _OFF)
		{
		LED_Off (Touch_Odd);
		LCD_Power_Set (_OFF);
		LCD_Home_View ();
		Stat.Lcd_Sleep_100ms  = 0xffffff00;		// 도착 못할 시간
		Cfg_Write ();
		}
	else
		{
		LCD_Power_Set (_ON);
		Stat.Lcd_Sleep_100ms  = Timer_100ms + LCD_Sleep_100ms;
		}
}
// prealble(2)+HD(1)+CC(1)+PCNT(1)+목적지(2)+출발지(2)+Op(1)+data(8)+Fcc(1)+EOT(1)
//--------------------------------------------------------------------------				
void Uart_Proc ()
{
uint8_t ret, Ch, s1, s2;

	if (Read_485 (&Ch) == 0)
		{
		if (Rx_Len == 0) return; 		
		if (Uart_Rx_Timer) return;				// 수신한 데이타는 있는데 시간안에 데이타가 완성되지 않으면 무시
		goto _Err;
		}

	switch (Rx_Len)
		{
		case 0 : 
			if (Ch != Preamble_0) return;
			break;
		case 1 :
			if (Ch != Preamble_1) goto _Err;
			break;
		case 19 :
		case 20 :
			if (Ch != EOT) goto _Err;
			break;
		}
	
	Uart_Rx_Timer = 200;						// 타이머 재 설정	200 msec
	Rx_Buff[Rx_Len++] = Ch;

	if (Rx_Len < Pkt_Size) return;

//	An_DataPrint ("Rx", Rx_Buff, Rx_Len);
	Print_Pkt ("Rx", Rx_Buff);

	ret = Get_Checksum (Rx_Buff);


// prealble(2)+HD(1)+CC(1)+PCNT(1)+목적지(2)+출발지(2)+Op(1)+data(8)+Fcc(1)+EOT
	
	if (ret     != Rx_Buff[Pkt_FCC]) goto _Err;		// checksum 다름
	if (ISD     != Rx_Buff[Pkt_ADH]) goto _Err;		// 나한테 온게 아님
//	if (WALLPAD != Rx_Buff[Pkt_ASH]) goto _Err;		// 월패드가 아님
	
// prealble(2)+HD(1)+CC(1)+PCNT(1)+목적지(2)+출발지(2)+Op(1)+data(8)+Fcc(1)+EOT(1)


	switch (Rx_Buff[Pkt_CC] & 0xf0)
		{
		//------------------------------------------ 월패드에서 오는 명령을 바로 ACK 한다.
		case 0x90 :			// ACK 안함
		case 0xb0 :			// ACK 요구
			s1 = Rx_Buff[Pkt_ADH];
			s2 = Rx_Buff[Pkt_ADL];
			Rx_Buff[Pkt_ADH] = Rx_Buff[Pkt_ASH];
			Rx_Buff[Pkt_ADL] = Rx_Buff[Pkt_ASL];
			Rx_Buff[Pkt_ASH] = s1;
			Rx_Buff[Pkt_ASL] = s2;

			//   1001 1100		
			//	  --    ---- 전송회수  00,01,10  
			//	   +-------- 00=ack 없음,  01=ACK확인,  10=ACK 패킷
			if ((Rx_Buff[Pkt_CC] & 0xf0) == 0xb0)
				{
				Rx_Buff[Pkt_CC]  &= 0x0f;						// ACK 자리 마스크  (월패드의 전송카운터 값 유지)
				Rx_Buff[Pkt_CC]  |= 0xd0;						// ACK 표시
				Rx_Buff[Pkt_FCC] = Get_Checksum (Rx_Buff);
				An_Delay (20);

				Send_485 (Rx_Buff, Pkt_Size);					// 20msec 지연 후 ACK 전송
//				Print_Pkt ("Tx", Rx_Buff);
				}
			Parsing_Packet ();
			break;
		//------------------------------------------ 월패드에서 ACK 가 오면 ACK 요청한 패킷을 재전송 중지
		case 0xd0 :			// ACK 패킷
			if (Uart.Ack_Wait_Opcode == Rx_Buff[Pkt_OP]
				&& Uart.Ack_Wait_Device == Rx_Buff[Pkt_DATA])
				{
				Uart.Ack_Wait_Opcode = 0;
				Uart.Ack_Wait_Count = 0;							// 재전송 중지
				}
			break;
		}
	Rx_Len = 0;
	return;

_Err:
	Send_232 ("Rx Error");
	Rx_Len = 0;
}
/*------------------------------------------------------------------------------------
OP   						내용 			WP -> ISD		ISD -> WP	
------------------------------------------------------------------------------------
0x01 날씨									0				0
0x02 에너지사용랑 조회											0 
0x03 제어
	 OP_induc 2d		인덕션				0				0
	 OP_Gas   2c		가스밸브			0				0			WP, ISD 제어시 상태 알림/요청
	 OP_Light 0e		일괄소등			0				0			WP, ISD 제어시 상태 알림/요청
	 OP_Elv   44		엘리베이터호출						0
0x04 택배알림								0		
0x05 주차정보								0
0x50 주차장보 상세							0
0x06 외출/귀가								0				0
0x07 에너지 종 조회											0
0x08 미세먼지								0				0

0x3a 시간정보								0
0x4a 버젼정보								0
0x6a 구현기능정보요청											0
------------------------------------------------------------------------------------*/
void Parsing_Packet ()
{
static uint32_t Set_Rtc_1sec = 0;
uint8_t v;
	
	switch (Rx_Buff[Pkt_OP])
		{
		//--------------------------------------- 시간별 날씨
		case OP_Weather :
			Stat.Weather = Rx_Buff[Pkt_DATA+4];		// 날씨종류 1~7
			Stat.Temp  = Rx_Buff[Pkt_DATA];			// 온도 
			Display_Weather (Stat.Weather);
			Display_Temp (Stat.Temp);
			break;

		//--------------------------------------  에너지 사용량
		case OP_Energy_Use :
			Get_Energy_Data (&Rx_Buff[Pkt_DATA]);
			break;

		//--------------------------------------- 제어
		case OP_Control :
			switch (Rx_Buff[Pkt_DATA])
				{
				case Device_Gas   :
				case Device_Induc :
					Stat.Gas_Valve = Rx_Buff[Pkt_DATA+2];			// 0x01=열림, 0x02=닫힘
					Stat.Gas_Blink_Count = Stat.Gas_100ms = 1;		// Blink_Led_Image() 에서 처리토롤
					break;

				case Device_Light :
					v = Rx_Buff[Pkt_DATA+2];
					if (v != Stat_Light_Off && v != Stat_Light_On) break;	// on/off 가 아니면 무시
					if (Stat.Light == v)	break;					// 현재와 같으면 무시
					Touch_Parsing (0x08, 1);						// 세대기에서 온 전등제어를 터치버튼을 누른것 같이 동작하도록	
					break;

				case Device_Elv :
					Response_Elv (Rx_Buff[Pkt_DATA+2], &Rx_Buff[Pkt_DATA+3]);
					break;
				}
			break;					

		//--------------------------------------- 외출에 대한 월패드
		case OP_Out :
			v = Rx_Buff[Pkt_DATA];
			if (v != Stat_Out_Goingout && v != Stat_Out_Return) break;	// 외출/복귀가 아니면 무시
			
			if (v == Stat_Out_Goingout) 								// 외출요청 버튼을 눌러 세대기로 외출요청 후 세대기에서 외출 결과값을 받았거나,  세대기에서 외출요청이 오는경우
				{
				if (Stat.Out == Stat_Out_Goingout)  break;				// 현재상태와 같으면 무시
				Stat.Out = Stat_Out_Goingout;
				}
			else		// 세대기에서 복귀요청이 온경우
				{
				if (Stat.Out == Stat_Out_Return) break;
				Stat.Out = Stat_Out_Return;	
				}
				
			Set_Out ();			
			break;

		//--------------------------------------  에너지 종류
		case OP_Enargy_List :
			if (Rx_Buff[Pkt_DATA]   == 0x00) Cfg.Energy_List &= 0xfe; else Cfg.Energy_List |= 0x01;  // 전기
			if (Rx_Buff[Pkt_DATA+1] == 0x00) Cfg.Energy_List &= 0xfd; else Cfg.Energy_List |= 0x02;	 // 수도
			if (Rx_Buff[Pkt_DATA+2] == 0x00) Cfg.Energy_List &= 0xfb; else Cfg.Energy_List |= 0x04;  // 가스		 
			if (Rx_Buff[Pkt_DATA+3] == 0x00) Cfg.Energy_List &= 0xf7; else Cfg.Energy_List |= 0x08;  // 온수
			if (Rx_Buff[Pkt_DATA+4] == 0x00) Cfg.Energy_List &= 0xef; else Cfg.Energy_List |= 0x10;  // 난방
			break;

		//--------------------------------------  미세먼지
		case OP_Finedust :
			Stat.Finedust_Stat = Rx_Buff[Pkt_DATA+2];
			if (Rx_Buff[Pkt_DATA] == 0xff) 
				Stat.Finedust_Value = 0xffff;
			else	
				Stat.Finedust_Value = (Rx_Buff[Pkt_DATA] * 100) + Rx_Buff[Pkt_DATA+1];
			break;

		//--------------------------------------- 시간설정
		case OP_Time :
			if (Set_Rtc_1sec > Timer_1sec) break;
			Set_Rtc_1sec = Timer_1sec + 1200;				// 20  분 간격으로 저장  (초단위로 저장하면 리셋됨)
			Send_232 ("Set RTC");
			Set_Rtc_Time (Rx_Buff[Pkt_DATA], Rx_Buff[Pkt_DATA+1], Rx_Buff[Pkt_DATA+2], Rx_Buff[Pkt_DATA+3], Rx_Buff[Pkt_DATA+4], Rx_Buff[Pkt_DATA+5]);
			break;
		//--------------------------------------- 버젼조회
		case OP_Version :
//			Version = (Rx_Buff[Pkt_DATA] << 8) + Rx_Buff[Pkt_DATA+1];
			Send_Packet (CC_Ack_Request, 0, Version_H, Version_L, 0,0,0,0,0,0);
			break;
		//--------------------------------------- 기능조회
		case OP_ServiceList :
			Send_Packet (CC_Ack_Request, 0, Cfg.Service_OGL, 0x03, Cfg.Service_PDE, 0,0,0,0,0);
			break;

		//--------------------------------------  택배
		case OP_Delivery :
			if (Rx_Buff[Pkt_DATA+1] == 1)
				{
				Stat.Delivery = _ON;
				Display_Delivery ();
				}
			break;
					
		//--------------------------------------- 주차위치
		case OP_Request_Park :
			if (Rx_Buff[Pkt_DATA] != 0x01) break;		// 첫번째 주차정보만 수용
			memcpy (Stat.Parking, &Rx_Buff[Pkt_DATA+1], 7);
			break;
					
		//--------------------------------------- 주차위치 상세
		case OP_Park_Detail :
			if (Rx_Buff[Pkt_DATA] != 0x15) break;		// 첫번째 차량의 차량위치만 수용
			memcpy (Stat.Parking, &Rx_Buff[Pkt_DATA+1], 7);
			break;
		}
}
//--------------------------------------------------------------------------
// 사용량만 취한다.  (평균사용량 X)
// 소수점 무시, 1000 으로 나눈값, 예제에는 전전월이 안옴
void Get_Energy_Data (uint8_t *p)
{
uint32_t val, v1,v2,v3;

	v1 = *(p+3);
	v2 = *(p+4);
	v3 = *(p+5);
	val = (v1*10000) + (v2 * 100) + v3;
	val %= 1000;

	if (*(p+1) != 0x01) return;		// 사용량만 취한다.
	
	switch (*p)
		{
		case 0x01 : 	// 전기
			switch (*(p+2))	
				{
				case 0x01 :	Stat.Power	 = (uint16_t)val;	break;		// 금월
				case 0x02 :	Stat.Power_1 = (uint16_t)val;	break;		// 전월
				case 0x03 :	Stat.Power_2 = (uint16_t)val;	break;		// 전전월		
				}
			break;		

		case 0x02 : 	// 수도
			switch (*(p+2))	
				{
				case 0x01 :	Stat.Sudo	= (uint16_t)val;	break;		// 금월
				case 0x02 :	Stat.Sudo_1 = (uint16_t)val;	break;		// 전월
				case 0x03 :	Stat.Sudo_2 = (uint16_t)val;	break;		// 전전월		
				}
			break;		

		case 0x03 : 	// 가스	
			switch (*(p+2))	
				{
				case 0x01 :	Stat.Gas   = (uint16_t)val;	break;		// 금월
				case 0x02 :	Stat.Gas_1 = (uint16_t)val;	break;		// 전월
				case 0x03 :	Stat.Gas_2 = (uint16_t)val;	break;		// 전전월		
				}
			break;		

		case 0x04 : 	// 온도
			switch (*(p+2))	
				{
				case 0x01 :	Stat.Onsu   = (uint16_t)val;	break;		// 금월
				case 0x02 :	Stat.Onsu_1 = (uint16_t)val;	break;		// 전월
				case 0x03 :	Stat.Onsu_2 = (uint16_t)val;	break;		// 전전월		
				}
			break;		

		case 0x05 : 	// 난방
			switch (*(p+2))	
				{
				case 0x01 :	Stat.Warm   = (uint16_t)val;	break;		// 금월
				case 0x02 :	Stat.Warm_1 = (uint16_t)val;	break;		// 전월
				case 0x03 :	Stat.Warm_2 = (uint16_t)val;	break;		// 전전월		
				}
			break;		
		}
}
// cnt = 재전송횟수 최대 3회,
// cc = 0xbc 이면 ACK 요구,  0x9c 이면 ACK 요청없음
//--------------------------------------------------------------------------
void Send_Packet (uint8_t cc, uint8_t op, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7)
{
//   1001 1100		
//	  --    ---- 전송회수  00,01,10  
//	   +-------- 00=ack 없음,  01=ACK확인,  10=ACK 패킷

	sprintf ((char *)Tx_Buff, "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				Preamble_0, Preamble_1,
				0x30,								// HD
				cc,									// CC			전송카운터 0 (최대 2까지 증가)
				0x00,								// PCNT
				WALLPAD,							// Dest
				0x00,
				ISD,								// Source
				0x00,
				op,									// Cmd		
				d0, d1, d2, d3, d4, d5, d6, d7,		// Data
				0,									// Bcc
				EOT,EOT);

	Tx_Buff[18] = Get_Checksum (Tx_Buff);

	if (cc == CC_Ack_Request)						// ack 요구
		{
		Uart.Ack_Wait_Opcode= op;
		Uart.Ack_Wait_Device= d0;
		Uart.Ack_Wait_Count = Wait_Ack_Limit;
		Uart.Ack_Wait_100ms = Timer_100ms + Wait_Ack_100ms;
		}
	else	
		{
		Uart.Ack_Wait_Count = 0;
		}
	
	Send_485 (Tx_Buff, Pkt_Size);
//	An_DataPrint ("Tx", Tx_Buff, Pkt_Size);
	Print_Pkt ("Tx", Tx_Buff);
}
//----------------------------------------------------------
void Remake_Pkt ()
{
	Tx_Buff[3]  ++;							// 재전송 카운터 증가
	Tx_Buff[18] ++;							// BCC 증가
}	
	
/*
AA 55 30 DC 00 01 00 73 00  03  00 00 00 00 00 00 00 00 63 0D 0D 
AA 55 30 BC 00 01 00 73 00  03  00 00 00 00 00 00 00 00 63 0D 0D 

AA 55 30 DC 00 01 00 73 00 03 01 00 00 00 00 00 00 00 64 0D 0D 
AA 55 30 BC 00 01 00 73 00 03 01 00 00 00 00 00 00 00 64 0D 0D 

AA 55 30 BC 00 73 00 01 00 03 00 00 00 00 00 00 00 00 63 0D 0D 
*/

//--------------------------------------------------------------------------		
void Check_Touch ()
{		
static uint8_t value=0;
uint8_t ret;

	ret = Get_Touch ();	
	if (!ret && !value) return;

	if (ret)
		{
		if (value == 0) Tc_Stay_Timer = 10000;		// 10초 설정
		value = ret;
		ret = 0;
		if ((value & 0x01) && (Tc_Stay_Timer < 8500))	// 부가기능1.5초 경과시 볼륩조절
			{
			Tc_Stay_Timer = 10000;
			if (++Cfg.Volume > 5) Cfg.Volume = 0;
			playSound(250, Cfg.Volume);
			Send_232 ("Set Volume\x0d\x0a");
			Stat.Lcd_Sleep_100ms = Timer_100ms + LCD_Sleep_100ms;
			if (Enargy_Use == 0)
				{
				if (Stat.Now_Screen != Touch_Odd)
					{
					Stat.Now_Screen = Touch_Odd;
					}
				LCD_DrawPicture(0,   0, 	6,      480, 272, 0);
				LCD_DrawNum (210,  130, Cfg.Volume+48, 6, 52,  82);
				LCD_DrawNum (210,  130, Cfg.Volume+48, 6, 52,  82);
				}
			}
		}
	else
		{
		ret = value;								// 눌럿다 떼었을대 알림
		value = 0;
		}
	
	if (ret) 
		{
		Touch_Parsing (ret, 0);	
		}
}			
//--------------------------------------------------------------------------							
uint8_t Get_Checksum(uint8_t *p)				// 패킷 버퍼
{
uint8_t sum=0, i;

	p += 2;
	for (i=0; i<16; i++, p++) sum += *p;
	return (sum);
}
//--------------------------------------------------------------------------
void Blink_Led_Image ()
{
static int8_t Light_Flag=0;
static int8_t Gas_Flag=0;
static int8_t Elv_Flag=0;
	
	//-----------------------------------------------
	if (Stat.Light_Blink_Count)
		{
		if (Stat.Light_100ms < Timer_100ms)
			{
			if (--Stat.Light_Blink_Count)
				{
				Stat.Light_100ms = Timer_100ms + Blink_Interval;
				Light_Flag ^= 1;
				if (Light_Flag == 0)
					LED_Off (Touch_Light);
				else
					LED_On (Touch_Light);
				}
			else	
				{
				Light_Flag = 0;	
				if (Stat.Now_Screen == Touch_Light && Stat.Lcd_Power == _ON)
					LCD_DrawPicture(0, 0, Img_Main_Light_Off,  480, 272, 0);
				LED_On (Touch_Light);
				Set_Relay(_ON);					// light off
				}
			}
		}
	//-----------------------------------------------
	if (Stat.Gas_Blink_Count)
		{
		if (Stat.Gas_100ms < Timer_100ms)
			{
			if (--Stat.Gas_Blink_Count)
				{
				Stat.Gas_100ms = Timer_100ms + Blink_Interval;
				Gas_Flag ^= 1;
				if (Gas_Flag == 0)
					LED_Off (Touch_Gas);
				else
					LED_On (Touch_Gas);
	
				if (Gas_Flag == 0)
					LCD_Out (Touch_Gas, 190, 45,  182, Img_Main_Gas, 92, 123);			// 182 가스 잠그는 그림
				else
					LCD_Out (Touch_Gas, 190, 45,  183, Img_Main_Gas, 92, 123);			// 183 가스 잠긴 그림
				}
			else	
				{
				Gas_Flag = 0;
				LCD_Out (Touch_Gas, 190, 45,  182, Img_Main_Gas, 92, 123);			// 마지막 그림은 가스감그는 그림으로
				
				if (Stat.Gas_Valve == Stat_Gas_Close)								// 마지막에 잠겼다는 신호가 오면 켜고, 타임아웃이면 그대로
					{
					LCD_Out (Touch_Gas, 138, 222, 190, Img_Main_Gas, 205, 18);		// 190 잠겨있습니다.	 
					LED_On (Touch_Gas);
					}
				else	
					LED_Off(Touch_Gas);
				}
			}
		}

	//----------------------------------------------------------
	if (Stat.Elv_Blink_Count)
		{
		if (Stat.Elv_100ms < Timer_100ms)
			{
			if (--Stat.Elv_Blink_Count)
				{
				Stat.Elv_100ms = Timer_100ms + Blink_Interval;
//				static int8_t flag=0;
				Elv_Flag ^= 1;
				if (Elv_Flag == 0)
					LED_Off (Touch_Elv);
				else
					LED_On (Touch_Elv);
				}
			else	
				{
				Elv_Flag = 0;
				LED_On (Touch_Elv);
				}
			Display_Elv_Floor ();
			}
		}
}
//--------------------------------------------------------------------------
void LCD_Out (int Screen, int x, int y, int image_no, int back_image_no, int xsise, int ysize)
{
	if (Stat.Now_Screen != Screen || Stat.Lcd_Power != _ON) return;
	LCD_DrawNum (x, y, image_no, back_image_no, xsise, ysize);
}
/*--------------------------------------------------------------------------				
stat 1=세대기에서 외출/전등 제어명령이 오는 경우,  0=터치버튼을 누른 경우
--------------------------------------------------------------------------*/
void Touch_Parsing (uint8_t tc, uint16_t flag)
{
uint8_t no;
	
	if (flag == 0 && Stat.Lcd_Power == _OFF)
		{
		Sleep_LCD (_ON);
		LCD_Home_View ();						// 꺼져있다 아무 터치를 누르면 홈화면으로 복귀
		Stat.Delivery = _OFF;					// 확인시켰으면 없음으로
		return;
		}

	Stat.Lcd_Sleep_100ms = Timer_100ms + LCD_Sleep_100ms;

	if (tc & 0x01)		no = LED1;		// Touch_Odd
	if (tc & 0x02)		no = LED2;		// Touch_Elv
	if (tc & 0x04)		no = LED3;		// Touch_Gas	
	if (tc & 0x08)		no = LED4;		// Touch_Light
	if (tc & 0x10)		no = LED5;		// Touch_Out

	Stat.Now_Screen = no;
	
	LED_Off(Touch_Odd);
	LED_Off(Touch_Elv);
	
	switch (Stat.Now_Screen)
		{
		case Touch_Out ://--------------------------------------------------
			if (flag == 0)	playSound(250, Cfg.Volume);
			LCD_Home_View ();
			Stat.Delivery = _OFF;					// 확인시켰으면 없음으로
			
			if (Stat.Out == Stat_Out_Return)		// 현재상태가 복귀이면
				Evt.Out = Stat_Out_Goingout;	
			else
				Evt.Out = Stat_Out_Return;
			break;
			
		case Touch_Light ://--------------------------------------------------
			LCD_DrawPicture(0, 0, Img_Main_Light_On,  480, 272, 0);
//			LCD_SetBackImage (Img_Main_Light_On);

			if (Stat.Light_Blink_Count) return;		// 이미 소등 중이면 취소
			
			if (flag == 0) playSound(250, Cfg.Volume);
			if (Stat.Light == Stat_Light_Off)
				{
				Stat.Light = Stat_Light_On;	
				Stat.Light_Blink_Count	= 0;
				LED_Off(Touch_Light);
				Set_Relay(_OFF);					// Light ON
				}
			else
				{
				Stat.Light = Stat_Light_Off;		
				Stat.Light_Blink_Count 	= Light_Blink_Counter;
				Stat.Light_100ms = Timer_100ms + Blink_Interval;
				}
	
			Evt.Light = Stat.Light;
			break;
			
		case Touch_Gas : //--------------------------------------------------
			LCD_DrawPicture(0, 0, Img_Main_Gas, 480, 272, 0);
//			LCD_SetBackImage (Img_Main_Gas);
		
			if (Stat.Gas_Valve == Stat_Gas_Close)
				{
				LCD_DrawNum(190, 45,  182, Img_Main_Gas, 92, 123);			// 182 가스 잠김그림
				LCD_DrawNum(138, 222, 190, Img_Main_Gas, 205, 18);			// 190 잠겨있습니다.
				if (flag == 0) playSound(250, Cfg.Volume);

				Stat.Gas_Blink_Count = 0;
				LED_On(Touch_Gas);
				}
			else
				{
				LCD_DrawNum(190, 45,   182, Img_Main_Gas, 92, 123);			// 182 가스 잠그는 그림
				LCD_DrawNum(138, 222,  189, Img_Main_Gas, 205, 18);			// 189 차단 중 입니다.

				if (Stat.Gas_Blink_Count) return;
				if (flag == 0) playSound(252, Cfg.Volume);									// 밸브를 닫습니다.
				Stat.Gas_Blink_Count = Gas_Blink_Counter;
				Stat.Gas_100ms = Timer_100ms + Blink_Interval;			
				Evt.Gas = Stat_Gas_Close;									// 닫기요청 0x02		
				}
			break;
	
		case Touch_Elv : //--------------------------------------------------
//			if (Stat.Elv_Blink_Count == 0)									// 재호출 하도록 제거
				{
				Stat.Elv_Blink_Count= Elv_Blink_Counter;
				Stat.Elv_100ms 		= Timer_100ms + Blink_Interval;
				Stat.Elv_Way  		= 1;		// 상향
				Stat.Elv_Pos1 		= 0;
				Stat.Elv_Pos2 		= 1;		// 1층
				Evt.Elv 			= Device_Elv;	// 호출 0x44	
				if (flag == 0) playSound(250, Cfg.Volume);
				}
			
			LCD_DrawPicture(0, 0, Img_Main_Elv, 480, 272, 0);
//			LCD_SetBackImage (Img_Main_Elv);
			LCD_DrawNum(130, 232, 170,	Img_Main_Elv, 234,  18);		// 호출중입니다.
			Display_Elv_Floor ();	
			break;
		
		case Touch_Odd : //--------------------------------------------------
			LED_On(Touch_Odd);
			if (Enargy_Use)
				{
				LCD_DrawPicture(0,   0, 	8,      480, 272, 0);
				Display_Energy ();
				playSound(250, Cfg.Volume);
				Display_Energy ();
				}
			else
				{
				LCD_DrawPicture(0,   0, 	6,      480, 272, 0);	
				playSound(250, Cfg.Volume);
				LCD_DrawNum (210,  130, Cfg.Volume+48, 6, 52,  82);
				LCD_DrawNum (210,  130, Cfg.Volume+48, 6, 52,  82);
				}
			break;
		}	
}			
//--------------------------------------------------------------------------------------
void Set_Out ()
{
	if (Stat.Out == Stat_Out_Goingout)		// 외촐 요청이면
		{
		if (Stat.Light == Stat_Light_On)
			{
			Stat.Light = Stat_Light_Off;		
			Stat.Light_Blink_Count 	= Light_Blink_Counter;
			Stat.Light_100ms = Timer_100ms + Blink_Interval;
			}
	
		if (Stat.Gas_Valve == Stat_Gas_Open)
			{
			Stat.Gas_Blink_Count = Gas_Blink_Counter;
			Stat.Gas_100ms = Timer_100ms + Blink_Interval;			
			Evt.Gas = Stat_Gas_Close;						// 닫기요청 0x02		
			}

		Stat.Elv_Blink_Count= Elv_Blink_Counter;
		Stat.Elv_100ms 		= Timer_100ms + Blink_Interval;
		Stat.Elv_Way  		= 1;		// 상향
		Stat.Elv_Pos1 		= 0;
		Stat.Elv_Pos2 		= 1;		// 1층
		Evt.Elv 			= Device_Elv;	// 호출 0x44	
		
		LED_On (Touch_Out);
		LED_Off(Touch_Odd);				
		}
	else
		{
		LED_Off(Touch_Out);
		LED_Off(Touch_Odd);				
		LED_Off(Touch_Elv);						// 복귀는 엘리베이터 off

		Stat.Elv_Blink_Count = 0;				// 브링크 끝
			
		if (Stat.Light == Stat_Light_Off)		// 복귀시 조명이 off 면 바로 켜기
			{
			Stat.Light = Stat_Light_On;	
			Stat.Light_Blink_Count	= 0;
			LED_Off(Touch_Light);
			Set_Relay(_OFF);					// Light ON
			}	
		}

	Evt.Light = Stat.Light;						// 전등 상태 월패드로 보내기
}
//--------------------------------------------------------------------------------------
//   LB1 층의 경우 "LB1"
//   B2  층의 경우 "B2"		뒤자리는 0x00 으로 

	uint8_t  	Elv_Pos;				// Elv 위치
	uint8_t  	Elv_Way;				// Elv 방향
//--------------------------------------------------------------------------------------
void Response_Elv (uint8_t val, uint8_t *s)
{
int8_t i, cnt=0;
int8_t buf[100];
uint8_t B=0;
										
	Stat.Elv_Way = val;			//0=모름, 1=상향, 2=하향, 3=정지중, 4=도착	

	memset (buf, 0x00, 10);

	if (*s == 'L') B = 195;
	if (*s == 'B') B = 196;

	for (i=0; i<5; i++, s++)
		{
		if (*s == 0x00) break;
		if (*s >= '0' && *s <= '9') buf [cnt++] = *s;
		}

	if (B) 
		{
		Stat.Elv_Pos1 = B;
		Stat.Elv_Pos2 = atoi(buf) % 10;
		}
	else
		{
		Stat.Elv_Pos1 = atoi(buf) / 10;
		Stat.Elv_Pos2 = atoi(buf) % 10;
		}	

	if (Stat.Elv_Way == 4)			// 도착
		{
		playSound(251, Cfg.Volume);
		LED_On (Touch_Elv);
		Display_Elv_Floor ();
		Stat.Elv_Blink_Count = 0;
		}		

//uint8_t tmp[128];
//sprintf (tmp, "Way=%d, 1=%d, 2=%d", Stat.Elv_Way, Stat.Elv_Pos1, Stat.Elv_Pos2);
//Send_232 (tmp);
} 
//--------------------------------------------------------------------------------------
void LCD_Home_View ()
{
	LCD_DrawPicture(0, 0, Img_Main_Home, 480, 272, 0);
//	LCD_SetBackImage (Img_Main_Home);

	Display_Time (1);
	Display_Weather (Stat.Weather);
	Display_Temp (Stat.Temp);
	
	Display_Msg_Dust ();
	Display_Msg_Parking (Stat.Parking);
	HAL_IWDG_Refresh(&hiwdg);

	Display_Msg_Dust ();
	Display_Msg_Parking (Stat.Parking);
	HAL_IWDG_Refresh(&hiwdg);
	
	if (Stat.Delivery)
		LCD_DrawNum(11, 220, 36, Img_Main_Home, 151, 35);		// 택배
	else	
		LCD_DrawNum(11, 220, 35, Img_Main_Home, 151, 35);		// 택배없음
}
//--------------------------------------------------------------------------------------
void Display_Time (uint8_t flag)
{
static uint8_t m=0xff, h, M, D;
static int8_t cflag=0;

	if (Stat.Now_Screen != Touch_Out || Stat.Lcd_Power == _OFF) return;
	cflag ^= 1;
	LCD_DrawNum    (133, 13, Img_Clock_Colon+cflag, Img_Main_Home, 27, 82);
	LCD_DrawNum    (133, 13, Img_Clock_Colon+cflag, Img_Main_Home, 27, 82);

	HAL_RTC_GetTime(&hrtc, &sTime, FORMAT_BIN);
	if (flag == 0 && m == sTime.Minutes) return;
	m = sTime.Minutes;
	h = sTime.Hours;
	HAL_RTC_GetDate(&hrtc, &sDate, FORMAT_BIN);
//	printf("%d %d/%d/%d %d:%d:%d%c%c", Timer_100ms, sDate.Year, sDate.Month, sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds, 0x0d, 0x0a);

	LCD_DrawNum(29,  13, Img_Clock_0 + (h/10), Img_Main_Home, 52, 82);
	LCD_DrawNum(81,  13, Img_Clock_0 + (h%10), Img_Main_Home, 52, 82);
	LCD_DrawNum(133, 13, Img_Clock_Colon, 	   Img_Main_Home, 27, 82);
	LCD_DrawNum(156, 13, Img_Clock_0 + (m/10), Img_Main_Home, 52, 82);
	LCD_DrawNum(208, 13, Img_Clock_0 + (m%10), Img_Main_Home, 52, 82);

	M = sDate.Month;
	LCD_DrawNum(100,  100, Img_Date_0 + (M/10), Img_Main_Home, 20, 35);
	LCD_DrawNum(120,  100, Img_Date_0 + (M%10), Img_Main_Home, 20, 35);
	
	D = sDate.Date;
	LCD_DrawNum(187,  100, Img_Date_0 + (D/10), Img_Main_Home, 20, 35);
	LCD_DrawNum(207,  100, Img_Date_0 + (D%10), Img_Main_Home, 20, 35);
	HAL_IWDG_Refresh(&hiwdg);
}
//--------------------------------------------------------------------------------------
void Display_Energy ()
{
uint16_t y1=85;
uint16_t y2=146;
uint16_t  x, sz;
	
	LCD_DrawNum(45,   y1, Stat.Power/100+143,      Img_Main_Energy, 15, 25);
	LCD_DrawNum(60,   y1, (Stat.Power%100)/10+143, Img_Main_Energy, 15, 25);
	LCD_DrawNum(75,   y1, Stat.Power%10+143,       Img_Main_Energy, 15, 25);

	LCD_DrawNum(133,  y1, Stat.Sudo/100+143,       Img_Main_Energy, 15, 25);
	LCD_DrawNum(148,  y1, (Stat.Sudo%100)/10+143,  Img_Main_Energy, 15, 25);
	LCD_DrawNum(163,  y1, Stat.Sudo%10+143,        Img_Main_Energy, 15, 25);

	LCD_DrawNum(221,  y1, Stat.Gas/100+143,        Img_Main_Energy, 15, 25);
	LCD_DrawNum(236,  y1, (Stat.Gas%100)/10+143,   Img_Main_Energy, 15, 25);
	LCD_DrawNum(251,  y1, Stat.Gas%10+143,         Img_Main_Energy, 15, 25);

	LCD_DrawNum(308,  y1, Stat.Onsu/100+143,       Img_Main_Energy, 15, 25);
	LCD_DrawNum(323,  y1, (Stat.Onsu%100)/10+143,  Img_Main_Energy, 15, 25);
	LCD_DrawNum(338,  y1, Stat.Onsu%10+143,        Img_Main_Energy, 15, 25);

	LCD_DrawNum(395,  y1, Stat.Warm/100+143,       Img_Main_Energy, 15, 25);
	LCD_DrawNum(410,  y1, (Stat.Warm%100)/10+143,  Img_Main_Energy, 15, 25);
	LCD_DrawNum(425,  y1, Stat.Warm%10+143,        Img_Main_Energy, 15, 25);

	x=29; sz=7;
	LCD_DrawNum(x,   y2, Stat.Power_2/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, (Stat.Power_2%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, Stat.Power_2%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;
	LCD_DrawNum(x,   y2, Stat.Power_1/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, (Stat.Power_1%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, Stat.Power_1%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;
	LCD_DrawNum(x,   y2, Stat.Power/100+153,        Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, (Stat.Power%100)/10+153,   Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,   y2, Stat.Power%10+153,         Img_Main_Energy, 7, 11); x+=sz;

	x=118; sz=7;
	LCD_DrawNum(x,  y2, Stat.Sudo_2/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Sudo_2%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Sudo_2%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;
	LCD_DrawNum(x,  y2, Stat.Sudo_1/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Sudo_1%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Sudo_1%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;
	LCD_DrawNum(x,  y2, Stat.Sudo/100+153,        Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Sudo%100)/10+153,   Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Sudo%10+153,         Img_Main_Energy, 7, 11); x+=sz;

	x=205; sz=7;
	LCD_DrawNum(x,  y2, Stat.Gas_2/100+153,      Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, (Stat.Gas_2%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, Stat.Gas_2%10+153,       Img_Main_Energy, 7, 11); x+=sz;   
	x+=5;                                                                         
	LCD_DrawNum(x,  y2, Stat.Gas_1/100+153,      Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, (Stat.Gas_1%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, Stat.Gas_1%10+153,       Img_Main_Energy, 7, 11); x+=sz;   
	x+=5;                                                                         
	LCD_DrawNum(x,  y2, Stat.Gas/100+153,        Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, (Stat.Gas%100)/10+153,   Img_Main_Energy, 7, 11); x+=sz;   
	LCD_DrawNum(x,  y2, Stat.Gas%10+153,         Img_Main_Energy, 7, 11); x+=sz;   
	
	x=293; sz=7;
	LCD_DrawNum(x,  y2, Stat.Onsu_2/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Onsu_2%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Onsu_2%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;                                                                        
	LCD_DrawNum(x,  y2, Stat.Onsu_1/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Onsu_1%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Onsu_1%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;                                                                        
	LCD_DrawNum(x,  y2, Stat.Onsu/100+153,        Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Onsu%100)/10+153,   Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Onsu%10+153,         Img_Main_Energy, 7, 11); x+=sz;

	x=377; sz=7;
	LCD_DrawNum(x,  y2, Stat.Warm_2/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Warm_2%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Warm_2%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;                                                                        
	LCD_DrawNum(x,  y2, Stat.Warm_1/100+153,      Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Warm_1%100)/10+153, Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Warm_1%10+153,       Img_Main_Energy, 7, 11); x+=sz;
	x+=5;                                                                        
	LCD_DrawNum(x,  y2, Stat.Warm/100+153,        Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, (Stat.Warm%100)/10+153,   Img_Main_Energy, 7, 11); x+=sz;
	LCD_DrawNum(x,  y2, Stat.Warm%10+153,         Img_Main_Energy, 7, 11); x+=sz;
}
//--------------------------------------------------------------------------------------
void Display_Delivery ()
{
	LCD_Power_Set (_ON);
	Stat.Lcd_Sleep_100ms  = Timer_100ms + LCD_Sleep_100ms;

	LCD_DrawPicture(0, 0, 9, 480, 272, 0);
//	LCD_SetBackImage (9);
	playSound(250, Cfg.Volume);	
	
	Stat.Now_Screen  = Touch_None;
}
//--------------------------------------------------------------------------------------
void Display_Weather (uint8_t val)
{
	if (val < 1 || val > 7) return;				// 1 ~ 7
	LCD_Out (Touch_Out, 286, 35, 19+val, Img_Main_Home, 152, 150);
}
//--------------------------------------------------------------------------------------
void Display_Msg_Dust ()
{
int x=94, y=177, m;

	
	if (Stat.Finedust_Value == 0xffff)
		LCD_DrawNum(20,  	177, 39, 					Img_Main_Home,	61, 16);		// 알수없음
	else
		LCD_DrawNum(20,  	177, 39+Stat.Finedust_Stat, Img_Main_Home,	61, 16);		// 알수없음=39 ~
		
	LCD_DrawNum(80, 	y, 79,  					Img_Main_Home,	15, 25);	// (
	
	
	if (Stat.Finedust_Value == 0xffff)
		{
		LCD_DrawNum(x+15, y, 80,  			Img_Main_Home,	15, 25);			// )
		return;
		}
	
	if (Stat.Finedust_Value > 100)
		{
		LCD_DrawNum(x,    y, 81+(Stat.Finedust_Value/100), Img_Main_Home,	15, 25);
		m = Stat.Finedust_Value % 100;
		LCD_DrawNum(x+15, y, 81+(m / 10), 	Img_Main_Home,	15, 25);
		LCD_DrawNum(x+30, y, 81+(m % 10), 	Img_Main_Home,	15, 25);
		LCD_DrawNum(x+45, y, 80,  			Img_Main_Home,	15, 25);			// )
		}
	else
		{
		m = Stat.Finedust_Value;	
		LCD_DrawNum(x   , y, 81+(m / 10), 	Img_Main_Home,	15, 25);
		LCD_DrawNum(x+15, y, 81+(m % 10), 	Img_Main_Home,	15, 25);
		LCD_DrawNum(x+30, y, 80,  			Img_Main_Home,	15, 25);			// )
		}
}
//--------------------------------------------------------------------------------------
//  B1-1111, B1-111, B1-11, B1-1
//
void Display_Msg_Parking (int8_t *p)
{
int x=173, y=177, len=0, i;
	
	for (i=0; i<7; i++)
		{
		if (*(p+i) == 0x00) break;
		len ++;
		}	
	
	switch (len)
		{
		case 7 : x=173; break;
		case 6 : x=180; break;
		case 5 : x=188; break;	
		default: x=188; break;	
		}
			
	LCD_DrawNum(x,      y, *p-'A'+107, Img_Main_Home,	15, 25);		p++;	// b
	LCD_DrawNum(x+15,   y, *p-'0'+92,  Img_Main_Home,	15, 25);		p++;	// 1
	
	LCD_DrawNum(x+30,   y,        91,  Img_Main_Home,	15, 25);				// -
	if (*p == 0x2d)	p++;														// '-' 가 오면 무시
	
	LCD_DrawNum(x+45,   y, *p-'0'+92,  Img_Main_Home,	15, 25);		
	p++;	// 0
	if (*p  >= '0' && *p <= '9')
		LCD_DrawNum(x+60,   y, *p-'0'+92,  Img_Main_Home,	15, 25);		
	
	p++;	// 0
	if (*p  >= '0' && *p <= '9')
		LCD_DrawNum(x+75,   y, *p-'0'+92,  Img_Main_Home,	15, 25);		
		
	p++;	// 0
	if (*p  >= '0' && *p <= '9')
		LCD_DrawNum(x+90,   y, *p-'0'+92,  Img_Main_Home,	15, 25);			// 0
}
//--------------------------------------------------------------------------------------
void Display_Temp (uint8_t val)
{
uint8_t bu=0;

	if (val == 0x64) return;		// 100도 가 오면 시간날씨를 못받은 것으로 취소
	if (Stat.Now_Screen != Touch_Out) return;

	if (val & 0x80)	
		{ bu  = 1; val &= 0x7f; }
		
	if (bu)
		{
		LCD_DrawNum(364, 228, Img_Temp_Mi, Img_Main_Home,	15, 25);
		}

	LCD_DrawNum(383, 232, Img_Temp_0+(val/10), Img_Main_Home,	15, 25);
	LCD_DrawNum(399, 232, Img_Temp_0+(val%10), Img_Main_Home,	15, 25);
}
//--------------------------------------------------------------------------------------
void Print_Pkt (int *rtx, uint8_t *s)
{
uint8_t tmp[128], *p;

	sprintf (tmp, "%d %s ",Timer_100ms, rtx);
    p = tmp;
    p += strlen(tmp);

	sprintf (p, "%02x>%02x  %02x  %02x%02x %02x%02x %02x%02x %02x%02x", 
		s[Pkt_ASH], s[Pkt_ADH], s[Pkt_OP], 
		s[10], s[11], s[12], s[13], s[14], s[15], s[16], s[17]);
	Send_232 (tmp);
}	







	
		


