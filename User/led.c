/**
  ******************************************************************************
  * @file    led.c
  * @author  정두원
  * @date    2020-04-20
  * @brief   5개 LED 제어
  * @details
  1. 사용 방법:
  --------------------------
    + 초기화는 gpio.c에서
    + 사용방법
      o 
  */

#include "led.h"
#include "gpio.h"

GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_Port,LED2_GPIO_Port,LED3_GPIO_Port,LED4_GPIO_Port,LED5_GPIO_Port};
const uint16_t LED_PIN[LEDn] = {LED1_Pin,LED2_Pin,LED3_Pin,LED4_Pin,LED5_Pin};

/** @defgroup 생활정보기_LED LED 제어 함수
  * @{
  */
 

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED_RDY
  * @retval None
  */
void LED_On(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_SET); 
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED_RDY
  * @retval None
  */
void LED_Off(Led_TypeDef Led)
{
  HAL_GPIO_WritePin(LED_PORT[Led], LED_PIN[Led], GPIO_PIN_RESET); 
}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *            @arg  LED_RDY
  * @retval None
  */
void LED_Toggle(Led_TypeDef Led)
{
  HAL_GPIO_TogglePin(LED_PORT[Led], LED_PIN[Led]);
}

/**
  * @}
  */
