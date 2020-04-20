#ifndef FLASH_H__
#define FLASH_H__ 1

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "main.h"
#include "spi.h"

#define _FLASH_SPI                   hspi2
#define _FLASH_CS_GPIO               FLASH_CS_GPIO_Port
#define _FLASH_CS_PIN                FLASH_CS_Pin
#define _FLASH_DEBUG                 0

typedef enum
{
	N25Q10=1,
	N25Q20,
	N25Q40,
	N25Q80,
	N25Q16,
	N25Q32,
	N25Q64,
	N25Q128,
	N25Q256,
	N25Q512,
	
}FLASH_ID_t;

typedef struct
{
	FLASH_ID_t	ID;
	uint8_t		UniqID[8];
	uint16_t	PageSize;
	uint32_t	PageCount;
	uint32_t	SectorSize;
	uint32_t	SectorCount;
	uint32_t	BlockSize;
	uint32_t	BlockCount;
	uint32_t	CapacityInKiloByte;
	uint8_t		StatusRegister1;
	uint8_t		StatusRegister2;
	uint8_t		StatusRegister3;	
	uint8_t		Lock;
	
}N25qxx_t;

extern N25qxx_t	N25qxx;


bool		FlashInit(void);

void		N25qxx_EraseChip(void);
void 		N25qxx_EraseSector(uint32_t SectorAddr);
void 		N25qxx_EraseBlock(uint32_t BlockAddr);

uint32_t	N25qxx_PageToSector(uint32_t	PageAddress);
uint32_t	N25qxx_PageToBlock(uint32_t	PageAddress);
uint32_t	N25qxx_SectorToBlock(uint32_t	SectorAddress);
uint32_t	N25qxx_SectorToPage(uint32_t	SectorAddress);
uint32_t	N25qxx_BlockToPage(uint32_t	BlockAddress);

bool 		N25qxx_IsEmptyPage(uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_PageSize);
bool 		N25qxx_IsEmptySector(uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_SectorSize);
bool 		N25qxx_IsEmptyBlock(uint32_t Block_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_BlockSize);

void 		N25qxx_WriteByte(uint8_t pBuffer,uint32_t Bytes_Address);
void 		N25qxx_WritePage(uint8_t *pBuffer	,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_PageSize);
void 		N25qxx_WriteSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_SectorSize);
void 		N25qxx_WriteBlock(uint8_t* pBuffer,uint32_t Block_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_BlockSize);

void 		N25qxx_ReadByte(uint8_t *pBuffer,uint32_t Bytes_Address);
void 		N25qxx_ReadBytes(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead);
void 		N25qxx_ReadPage(uint8_t *pBuffer,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_PageSize);
void 		N25qxx_ReadSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_SectorSize);
void 		N25qxx_ReadBlock(uint8_t* pBuffer,uint32_t Block_Address,uint32_t OffsetInByte,uint32_t	NumByteToRead_up_to_BlockSize);

void 		Flash_writeImage(uint8_t *fbAddress, int numImage);
void 		Flash_readImage(uint8_t *fbAddress, int numImage);

#ifdef __cplusplus
}
#endif

#endif

