#ifndef MESSAGE_H__
#define MESSAGE_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define         UART_BUFFER_SIZE     200 /* UART 버퍼 크기 */

typedef struct
{
  uint8_t Head;
  uint8_t Tail;
//  uint8_t count;
  uint8_t buff[UART_BUFFER_SIZE];
  uint8_t ch;
} uartFIFO_TypeDef;     /* 수신 패킷 저장 버퍼 구조체 */


void messageInit(void);
void procMessage(void);
void putByteToBuffer(volatile uartFIFO_TypeDef *buffer, uint8_t ch);
bool getByteFromBuffer(volatile uartFIFO_TypeDef *buffer, uint8_t *ch);
uint8_t sendMessage(uint8_t* data, uint8_t len);
void  Send_485(uint8_t *data, uint8_t len);
uint8_t Read_485 (uint8_t *buff);
void An_DataPrint (char *RTx, char *buff, int len);
void An_MsgPrint (const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif
