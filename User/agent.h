//#define AGENCY_HYUNDAI
//#define AGENCY_CVNET
//#define AGENCY_SDS
#define AGENCY_KOCOM
//#define AGENCY_COMMAX
//#define AGENCY_ICONTROLS
//#define AGENCY_KYUNGDONG_ONE




// 고객별 정의





// LED 정의

#ifdef AGENCY_KOCOM
	#define LED_GAS		LED_1
	#define LED_ELV			P06
	#define LED_LIT		P05
	#define LED_LIT		P05
	#define LED_LIT		P05
	
	#define TOUCH_ILGUAL	touch1_cnt
	#define TOUCH_GAS		touch2_cnt
	#define TOUCH_ELEV		touch3_cnt
#endif



#ifdef AGENCY_KOCOM
#include	"hd_uart.h"
#endif
#if defined(AGENCY_CVNET)
#include	"cv_uart.h"
#define UART_PARITY		UART_PARITY_NONE