/** 
 ******************************************************************************
  * @file    message.c
  * @author  정두원
  * @date    2020-04-21
  * @brif    메시지 처리 함수
  */

#include <stdio.h>
#include <stdbool.h>
#include "message.h"
#include "usart.h"

uartFIFO_TypeDef uart3Buffer, uart1Buffer;

/* printf IO 사용을 위한 설정 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/**
  * @brief  UART RX 인터럽트
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if (huart->Instance == USART3)
  { /* RS485 */
    HAL_UART_Receive_IT(huart, (uint8_t *)&uart3Buffer.ch, 1);
    putByteToBuffer(&uart3Buffer, uart3Buffer.ch);
  }
  if (huart->Instance == USART1)
  { /* Debug */
    HAL_UART_Receive_DMA(huart, (uint8_t *)&uart1Buffer.ch, 1);
    putByteToBuffer(&uart1Buffer, uart1Buffer.ch);
  }
}

/** @defgroup 생활정보기_MESSAGE UART 패킷 송수신
  * @{
  */
void messageInit(void)
{
  HAL_UART_Receive_IT(&huart3, (uint8_t *)&uart3Buffer.ch, 1); /* UART3(RS485) 인터럽트 시작 */
  HAL_UART_Receive_DMA(&huart1, (uint8_t *)&uart1Buffer.ch, 1);
}

/**
 * @brief UART 버퍼 1Byte 저장
 * 
 * @param uartFIFO_TypeDef : 버퍼 구조체 포인터
 * @param ch : 버퍼에 저장 할 값
 */
void putByteToBuffer(volatile uartFIFO_TypeDef *buffer, uint8_t ch)
{
  if (buffer->count != UART_BUFFER_SIZE) /* 데이터가 버퍼에 가득 찼으면 ERROR 리턴 */
  {
    buffer->buff[buffer->in++] = ch;    /* 버퍼에 1Byte 저장 */
    buffer->count++;                    /* 버퍼에 저장된 갯수 1 증가 */
    if (buffer->in == UART_BUFFER_SIZE) /* 시작 인덱스가 버퍼의 끝이면 */
    {
      buffer->in = 0; /* 시작 인덱스를 0부터 다시 시작 */
    }
  }
}

/**
 * @brief Get the Byte From Buffer object
 * 
 * @param uartFIFO_TypeDef : 버퍼 구조체 포인터
 * @param ch : 리턴받을 1byte 포인터
 * @return true : 데이터가 있으면
 * @return false : 버퍼에 데이터가 없으면
 */
bool getByteFromBuffer(volatile uartFIFO_TypeDef *buffer, uint8_t *ch)
{
  bool error;
  if (buffer->count != 0U) /* 버퍼에 데이터가 있으면 */
  {
    *ch = buffer->buff[buffer->out]; /* 버퍼에서 1Byte 읽음 */
    buffer->buff[buffer->out++] = 0;
    buffer->count--;                     /* 버퍼에 저장된 데이터 갯수 1 감소 */
    if (buffer->out == UART_BUFFER_SIZE) /* 끝 인덱스가 버퍼의 끝이면 */
    {
      buffer->out = 0; /* 끝 인덱스를 0부터 다시 시작 */
    }
    error = true;
  }
  else
  {
    error = false; /* 버퍼에 데이터가 없으면 false 리턴 */
  }
  return error;
}
/**
 * @brief RS485로 메시지 송신
 * 
 * @param data  : 전송할 데이터 포인터
 * @param len : 메시지 길이 0~255
 * @return uint8_t :   0 OK, 1 ERROR, 2 BUSY, 3 Timeout
 */
uint8_t sendMessage(uint8_t* data, uint8_t len)
{
  return (uint8_t)HAL_UART_Transmit(&huart3, data, len, 0xFFFF);
}

/**
 * @brief 메시지 처리 예제
 * 
 */
void procMessage(void)
{
  /* UART3(RS485) 데이터가 uart3Buffer 버퍼에 있으면 아스키 값 출력 */
  uint8_t rxCh = 0;
  while (getByteFromBuffer(&uart3Buffer, &rxCh) == true)
  {
    printf("%c",rxCh);
  }
}

/**
  * @}
  */
