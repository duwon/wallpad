#ifndef LED_H__
#define LED_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define LEDn    5

typedef enum 
{
  LED1 = 0,
  LED2,
  LED3,
  LED4,
  LED5,
} Led_TypeDef;

void LED_On(Led_TypeDef Led);
void LED_Off(Led_TypeDef Led);
void LED_Toggle(Led_TypeDef Led);

#ifdef __cplusplus
}
#endif


#endif
