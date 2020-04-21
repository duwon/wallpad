#ifndef MESSAGE_H__
#define MESSAGE_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define         UART_BUFFER_SIZE     100 /* UART 버퍼 크기 */

typedef struct
{
  uint8_t in;
  uint8_t out;
  uint8_t count;
  uint8_t buff[UART_BUFFER_SIZE];
  uint8_t ch;
} uartFIFO_TypeDef;     /* 수신 패킷 저장 버퍼 구조체 */


void messageInit(void);
void procMessage(void);


#ifdef __cplusplus
}
#endif


#endif
