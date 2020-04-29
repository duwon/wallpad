/** 
 ******************************************************************************
  * @file    qspi.c
  * @author  정두원
  * @date    2020-04-24
  * @brif    QSPI Flash 메모리 맵 방식 사용 설정
  */

#include <stdio.h>
#include <string.h>
#include "qspi.h"
#include "quadspi.h"
#include "led.h"

__IO uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch, TimeOut;

/* Buffer used for transmission */

static void QSPI_WriteEnable(QSPI_HandleTypeDef *QSPIHandle);
static void QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *QSPIHandle);
static void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *QSPIHandle);
static void CPU_CACHE_Enable(void);

/**
 * @brief QSPI 메모리 맵 방식 사용 설정
 * 
 */
void QSPI_EnableMemoryMapped(void)
{
  QSPI_CommandTypeDef      sCommand;
  QSPI_MemoryMappedTypeDef sMemMappedCfg;
  __IO uint8_t step = 0;
	
	CPU_CACHE_Enable();
	
	/* Init */
	HAL_QSPI_DeInit(&hqspi);
	if (HAL_QSPI_Init(&hqspi) != HAL_OK)
	{
		Error_Handler();
	}

		/* wait for end of program */  
		QSPI_AutoPollingMemReady(&hqspi);

	while(StatusMatch != 1)
	{
		StatusMatch = 0;
		RxCplt = 0;

		/* dummy */
		QSPI_DummyCyclesCfg(&hqspi);
		
		/* READ Command */
		sCommand.Instruction 				= READ_CMD;
		sCommand.DummyCycles 				= DUMMY_CLOCK_CYCLES_READ;
		sCommand.AddressMode 				= QSPI_ADDRESS_1_LINE;
		sCommand.DataMode    				= QSPI_DATA_1_LINE;
		
		sCommand.InstructionMode   	= QSPI_INSTRUCTION_1_LINE;
		sCommand.AddressSize       	= QSPI_ADDRESS_24_BITS;
		sCommand.AlternateByteMode 	= QSPI_ALTERNATE_BYTES_NONE;
		sCommand.DdrMode           	= QSPI_DDR_MODE_DISABLE;
		sCommand.DdrHoldHalfCycle  	= QSPI_DDR_HHC_ANALOG_DELAY;
		sCommand.SIOOMode          	= QSPI_SIOO_INST_EVERY_CMD;				

		sMemMappedCfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
		sMemMappedCfg.TimeOutPeriod     = 0;
		
    /* Convert Memory Mapped Mode */
		HAL_QSPI_MemoryMapped(&hqspi, &sCommand, &sMemMappedCfg);
	}
}


/**
  * @brief  Command completed callbacks.
  * @param  hqspi: QSPI handle
  * @retval None
  */
void HAL_QSPI_CmdCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  CmdCplt++;
}

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  hqspi: QSPI handle
  */
void HAL_QSPI_RxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  RxCplt++;
}

/**
  * @brief  Tx Transfer completed callbacks.
  * @param  hqspi: QSPI handle
  */
void HAL_QSPI_TxCpltCallback(QSPI_HandleTypeDef *hqspi)
{
  TxCplt++; 
}

/**
  * @brief  Status Match callbacks
  * @param  hqspi: QSPI handle
  */
void HAL_QSPI_StatusMatchCallback(QSPI_HandleTypeDef *hqspi)
{
  StatusMatch++;
}

/**
  * @brief  This function sends a Write Enable and waits until it is effective.
  * @param  hqspi: QSPI handle
  */
static void QSPI_WriteEnable(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Enable write operations ------------------------------------------ */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = WRITE_ENABLE_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_NONE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode          = QSPI_SIOO_INST_EVERY_CMD;

  if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
  
  /* Configure automatic polling mode to wait for write enabling ---- */  
  sConfig.Match           = 0x02;
  sConfig.Mask            = 0x02;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  sCommand.Instruction    = READ_STATUS_REG_CMD;
  sCommand.DataMode       = QSPI_DATA_1_LINE;

  if (HAL_QSPI_AutoPolling(&hqspi, &sCommand, &sConfig, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function reads the SR of the memory and awaits the EOP.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static void QSPI_AutoPollingMemReady(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef     sCommand;
  QSPI_AutoPollingTypeDef sConfig;

  /* Configure automatic polling mode to wait for memory ready ------ */  
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_STATUS_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode         = QSPI_SIOO_INST_EVERY_CMD;

  sConfig.Match           = 0x00;
  sConfig.Mask            = 0x01;
  sConfig.MatchMode       = QSPI_MATCH_MODE_AND;
  sConfig.StatusBytesSize = 1;
  sConfig.Interval        = 0x10;
  sConfig.AutomaticStop   = QSPI_AUTOMATIC_STOP_ENABLE;

  if (HAL_QSPI_AutoPolling_IT(&hqspi, &sCommand, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function configures the dummy cycles on memory side.
  * @param  hqspi: QSPI handle
  * @retval None
  */
static void QSPI_DummyCyclesCfg(QSPI_HandleTypeDef *QSPIHandle)
{
  QSPI_CommandTypeDef sCommand;
  uint8_t reg;

  /* Read Volatile Configuration register --------------------------- */
  sCommand.InstructionMode   = QSPI_INSTRUCTION_1_LINE;
  sCommand.Instruction       = READ_VOL_CFG_REG_CMD;
  sCommand.AddressMode       = QSPI_ADDRESS_NONE;
  sCommand.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
  sCommand.DataMode          = QSPI_DATA_1_LINE;
  sCommand.DummyCycles       = 0;
  sCommand.DdrMode           = QSPI_DDR_MODE_DISABLE;
  sCommand.DdrHoldHalfCycle  = QSPI_DDR_HHC_ANALOG_DELAY;
  sCommand.SIOOMode         = QSPI_SIOO_INST_EVERY_CMD;
  sCommand.NbData            = 1;

  if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_QSPI_Receive(&hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /* Enable write operations ---------------------------------------- */
  QSPI_WriteEnable(&hqspi);

  /* Write Volatile Configuration register (with new dummy cycles) -- */  
  sCommand.Instruction = WRITE_VOL_CFG_REG_CMD;
  MODIFY_REG(reg, 0xF0, (DUMMY_CLOCK_CYCLES_READ_QUAD << POSITION_VAL(0xF0)));
      
  if (HAL_QSPI_Command(&hqspi, &sCommand, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_QSPI_Transmit(&hqspi, &reg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  CPU L1-Cache enable.
  * 
  */
static void CPU_CACHE_Enable(void)
{
  /* Enable I-Cache */
  SCB_EnableICache();

  /* Enable D-Cache */
  SCB_EnableDCache();
}



