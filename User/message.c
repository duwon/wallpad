
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

extern uint32_t  Timer_100ms;
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
  * @param huart
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{

  if (huart->Instance == USART3)
	  	{	 /* RS485 */
    	putByteToBuffer(&uart3Buffer, uart3Buffer.ch);
    	HAL_UART_Receive_DMA (huart, (uint8_t *)&uart3Buffer.ch, 1);
  		}
  		
  if (huart->Instance == USART1)
	  	{ /* Debug */
    	putByteToBuffer(&uart1Buffer, uart1Buffer.ch);
    	HAL_UART_Receive_DMA(huart, (uint8_t *)&uart1Buffer.ch, 1);
  		}
}

/**
 * @brief UART 에러 발생 인터럽트
 * @param huart 
 * @retval None
 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(huart->ErrorCode == HAL_UART_ERROR_ORE)
  	{
    /* Overrun error 처리 */
    while (1) HAL_Delay(10);		// reset
  	}
}

/** @defgroup 생활정보기_MESSAGE UART 패킷 송수신
  * @{
  */
void messageInit(void)
{
	HAL_UART_Receive_DMA (&huart3, (uint8_t *)&uart3Buffer.ch, 1); /* UART3(RS485) 인터럽트 시작 */
	HAL_UART_Receive_DMA(&huart1, (uint8_t *)&uart1Buffer.ch, 1);

  uart3Buffer.Head = 0;
  uart3Buffer.Tail = 0;
  uart1Buffer.Head = 0;
  uart1Buffer.Tail = 0;
  
}

/**
 * @brief UART 버퍼 1Byte 저장
 * 
 * @param uartFIFO_TypeDef : 버퍼 구조체 포인터
 * @param ch : 버퍼에 저장 할 값
 */
void putByteToBuffer(volatile uartFIFO_TypeDef *buffer, uint8_t ch)
{
  buffer->buff[buffer->Head] = ch;
  if (++buffer->Head >= UART_BUFFER_SIZE) buffer->Head = 0;
  if (buffer->Head == buffer->Tail) 
  	{
  	if (++buffer->Tail >= UART_BUFFER_SIZE) buffer->Tail = 0;
  	}
#if 0  	
  if (buffer->count != UART_BUFFER_SIZE) /* 데이터가 버퍼에 가득 찼으면 ERROR 리턴 */
  {
    buffer->buff[buffer->in++] = ch;    /* 버퍼에 1Byte 저장 */
    buffer->count++;                    /* 버퍼에 저장된 갯수 1 증가 */
    if (buffer->in == UART_BUFFER_SIZE) /* 시작 인덱스가 버퍼의 끝이면 */
    {
      buffer->in = 0; /* 시작 인덱스를 0부터 다시 시작 */
    }
  }
#endif  
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
	if (buffer->Head == buffer->Tail) return false;

	*ch = buffer->buff[buffer->Tail]; 	/* 버퍼에서 1Byte 읽음 */
	if (++buffer->Tail >= UART_BUFFER_SIZE)	buffer->Tail = 0;
	return true;	

#if 0 
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
#endif 
}

/**
 * @brief 메시지 처리 예제
 * 
void procMessage(void)
{
  uint8_t rxCh = 0;
  while (getByteFromBuffer(&uart3Buffer, &rxCh) == true)
  {
    //printf("%c",rxCh);
    HAL_UART_Transmit(&huart3, &rxCh, 1, 0xFFFF);
  }
}
*/
//---------------------------------------------------------------
void Send_232 (uint8_t *data)
{
int len;
	len = strlen (data);
	*(data+len)   = 0x0d;
	*(data+len+1) = 0x0a;
	HAL_UART_Transmit(&huart1, data, len+2, 0xFFFF);
}
//---------------------------------------------------------------
void Send_485 (uint8_t *data, uint8_t len)
{
  HAL_NVIC_DisableIRQ(USART3_IRQn);
  HAL_UART_Transmit(&huart3, data, len, 0xFFFF);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
}
//---------------------------------------------------------------
uint8_t Read_485 (uint8_t *buff)
{
	if (getByteFromBuffer(&uart3Buffer, buff) == true)	
		return 1;
	else
		return 0;
}			

//=============================================================
void An_DataPrint (char *RTx, char *buff, int len)
{
int i, p=0;
char tmp[1000];

	sprintf (tmp, "%d %s = ",Timer_100ms, RTx);
    p = strlen(tmp);

   	for (i=0; i<len; i++)
	   	{
		sprintf (&tmp[p], "%02X ", (unsigned char)*buff++);
		p += 3;
   		if (p > 993) break;
   		}
	strcpy (&tmp[p], "\xd\xa\x0");
    Send_232 (tmp);
}
















/*
uint8_t Read_485 (uint8_t *buff)
{
uint8_t cnt=0;

	while (1)
		{
		if (getByteFromBuffer(&uart3Buffer, buff++) == true)	
			if (++cnt > 64) return cnt;
		else
			break;	
		}
	return cnt;
}	
*/
