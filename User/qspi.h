#ifndef QSPI_H__
#define QSPI_H__ 1

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdbool.h>
#include "main.h"

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


//bool		FlashInit(void);
//
//void		N25qxx_EraseChip(void);
//void 		N25qxx_EraseSector(uint32_t SectorAddr);
//void 		N25qxx_EraseBlock(uint32_t BlockAddr);
//
//uint32_t	N25qxx_PageToSector(uint32_t	PageAddress);
//uint32_t	N25qxx_PageToBlock(uint32_t	PageAddress);
//uint32_t	N25qxx_SectorToBlock(uint32_t	SectorAddress);
//uint32_t	N25qxx_SectorToPage(uint32_t	SectorAddress);
//uint32_t	N25qxx_BlockToPage(uint32_t	BlockAddress);
//
//bool 		N25qxx_IsEmptyPage(uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_PageSize);
//bool 		N25qxx_IsEmptySector(uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_SectorSize);
//bool 		N25qxx_IsEmptyBlock(uint32_t Block_Address,uint32_t OffsetInByte,uint32_t NumByteToCheck_up_to_BlockSize);
//
//void 		N25qxx_WriteByte(uint8_t pBuffer,uint32_t Bytes_Address);
//void 		N25qxx_WritePage(uint8_t *pBuffer	,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_PageSize);
//void 		N25qxx_WriteSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_SectorSize);
//void 		N25qxx_WriteBlock(uint8_t* pBuffer,uint32_t Block_Address,uint32_t OffsetInByte,uint32_t NumByteToWrite_up_to_BlockSize);
//
//void 		N25qxx_ReadByte(uint8_t *pBuffer,uint32_t Bytes_Address);
//void 		N25qxx_ReadBytes(uint8_t *pBuffer,uint32_t ReadAddr,uint32_t NumByteToRead);
//void 		N25qxx_ReadPage(uint8_t *pBuffer,uint32_t Page_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_PageSize);
//void 		N25qxx_ReadSector(uint8_t *pBuffer,uint32_t Sector_Address,uint32_t OffsetInByte,uint32_t NumByteToRead_up_to_SectorSize);
//void 		N25qxx_ReadBlock(uint8_t* pBuffer,uint32_t Block_Address,uint32_t OffsetInByte,uint32_t	NumByteToRead_up_to_BlockSize);
//
//void 		Flash_writeImage(uint8_t *fbAddress, int numImage);
//void 		Flash_readImage(uint8_t *fbAddress, int numImage);




/* N25Q512A Micron memory */
/* Size of the flash */
//#define QSPI_FLASH_SIZE                      23
//#define QSPI_PAGE_SIZE                       256
#define QSPI_FLASH_SIZE                  0x4000000 /* 512 MBits => 64MBytes */
#define QSPI_SECTOR_SIZE                 0x10000   /* 1024 sectors of 64KBytes */
#define QSPI_SUBSECTOR_SIZE              0x1000    /* 16384 subsectors of 4kBytes */
#define QSPI_PAGE_SIZE                   0x100     /* 262144 pages of 256 bytes */

/* Reset Operations */
#define RESET_ENABLE_CMD                     0x66
#define RESET_MEMORY_CMD                     0x99

/* Identification Operations */
#define READ_ID_CMD                          0x9E
#define READ_ID_CMD2                         0x9F
#define MULTIPLE_IO_READ_ID_CMD              0xAF
#define READ_SERIAL_FLASH_DISCO_PARAM_CMD    0x5A

/* Read Operations */
#define READ_CMD                             0x03
#define READ_4_BYTE_ADDR_CMD                 0x13

#define FAST_READ_CMD                        0x0B
#define FAST_READ_DTR_CMD                    0x0D
#define FAST_READ_4_BYTE_ADDR_CMD            0x0C

#define DUAL_OUT_FAST_READ_CMD               0x3B
#define DUAL_OUT_FAST_READ_DTR_CMD           0x3D
#define DUAL_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x3C

#define DUAL_INOUT_FAST_READ_CMD             0xBB
#define DUAL_INOUT_FAST_READ_DTR_CMD         0xBD
#define DUAL_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xBC

#define QUAD_OUT_FAST_READ_CMD               0x6B
#define QUAD_OUT_FAST_READ_DTR_CMD           0x6D
#define QUAD_OUT_FAST_READ_4_BYTE_ADDR_CMD   0x6C

#define QUAD_INOUT_FAST_READ_CMD             0xEB
#define QUAD_INOUT_FAST_READ_DTR_CMD         0xED
#define QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC

#define READ_STATUS_REG_CMD                  0x05
#define READ_CFG_REG_CMD                     0x15   
#define WRITE_STATUS_CFG_REG_CMD             0x01

/* Write Operations */
#define WRITE_ENABLE_CMD                     0x06
#define WRITE_DISABLE_CMD                    0x04

/* Register Operations */
#define READ_STATUS_REG_CMD                  0x05
#define WRITE_STATUS_REG_CMD                 0x01

#define READ_LOCK_REG_CMD                    0xE8
#define WRITE_LOCK_REG_CMD                   0xE5

#define READ_FLAG_STATUS_REG_CMD             0x70
#define CLEAR_FLAG_STATUS_REG_CMD            0x50

#define READ_NONVOL_CFG_REG_CMD              0xB5
#define WRITE_NONVOL_CFG_REG_CMD             0xB1

#define READ_VOL_CFG_REG_CMD                 0x85
#define WRITE_VOL_CFG_REG_CMD                0x81

#define READ_ENHANCED_VOL_CFG_REG_CMD        0x65
#define WRITE_ENHANCED_VOL_CFG_REG_CMD       0x61

#define READ_EXT_ADDR_REG_CMD                0xC8
#define WRITE_EXT_ADDR_REG_CMD               0xC5

/* Program Operations */
#define PAGE_PROG_CMD                        0x02
#define PAGE_PROG_4_BYTE_ADDR_CMD            0x12

#define DUAL_IN_FAST_PROG_CMD                0xA2
#define EXT_DUAL_IN_FAST_PROG_CMD            0xD2

#define QUAD_IN_FAST_PROG_CMD                0x32
#define EXT_QUAD_IN_FAST_PROG_CMD            0x12 /*0x38*/
#define QUAD_IN_FAST_PROG_4_BYTE_ADDR_CMD    0x34

/* Erase Operations */
#define SUBSECTOR_ERASE_CMD                  0x20
#define SUBSECTOR_ERASE_4_BYTE_ADDR_CMD      0x21

#define SECTOR_ERASE_CMD                     0xD8
#define SECTOR_ERASE_4_BYTE_ADDR_CMD         0xDC

#define BULK_ERASE_CMD                       0xC7

#define PROG_ERASE_RESUME_CMD                0x7A
#define PROG_ERASE_SUSPEND_CMD               0x75

/* One-Time Programmable Operations */
#define READ_OTP_ARRAY_CMD                   0x4B
#define PROG_OTP_ARRAY_CMD                   0x42

/* 4-byte Address Mode Operations */
#define ENTER_4_BYTE_ADDR_MODE_CMD           0xB7
#define EXIT_4_BYTE_ADDR_MODE_CMD            0xE9

/* Quad Operations */
#define ENTER_QUAD_CMD                       0x35
#define EXIT_QUAD_CMD                        0xF5

/* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ              8
#define DUMMY_CLOCK_CYCLES_READ_QUAD         10

#define DUMMY_CLOCK_CYCLES_READ_DTR          6
#define DUMMY_CLOCK_CYCLES_READ_QUAD_DTR     8

/* End address of the QSPI memory */
#define QSPI_END_ADDR              (1 << QSPI_FLASH_SIZE)

/* Size of buffers */
#define BUFFERSIZE                 (COUNTOF(aTxBuffer) - 1)

/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)        (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

#define QSPI_BULK_ERASE_MAX_TIME         600000
#define QSPI_SECTOR_ERASE_MAX_TIME       2000
#define QSPI_SUBSECTOR_ERASE_MAX_TIME    800

uint8_t QSPI_Init(void);
//void testFunction(void);
void qspiTestFucntion2(void);


uint8_t QSPI_Write(uint8_t* pData, uint32_t WriteAddr, uint32_t Size);
uint8_t QSPI_Read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
uint8_t QSPI_Erase_Block(uint32_t BlockAddress);
#ifdef __cplusplus
}
#endif

#endif

