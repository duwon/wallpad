/**
  ******************************************************************************
  * @file    sound.c
  * @author  정두원
  * @date    2020-04-10
  * @brief   사운드 재생
  @verbatim
  1. 사용 방법:
  --------------------------
    + 초기화
      o touchInit() 함수로 초기화
    + 사용방법
      o touchValue() 함수 리턴 값이 터치센서 값
  @endverbatim
  */ 

#include <stdio.h>
#include "touch.h"
#include "i2c.h"

static void touchWriteRegData(uint8_t reg, uint8_t data);
static uint8_t touchReadRegData(uint8_t reg);

/** @defgroup 생활정보기_TOUCH 터치 읽기 함수
  * @{
  */

static void touchWriteRegData(uint8_t reg, uint8_t data)
{
  uint8_t txData[2] = {reg, data};
  HAL_I2C_Master_Transmit(&hi2c2, 0xD2, txData, 2, 10000);
  HAL_Delay(1);
}

static uint8_t touchReadRegData(uint8_t reg)
{
  uint8_t rxData[2] = {
      0,
  };
  HAL_I2C_Master_Transmit(&hi2c2, 0xD2, &reg, 1, 10000);
  HAL_I2C_Master_Receive(&hi2c2, 0xD2, rxData, 2, 10000);
  return rxData[0];
}

/**
  * @brief  터치 초기화
  * @retval None
  */
void touchInit(void)
{
  touchWriteRegData(0x04, 0x08); /* 소프트웨어 리셋 */
  HAL_Delay(100);

  //for (int i = 0; i < 0x16; i++)
  //{
  //  printf("%x %x\r\n", i, touchReadRegData(i));
  //}

  touchWriteRegData(0x00, 0xFF);
  touchWriteRegData(0x01, 0xFF);
  touchWriteRegData(0x02, 0xFF);
  touchWriteRegData(0x03, 0x05);
  touchWriteRegData(0x04, 0x13);
  touchWriteRegData(0x05, 0x00);
  touchWriteRegData(0x06, 0x00);
  touchWriteRegData(0x07, 0x00);
}

/**
  * @brief  터치센서의 값 읽기
  * @retval 터치 값
  */
uint8_t getTouchValue(void)
{
  return touchReadRegData(0x25);
}

/**
  * @}
  */


