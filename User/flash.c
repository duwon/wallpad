/**
  ******************************************************************************
  * @file    flash.c
  * @author  정두원
  * @date    2020-04-20
  * @brief   N25Q Flash 제어
  * @details
  1. 사용 방법:
  --------------------------
    + 초기화
      o Flash_Init() 함수로 초기화
    + 사용
  */ 

#include "flash.h"
#include <stdio.h>

#if (_FLASH_DEBUG == 1)
#include <stdio.h>
#endif

#define FLASH_DUMMY_BYTE 0xA5

N25qxx_t N25qxx;

#define N25qxx_Delay(delay) //HAL_Delay(delay)

/** @defgroup 생활정보기_FLASH N25Q128A FLASH 제어 함수
  * @{
  */

/**
 * @brief 
 * 
 * @param Data 
 * @return uint8_t 
 */
uint8_t N25qxx_Spi(uint8_t Data)
{
	uint8_t ret;
	HAL_SPI_TransmitReceive(&_FLASH_SPI, &Data, &ret, 1, 100);
	return ret;
}

/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t N25qxx_ReadID(void)
{
	uint32_t Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x9F);
	Temp0 = N25qxx_Spi(FLASH_DUMMY_BYTE);
	Temp1 = N25qxx_Spi(FLASH_DUMMY_BYTE);
	Temp2 = N25qxx_Spi(FLASH_DUMMY_BYTE);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
	return Temp;
}

/**
 * @brief 
 * 
 */
void N25qxx_ReadUniqID(void)
{
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x4B);
	for (uint8_t i = 0; i < 4; i++)
		N25qxx_Spi(FLASH_DUMMY_BYTE);
	for (uint8_t i = 0; i < 8; i++)
		N25qxx.UniqID[i] = N25qxx_Spi(FLASH_DUMMY_BYTE);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief 
 * 
 */
void N25qxx_WriteEnable(void)
{
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x06);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_Delay(1);
}

/**
 * @brief 
 * 
 */
void N25qxx_WriteDisable(void)
{
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x04);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_Delay(1);
}

/**
 * @brief 
 * 
 * @param SelectStatusRegister_1_2_3 
 * @return uint8_t 
 */
uint8_t N25qxx_ReadStatusRegister(uint8_t SelectStatusRegister_1_2_3)
{
	uint8_t status = 0;
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	if (SelectStatusRegister_1_2_3 == 1)
	{
		N25qxx_Spi(0x05);
		status = N25qxx_Spi(FLASH_DUMMY_BYTE);
		N25qxx.StatusRegister1 = status;
	}
	else if (SelectStatusRegister_1_2_3 == 2)
	{
		N25qxx_Spi(0x35);
		status = N25qxx_Spi(FLASH_DUMMY_BYTE);
		N25qxx.StatusRegister2 = status;
	}
	else
	{
		N25qxx_Spi(0x15);
		status = N25qxx_Spi(FLASH_DUMMY_BYTE);
		N25qxx.StatusRegister3 = status;
	}
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	return status;
}

/**
 * @brief 
 * 
 * @param SelectStatusRegister_1_2_3 
 * @param Data 
 */
void N25qxx_WriteStatusRegister(uint8_t SelectStatusRegister_1_2_3, uint8_t Data)
{
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	if (SelectStatusRegister_1_2_3 == 1)
	{
		N25qxx_Spi(0x01);
		N25qxx.StatusRegister1 = Data;
	}
	else if (SelectStatusRegister_1_2_3 == 2)
	{
		N25qxx_Spi(0x31);
		N25qxx.StatusRegister2 = Data;
	}
	else
	{
		N25qxx_Spi(0x11);
		N25qxx.StatusRegister3 = Data;
	}
	N25qxx_Spi(Data);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief 
 * 
 */
void N25qxx_WaitForWriteEnd(void)
{
	N25qxx_Delay(1);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x05);
	do
	{
		N25qxx.StatusRegister1 = N25qxx_Spi(FLASH_DUMMY_BYTE);
		N25qxx_Delay(1);
	} while ((N25qxx.StatusRegister1 & 0x01) == 0x01);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool FlashInit(void)
{
	N25qxx.Lock = 1;
	while (HAL_GetTick() < 100)
		N25qxx_Delay(1);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_Delay(100);
	uint32_t id;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx Init Begin...\r\n");
#endif
	id = N25qxx_ReadID();

#if (_FLASH_DEBUG == 1)
	printf("N25qxx ID:0x%X\r\n", id);
#endif
	switch (id & 0x0000FFFF)
	{
	case 0x401A: // 	n25q512
		N25qxx.ID = N25Q512;
		N25qxx.BlockCount = 1024;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q512\r\n");
#endif
		break;
	case 0x4019: // 	n25q256
		N25qxx.ID = N25Q256;
		N25qxx.BlockCount = 512;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q256\r\n");
#endif
		break;
	case 0x4018: // 	n25q128
		N25qxx.ID = N25Q128;
		N25qxx.BlockCount = 256;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q128\r\n");
#endif
		break;
	case 0x4017: //	n25q64
		N25qxx.ID = N25Q64;
		N25qxx.BlockCount = 128;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q64\r\n");
#endif
		break;
	case 0x4016: //	n25q32
		N25qxx.ID = N25Q32;
		N25qxx.BlockCount = 64;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q32\r\n");
#endif
		break;
	case 0x4015: //	n25q16
		N25qxx.ID = N25Q16;
		N25qxx.BlockCount = 32;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q16\r\n");
#endif
		break;
	case 0x4014: //	n25q80
		N25qxx.ID = N25Q80;
		N25qxx.BlockCount = 16;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q80\r\n");
#endif
		break;
	case 0x4013: //	n25q40
		N25qxx.ID = N25Q40;
		N25qxx.BlockCount = 8;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q40\r\n");
#endif
		break;
	case 0x4012: //	n25q20
		N25qxx.ID = N25Q20;
		N25qxx.BlockCount = 4;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q20\r\n");
#endif
		break;
	case 0x4011: //	n25q10
		N25qxx.ID = N25Q10;
		N25qxx.BlockCount = 2;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q10\r\n");
#endif
		break;
	case 0x3217: // n25q128
		N25qxx.ID = N25Q128;
		N25qxx.BlockCount = 256;
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Chip: n25q128\r\n");
#endif
		break;
	default:
#if (_FLASH_DEBUG == 1)
		printf("N25qxx Unknown ID\r\n");
#endif
		N25qxx.Lock = 0;
		return false;
	}
	N25qxx.PageSize = 256;
	N25qxx.SectorSize = 0x1000;
	N25qxx.SectorCount = N25qxx.BlockCount * 16;
	N25qxx.PageCount = (N25qxx.SectorCount * N25qxx.SectorSize) / N25qxx.PageSize;
	N25qxx.BlockSize = N25qxx.SectorSize * 16;
	N25qxx.CapacityInKiloByte = (N25qxx.SectorCount * N25qxx.SectorSize) / 1024;
	N25qxx_ReadUniqID();
	N25qxx_ReadStatusRegister(1);
	N25qxx_ReadStatusRegister(2);
	N25qxx_ReadStatusRegister(3);
//#if (_FLASH_DEBUG == 1)
	printf("N25qxx Page Size: %d Bytes\r\n", N25qxx.PageSize);
	printf("N25qxx Page Count: %d\r\n", N25qxx.PageCount);
	printf("N25qxx Sector Size: %d Bytes\r\n", N25qxx.SectorSize);
	printf("N25qxx Sector Count: %d\r\n", N25qxx.SectorCount);
	printf("N25qxx Block Size: %d Bytes\r\n", N25qxx.BlockSize);
	printf("N25qxx Block Count: %d\r\n", N25qxx.BlockCount);
	printf("N25qxx Capacity: %d KiloBytes\r\n", N25qxx.CapacityInKiloByte);
	printf("N25qxx Init Done\r\n");
//#endif
	N25qxx.Lock = 0;
	return true;
}

/**
 * @brief 
 * 
 */
void N25qxx_EraseChip(void)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("N25qxx EraseChip Begin...\r\n");
#endif
	N25qxx_WriteEnable();
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0xC7);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_WaitForWriteEnd();
#if (_FLASH_DEBUG == 1)
	printf("N25qxx EraseBlock done after %d ms!\r\n", HAL_GetTick() - StartTime);
#endif
	N25qxx_Delay(10);
	N25qxx.Lock = 0;
}
/**
 * @brief 
 * 
 * @param SectorAddr 
 */
void N25qxx_EraseSector(uint32_t SectorAddr)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("N25qxx EraseSector %d Begin...\r\n", SectorAddr);
#endif
	N25qxx_WaitForWriteEnd();
	SectorAddr = SectorAddr * N25qxx.SectorSize;
	N25qxx_WriteEnable();
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x20);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((SectorAddr & 0xFF000000) >> 24);
	N25qxx_Spi((SectorAddr & 0xFF0000) >> 16);
	N25qxx_Spi((SectorAddr & 0xFF00) >> 8);
	N25qxx_Spi(SectorAddr & 0xFF);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_WaitForWriteEnd();
#if (_FLASH_DEBUG == 1)
	printf("N25qxx EraseSector done after %d ms\r\n", HAL_GetTick() - StartTime);
#endif
	N25qxx_Delay(1);
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param BlockAddr 
 */
void N25qxx_EraseBlock(uint32_t BlockAddr)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx EraseBlock %d Begin...\r\n", BlockAddr);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	N25qxx_WaitForWriteEnd();
	BlockAddr = BlockAddr * N25qxx.SectorSize * 16;
	N25qxx_WriteEnable();
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0xD8);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((BlockAddr & 0xFF000000) >> 24);
	N25qxx_Spi((BlockAddr & 0xFF0000) >> 16);
	N25qxx_Spi((BlockAddr & 0xFF00) >> 8);
	N25qxx_Spi(BlockAddr & 0xFF);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_WaitForWriteEnd();
#if (_FLASH_DEBUG == 1)
	printf("N25qxx EraseBlock done after %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx_Delay(1);
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param PageAddress 
 * @return uint32_t 
 */
uint32_t N25qxx_PageToSector(uint32_t PageAddress)
{
	return ((PageAddress * N25qxx.PageSize) / N25qxx.SectorSize);
}

/**
 * @brief 
 * 
 * @param PageAddress 
 * @return uint32_t 
 */
uint32_t N25qxx_PageToBlock(uint32_t PageAddress)
{
	return ((PageAddress * N25qxx.PageSize) / N25qxx.BlockSize);
}

/**
 * @brief 
 * 
 * @param SectorAddress 
 * @return uint32_t 
 */
uint32_t N25qxx_SectorToBlock(uint32_t SectorAddress)
{
	return ((SectorAddress * N25qxx.SectorSize) / N25qxx.BlockSize);
}

/**
 * @brief 
 * 
 * @param SectorAddress 
 * @return uint32_t 
 */
uint32_t N25qxx_SectorToPage(uint32_t SectorAddress)
{
	return (SectorAddress * N25qxx.SectorSize) / N25qxx.PageSize;
}

/**
 * @brief 
 * 
 * @param BlockAddress 
 * @return uint32_t 
 */
uint32_t N25qxx_BlockToPage(uint32_t BlockAddress)
{
	return (BlockAddress * N25qxx.BlockSize) / N25qxx.PageSize;
}

/**
 * @brief 
 * 
 * @param Page_Address 
 * @param OffsetInByte 
 * @param NumByteToCheck_up_to_PageSize 
 * @return true 
 * @return false 
 */
bool N25qxx_IsEmptyPage(uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_PageSize)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
	if (((NumByteToCheck_up_to_PageSize + OffsetInByte) > N25qxx.PageSize) || (NumByteToCheck_up_to_PageSize == 0))
		NumByteToCheck_up_to_PageSize = N25qxx.PageSize - OffsetInByte;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckPage:%d, Offset:%d, Bytes:%d begin...\r\n", Page_Address, OffsetInByte, NumByteToCheck_up_to_PageSize);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < N25qxx.PageSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Page_Address * N25qxx.PageSize);
		N25qxx_Spi(0x0B);
		if (N25qxx.ID >= N25Q256)
			N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		N25qxx_Spi(WorkAddress & 0xFF);
		N25qxx_Spi(0);
		HAL_SPI_Receive(&_FLASH_SPI, pBuffer, sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((N25qxx.PageSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < N25qxx.PageSize; i++)
		{
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Page_Address * N25qxx.PageSize);
			N25qxx_Spi(0x0B);
			if (N25qxx.ID >= N25Q256)
				N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			N25qxx_Spi(WorkAddress & 0xFF);
			N25qxx_Spi(0);
			HAL_SPI_Receive(&_FLASH_SPI, pBuffer, 1, 100);
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckPage is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckPage is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return false;
}

/**
 * @brief 
 * 
 * @param Sector_Address 
 * @param OffsetInByte 
 * @param NumByteToCheck_up_to_SectorSize 
 * @return true 
 * @return false 
 */
bool N25qxx_IsEmptySector(uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_SectorSize)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
	if ((NumByteToCheck_up_to_SectorSize > N25qxx.SectorSize) || (NumByteToCheck_up_to_SectorSize == 0))
		NumByteToCheck_up_to_SectorSize = N25qxx.SectorSize;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckSector:%d, Offset:%d, Bytes:%d begin...\r\n", Sector_Address, OffsetInByte, NumByteToCheck_up_to_SectorSize);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < N25qxx.SectorSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Sector_Address * N25qxx.SectorSize);
		N25qxx_Spi(0x0B);
		if (N25qxx.ID >= N25Q256)
			N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		N25qxx_Spi(WorkAddress & 0xFF);
		N25qxx_Spi(0);
		HAL_SPI_Receive(&_FLASH_SPI, pBuffer, sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((N25qxx.SectorSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < N25qxx.SectorSize; i++)
		{
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Sector_Address * N25qxx.SectorSize);
			N25qxx_Spi(0x0B);
			if (N25qxx.ID >= N25Q256)
				N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			N25qxx_Spi(WorkAddress & 0xFF);
			N25qxx_Spi(0);
			HAL_SPI_Receive(&_FLASH_SPI, pBuffer, 1, 100);
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckSector is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckSector is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return false;
}

/**
 * @brief 
 * 
 * @param Block_Address 
 * @param OffsetInByte 
 * @param NumByteToCheck_up_to_BlockSize 
 * @return true 
 * @return false 
 */
bool N25qxx_IsEmptyBlock(uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToCheck_up_to_BlockSize)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
	if ((NumByteToCheck_up_to_BlockSize > N25qxx.BlockSize) || (NumByteToCheck_up_to_BlockSize == 0))
		NumByteToCheck_up_to_BlockSize = N25qxx.BlockSize;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckBlock:%d, Offset:%d, Bytes:%d begin...\r\n", Block_Address, OffsetInByte, NumByteToCheck_up_to_BlockSize);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	uint8_t pBuffer[32];
	uint32_t WorkAddress;
	uint32_t i;
	for (i = OffsetInByte; i < N25qxx.BlockSize; i += sizeof(pBuffer))
	{
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
		WorkAddress = (i + Block_Address * N25qxx.BlockSize);
		N25qxx_Spi(0x0B);
		if (N25qxx.ID >= N25Q256)
			N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
		N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
		N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
		N25qxx_Spi(WorkAddress & 0xFF);
		N25qxx_Spi(0);
		HAL_SPI_Receive(&_FLASH_SPI, pBuffer, sizeof(pBuffer), 100);
		HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
		for (uint8_t x = 0; x < sizeof(pBuffer); x++)
		{
			if (pBuffer[x] != 0xFF)
				goto NOT_EMPTY;
		}
	}
	if ((N25qxx.BlockSize + OffsetInByte) % sizeof(pBuffer) != 0)
	{
		i -= sizeof(pBuffer);
		for (; i < N25qxx.BlockSize; i++)
		{
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
			WorkAddress = (i + Block_Address * N25qxx.BlockSize);
			N25qxx_Spi(0x0B);
			if (N25qxx.ID >= N25Q256)
				N25qxx_Spi((WorkAddress & 0xFF000000) >> 24);
			N25qxx_Spi((WorkAddress & 0xFF0000) >> 16);
			N25qxx_Spi((WorkAddress & 0xFF00) >> 8);
			N25qxx_Spi(WorkAddress & 0xFF);
			N25qxx_Spi(0);
			HAL_SPI_Receive(&_FLASH_SPI, pBuffer, 1, 100);
			HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
			if (pBuffer[0] != 0xFF)
				goto NOT_EMPTY;
		}
	}
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckBlock is Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return true;
NOT_EMPTY:
#if (_FLASH_DEBUG == 1)
	printf("N25qxx CheckBlock is Not Empty in %d ms\r\n", HAL_GetTick() - StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx.Lock = 0;
	return false;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param WriteAddr_inBytes 
 */
void N25qxx_WriteByte(uint8_t pBuffer, uint32_t WriteAddr_inBytes)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("N25qxx WriteByte 0x%02X at address %d begin...", pBuffer, WriteAddr_inBytes);
#endif
	N25qxx_WaitForWriteEnd();
	N25qxx_WriteEnable();
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x02);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((WriteAddr_inBytes & 0xFF000000) >> 24);
	N25qxx_Spi((WriteAddr_inBytes & 0xFF0000) >> 16);
	N25qxx_Spi((WriteAddr_inBytes & 0xFF00) >> 8);
	N25qxx_Spi(WriteAddr_inBytes & 0xFF);
	N25qxx_Spi(pBuffer);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_WaitForWriteEnd();
#if (_FLASH_DEBUG == 1)
	printf("N25qxx WriteByte done after %d ms\r\n", HAL_GetTick() - StartTime);
#endif
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Page_Address 
 * @param OffsetInByte 
 * @param NumByteToWrite_up_to_PageSize 
 */
void N25qxx_WritePage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_PageSize)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
	if (((NumByteToWrite_up_to_PageSize + OffsetInByte) > N25qxx.PageSize) || (NumByteToWrite_up_to_PageSize == 0))
		NumByteToWrite_up_to_PageSize = N25qxx.PageSize - OffsetInByte;
	if ((OffsetInByte + NumByteToWrite_up_to_PageSize) > N25qxx.PageSize)
		NumByteToWrite_up_to_PageSize = N25qxx.PageSize - OffsetInByte;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx WritePage:%d, Offset:%d ,Writes %d Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToWrite_up_to_PageSize);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	N25qxx_WaitForWriteEnd();
	N25qxx_WriteEnable();
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x02);
	Page_Address = (Page_Address * N25qxx.PageSize) + OffsetInByte;
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((Page_Address & 0xFF000000) >> 24);
	N25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	N25qxx_Spi((Page_Address & 0xFF00) >> 8);
	N25qxx_Spi(Page_Address & 0xFF);
	HAL_SPI_Transmit(&_FLASH_SPI, pBuffer, NumByteToWrite_up_to_PageSize, 100);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
	N25qxx_WaitForWriteEnd();
#if (_FLASH_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	printf("N25qxx WritePage done after %d ms\r\n", StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx_Delay(1);
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Sector_Address 
 * @param OffsetInByte 
 * @param NumByteToWrite_up_to_SectorSize 
 */
void N25qxx_WriteSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_SectorSize)
{
	if ((NumByteToWrite_up_to_SectorSize > N25qxx.SectorSize) || (NumByteToWrite_up_to_SectorSize == 0))
		NumByteToWrite_up_to_SectorSize = N25qxx.SectorSize;
#if (_FLASH_DEBUG == 1)
	printf("+++N25qxx WriteSector:%d, Offset:%d ,Write %d Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToWrite_up_to_SectorSize);
	N25qxx_Delay(100);
#endif
	if (OffsetInByte >= N25qxx.SectorSize)
	{
#if (_FLASH_DEBUG == 1)
		printf("---N25qxx WriteSector Faild!\r\n");
		N25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToWrite_up_to_SectorSize) > N25qxx.SectorSize)
		BytesToWrite = N25qxx.SectorSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_SectorSize;
	StartPage = N25qxx_SectorToPage(Sector_Address) + (OffsetInByte / N25qxx.PageSize);
	LocalOffset = OffsetInByte % N25qxx.PageSize;
	do
	{
		N25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= N25qxx.PageSize - LocalOffset;
		pBuffer += N25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
#if (_FLASH_DEBUG == 1)
	printf("---N25qxx WriteSector Done\r\n");
	N25qxx_Delay(100);
#endif
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Block_Address 
 * @param OffsetInByte 
 * @param NumByteToWrite_up_to_BlockSize 
 */
void N25qxx_WriteBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToWrite_up_to_BlockSize)
{
	if ((NumByteToWrite_up_to_BlockSize > N25qxx.BlockSize) || (NumByteToWrite_up_to_BlockSize == 0))
		NumByteToWrite_up_to_BlockSize = N25qxx.BlockSize;
#if (_FLASH_DEBUG == 1)
	printf("+++N25qxx WriteBlock:%d, Offset:%d ,Write %d Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToWrite_up_to_BlockSize);
	N25qxx_Delay(100);
#endif
	if (OffsetInByte >= N25qxx.BlockSize)
	{
#if (_FLASH_DEBUG == 1)
		printf("---N25qxx WriteBlock Faild!\r\n");
		N25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToWrite;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToWrite_up_to_BlockSize) > N25qxx.BlockSize)
		BytesToWrite = N25qxx.BlockSize - OffsetInByte;
	else
		BytesToWrite = NumByteToWrite_up_to_BlockSize;
	StartPage = N25qxx_BlockToPage(Block_Address) + (OffsetInByte / N25qxx.PageSize);
	LocalOffset = OffsetInByte % N25qxx.PageSize;
	do
	{
		N25qxx_WritePage(pBuffer, StartPage, LocalOffset, BytesToWrite);
		StartPage++;
		BytesToWrite -= N25qxx.PageSize - LocalOffset;
		pBuffer += N25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToWrite > 0);
#if (_FLASH_DEBUG == 1)
	printf("---N25qxx WriteBlock Done\r\n");
	N25qxx_Delay(100);
#endif
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Bytes_Address 
 */
void N25qxx_ReadByte(uint8_t *pBuffer, uint32_t Bytes_Address)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("N25qxx ReadByte at address %d begin...\r\n", Bytes_Address);
#endif
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x0B);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((Bytes_Address & 0xFF000000) >> 24);
	N25qxx_Spi((Bytes_Address & 0xFF0000) >> 16);
	N25qxx_Spi((Bytes_Address & 0xFF00) >> 8);
	N25qxx_Spi(Bytes_Address & 0xFF);
	N25qxx_Spi(0);
	*pBuffer = N25qxx_Spi(FLASH_DUMMY_BYTE);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
#if (_FLASH_DEBUG == 1)
	printf("N25qxx ReadByte 0x%02X done after %d ms\r\n", *pBuffer, HAL_GetTick() - StartTime);
#endif
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param ReadAddr 
 * @param NumByteToRead 
 */
void N25qxx_ReadBytes(uint8_t *pBuffer, uint32_t ReadAddr, uint32_t NumByteToRead)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
#if (_FLASH_DEBUG == 1)
	uint32_t StartTime = HAL_GetTick();
	printf("N25qxx ReadBytes at Address:%d, %d Bytes  begin...\r\n", ReadAddr, NumByteToRead);
#endif
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x0B);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((ReadAddr & 0xFF000000) >> 24);
	N25qxx_Spi((ReadAddr & 0xFF0000) >> 16);
	N25qxx_Spi((ReadAddr & 0xFF00) >> 8);
	N25qxx_Spi(ReadAddr & 0xFF);
	N25qxx_Spi(0);
	HAL_SPI_Receive(&_FLASH_SPI, pBuffer, NumByteToRead, 2000);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
#if (_FLASH_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	for (uint32_t i = 0; i < NumByteToRead; i++)
	{
		if ((i % 8 == 0) && (i > 2))
		{
			printf("\r\n");
			N25qxx_Delay(10);
		}
		printf("0x%02X,", pBuffer[i]);
	}
	printf("\r\n");
	printf("N25qxx ReadBytes done after %d ms\r\n", StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx_Delay(1);
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Page_Address 
 * @param OffsetInByte 
 * @param NumByteToRead_up_to_PageSize 
 */
void N25qxx_ReadPage(uint8_t *pBuffer, uint32_t Page_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_PageSize)
{
	while (N25qxx.Lock == 1)
		N25qxx_Delay(1);
	N25qxx.Lock = 1;
	if ((NumByteToRead_up_to_PageSize > N25qxx.PageSize) || (NumByteToRead_up_to_PageSize == 0))
		NumByteToRead_up_to_PageSize = N25qxx.PageSize;
	if ((OffsetInByte + NumByteToRead_up_to_PageSize) > N25qxx.PageSize)
		NumByteToRead_up_to_PageSize = N25qxx.PageSize - OffsetInByte;
#if (_FLASH_DEBUG == 1)
	printf("N25qxx ReadPage:%d, Offset:%d ,Read %d Bytes, begin...\r\n", Page_Address, OffsetInByte, NumByteToRead_up_to_PageSize);
	N25qxx_Delay(100);
	uint32_t StartTime = HAL_GetTick();
#endif
	Page_Address = Page_Address * N25qxx.PageSize + OffsetInByte;
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_RESET);
	N25qxx_Spi(0x0B);
	if (N25qxx.ID >= N25Q256)
		N25qxx_Spi((Page_Address & 0xFF000000) >> 24);
	N25qxx_Spi((Page_Address & 0xFF0000) >> 16);
	N25qxx_Spi((Page_Address & 0xFF00) >> 8);
	N25qxx_Spi(Page_Address & 0xFF);
	N25qxx_Spi(0);
	HAL_SPI_Receive(&_FLASH_SPI, pBuffer, NumByteToRead_up_to_PageSize, 100);
	HAL_GPIO_WritePin(_FLASH_CS_GPIO, _FLASH_CS_PIN, GPIO_PIN_SET);
#if (_FLASH_DEBUG == 1)
	StartTime = HAL_GetTick() - StartTime;
	printf("N25qxx ReadPage done after %d ms\r\n", StartTime);
	N25qxx_Delay(100);
#endif
	N25qxx_Delay(1);
	N25qxx.Lock = 0;
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Sector_Address 
 * @param OffsetInByte 
 * @param NumByteToRead_up_to_SectorSize 
 */
void N25qxx_ReadSector(uint8_t *pBuffer, uint32_t Sector_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_SectorSize)
{
	if ((NumByteToRead_up_to_SectorSize > N25qxx.SectorSize) || (NumByteToRead_up_to_SectorSize == 0))
		NumByteToRead_up_to_SectorSize = N25qxx.SectorSize;
#if (_FLASH_DEBUG == 1)
	printf("+++N25qxx ReadSector:%d, Offset:%d ,Read %d Bytes, begin...\r\n", Sector_Address, OffsetInByte, NumByteToRead_up_to_SectorSize);
	N25qxx_Delay(100);
#endif
	if (OffsetInByte >= N25qxx.SectorSize)
	{
#if (_FLASH_DEBUG == 1)
		printf("---N25qxx ReadSector Faild!\r\n");
		N25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToRead_up_to_SectorSize) > N25qxx.SectorSize)
		BytesToRead = N25qxx.SectorSize - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_SectorSize;
	StartPage = N25qxx_SectorToPage(Sector_Address) + (OffsetInByte / N25qxx.PageSize);
	LocalOffset = OffsetInByte % N25qxx.PageSize;
	do
	{
		N25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= N25qxx.PageSize - LocalOffset;
		pBuffer += N25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
#if (_FLASH_DEBUG == 1)
	printf("---N25qxx ReadSector Done\r\n");
	N25qxx_Delay(100);
#endif
}

/**
 * @brief 
 * 
 * @param pBuffer 
 * @param Block_Address 
 * @param OffsetInByte 
 * @param NumByteToRead_up_to_BlockSize 
 */
void N25qxx_ReadBlock(uint8_t *pBuffer, uint32_t Block_Address, uint32_t OffsetInByte, uint32_t NumByteToRead_up_to_BlockSize)
{
	if ((NumByteToRead_up_to_BlockSize > N25qxx.BlockSize) || (NumByteToRead_up_to_BlockSize == 0))
		NumByteToRead_up_to_BlockSize = N25qxx.BlockSize;
#if (_FLASH_DEBUG == 1)
	printf("+++N25qxx ReadBlock:%d, Offset:%d ,Read %d Bytes, begin...\r\n", Block_Address, OffsetInByte, NumByteToRead_up_to_BlockSize);
	N25qxx_Delay(100);
#endif
	if (OffsetInByte >= N25qxx.BlockSize)
	{
#if (_FLASH_DEBUG == 1)
		printf("N25qxx ReadBlock Faild!\r\n");
		N25qxx_Delay(100);
#endif
		return;
	}
	uint32_t StartPage;
	int32_t BytesToRead;
	uint32_t LocalOffset;
	if ((OffsetInByte + NumByteToRead_up_to_BlockSize) > N25qxx.BlockSize)
		BytesToRead = N25qxx.BlockSize - OffsetInByte;
	else
		BytesToRead = NumByteToRead_up_to_BlockSize;
	StartPage = N25qxx_BlockToPage(Block_Address) + (OffsetInByte / N25qxx.PageSize);
	LocalOffset = OffsetInByte % N25qxx.PageSize;
	do
	{
		N25qxx_ReadPage(pBuffer, StartPage, LocalOffset, BytesToRead);
		StartPage++;
		BytesToRead -= N25qxx.PageSize - LocalOffset;
		pBuffer += N25qxx.PageSize - LocalOffset;
		LocalOffset = 0;
	} while (BytesToRead > 0);
#if (_FLASH_DEBUG == 1)
	printf("---N25qxx ReadBlock Done\r\n");
	N25qxx_Delay(100);
#endif
}

#define BLOCK_CNT 3
#define BLOCK_REMAIN 0xFC00
void Flash_writeImage(uint8_t *fbAddress, int numImage)
{

	N25qxx_EraseBlock(0 + (numImage * 5));
	N25qxx_EraseBlock(1 + (numImage * 5));
	N25qxx_EraseBlock(2 + (numImage * 5));
	N25qxx_EraseBlock(3 + (numImage * 5));
#if (_FLASH_DEBUG == 1)
	printf("Erase Done\r\n");
#endif
	N25qxx_WriteBlock((uint8_t *)&fbAddress[0], 0 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_WriteBlock((uint8_t *)&fbAddress[0x10000], 1 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_WriteBlock((uint8_t *)&fbAddress[0x20000], 2 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_WriteBlock((uint8_t *)&fbAddress[0x30000], 3 + (numImage * 5), 0, BLOCK_REMAIN);
#if (_FLASH_DEBUG == 1)
	printf("Write 4\r\n");
#endif
}

void Flash_readImage(uint8_t *fbAddress, int numImage)
{
	N25qxx_ReadBlock((uint8_t *)&fbAddress[0], 0 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_ReadBlock((uint8_t *)&fbAddress[0x10000], 1 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_ReadBlock((uint8_t *)&fbAddress[0x20000], 2 + (numImage * 5), 0, N25qxx.BlockSize);
	N25qxx_ReadBlock((uint8_t *)&fbAddress[0x30000], 3 + (numImage * 5), 0, BLOCK_REMAIN);
#if (_FLASH_DEBUG == 1)
	printf("Read %d Done\r\n", numImage);
#endif
}

/**
  * @}
  */

