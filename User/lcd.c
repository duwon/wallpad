#include <string.h>
#include "lcd.h"
#include "ltdc.h"
#include "dma2d.h"
#include "user.h"

static LTDC_HandleTypeDef  hLtdcHandler;
static DMA2D_HandleTypeDef hDma2dHandler;
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

typedef struct 
{ 
  uint8_t B;
  uint8_t G;
  uint8_t R;
  uint8_t A;
  
}ARGB8888_TypeDef;

//----------------------------------------------------------------------
void LCD_Init(void)
{
	hLtdcHandler = hltdc;
	hDma2dHandler = hdma2d;
	HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);
}
//----------------------------------------------------------------------
void LCD_Power_Set (int8_t flag)
{
static int8_t be=0xff;
	
	if (be == flag) return;
	be = flag;	
	if (flag)
		HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);
	else	
		HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_RESET);
}
/**----------------------------------------------------------------------
  * @brief  LCD의 가로 크기
  * @retval LCD X size
  */
uint32_t LCD_GetXSize(void)
{
	return hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth;
}
/**----------------------------------------------------------------------
  * @brief  LCD의 세로 크기
  * @retval LCD Y size
  */
uint32_t LCD_GetYSize(void)
{
	return hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight;
}
/**----------------------------------------------------------------------
  * @brief  LCD의 가로 크기 설정. 초기값 480px
  * @param  imageWidthPixels : 이미지 가로 크기 (Pixels)
  * @retval None
  */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
	hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}
/**----------------------------------------------------------------------
  * @brief  LCD의 세로 크기 설정. 초기값 272px
  * @param  imageHeightPixels : 이미지 세로 크기 (Pixels)
  * @retval None
  */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
	hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}
/**----------------------------------------------------------------------
  * @brief  RGB565 format LCD Layer 초기화. 초기값 0 Layer
  * @param  LayerIndex: 레이어 0 or 1
  * @param  FB_Address: 레이어 프레임 버퍼
  * @retval None
  */
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address)
{     
  	LTDC_LayerCfgTypeDef  layer_cfg;

  	layer_cfg.WindowX0 = 0;
  	layer_cfg.WindowX1 = LCD_GetXSize();
  	layer_cfg.WindowY0 = 0;
  	layer_cfg.WindowY1 = LCD_GetYSize(); 
  	layer_cfg.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  	layer_cfg.FBStartAdress = FB_Address;
  	layer_cfg.Alpha = 255;
  	layer_cfg.Alpha0 = 0;
  	layer_cfg.Backcolor.Blue = 0;
  	layer_cfg.Backcolor.Green = 0;
  	layer_cfg.Backcolor.Red = 0;
  	layer_cfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  	layer_cfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  	layer_cfg.ImageWidth = LCD_GetXSize();
  	layer_cfg.ImageHeight = LCD_GetYSize();
  
  	HAL_LTDC_ConfigLayer(&hLtdcHandler, &layer_cfg, LayerIndex); 

  	DrawProp[LayerIndex].BackColor = LCD_COLOR_WHITE;
//  DrawProp[LayerIndex].pFont     = &Font24;
  	DrawProp[LayerIndex].TextColor = LCD_COLOR_BLACK; 
  	DrawProp[LayerIndex].pBackImage= (uint16_t *)FB_Address; 
}

/**----------------------------------------------------------------------
  * @brief  레이어의 투명도 설정. 기본값 255
  * @param  LayerIndex: 레이어 0 or 1 (기본 0)
  * @param  Transparency: 투명도 
  *           0~255까지 입력 가능 
  * @retval None
  */
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency)
{    
	HAL_LTDC_SetAlpha(&hLtdcHandler, Transparency, LayerIndex);
}

/**----------------------------------------------------------------------
  * @brief  LCD 레이어 선택. 초기값 0
  * @param  LayerIndex: 0 또는 1
  * @retval None
  */
void LCD_SelectLayer(uint32_t LayerIndex)
{
	ActiveLayer = LayerIndex;
} 

/**----------------------------------------------------------------------
  * @brief  픽셀 또는 폰트 색상 설정
  * @param  Color: RGB565 색상
  * @retval None
  */
void LCD_SetTextColor(uint32_t Color)
{
	DrawProp[ActiveLayer].TextColor = Color;
}

/**----------------------------------------------------------------------
  * @brief  현재 설정된 픽셀 또는 폰트 색상 읽어오기
  * @retval RGB565 색상
  */
uint32_t LCD_GetTextColor(void)
{
	return DrawProp[ActiveLayer].TextColor;
}

/**----------------------------------------------------------------------
  * @brief  배경 생삭 설정
  * @param  Color: RGB565 색상
  * @retval None
  */
void LCD_SetBackColor(uint32_t Color)
{
	DrawProp[ActiveLayer].BackColor = Color;
}

/**----------------------------------------------------------------------
  * @brief  현재 설정된 LCD 배경 색상 읽어오기
  * @retval RGB565 색상
  */
uint32_t LCD_GetBackColor(void)
{
	return DrawProp[ActiveLayer].BackColor;
}

/**----------------------------------------------------------------------
  * @brief  배경 이미지 설정.
  * @param  pBackgroundImage: 배경으로 설정 할 이미지 포인터 (480x272 크기 사용*)
  * @retval None
  * 데이타의 첫 2바이트는 가로크기(빅엔디안),  다음 2바이트는 세로크기(빅엔디안)
  */
void LCD_SetBackImage(uint8_t no)
{
uint32_t ImageAddress, len;

	ImageAddress = Get_Address (Flash_Image_Base, no, &len);
	if (ImageAddress == 0 || len == 0) return;			// 에러
	
	
  	DrawProp[ActiveLayer].pBackImage = (uint16_t*)(ImageAddress +4);	

	/* RGB565 format */
  	hdma2d.Init.Mode         = DMA2D_M2M;
  	hdma2d.Init.ColorMode    = DMA2D_RGB565;
  	hdma2d.Init.OutputOffset = 0;
  	hdma2d.Instance = DMA2D;
  
  	/* DMA2D Initialization */
  	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
	  	{
    	if(HAL_DMA2D_ConfigLayer(&hdma2d, ActiveLayer) == HAL_OK) 
    		{
      		if (HAL_DMA2D_Start(&hdma2d, ImageAddress, (uint32_t)hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress, LCD_WIDTH, LCD_HEIGHT) == HAL_OK)
      			{
        		/* Polling For DMA transfer */  
        		HAL_DMA2D_PollForTransfer(&hdma2d, 100);
      			}
    		}
  		}
}
/**----------------------------------------------------------------------
  * @brief  LCD에 1픽셀 표시
  * @param  Xpos: X 좌표
  * @param  Ypos: Y 좌표
  * @param  RGB_Code: RGB565 색상
  * @retval None
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code)
{
	*(__IO uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos))) = RGB_Code;
}
/**----------------------------------------------------------------------
  * @brief  LCD 그림 표시
  * @param  Xpos: 시작 X 위치
  * @param  Ypos: 시작 Y 위치
  * @param  ImageAddress: 그림 시작 주소
  * @param  Xsize: 표시할 그림 가로축 크기 px
  * @param  Ysize: 표시할 그림 세로축 크기 px
  * @param  Color_Alpha0: 표시 할 그림의 뒷 배경 제거 색상. RGB565. 검정색 배경은 제거 불가능하며 0 입력 시 배경 제거 없이 출력
  *
  * 데이타의 첫 2바이트는 가로크기(빅엔디안),  다음 2바이트는 세로크기(빅엔디안)
  * @retval None
  */
void LCD_DrawPicture(uint16_t Xpos, uint16_t Ypos, uint8_t no, uint16_t Xsize, uint16_t Ysize, uint16_t Color_Alpha0)
{
__IO uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_WIDTH + Xpos)));
uint32_t ImageAddress, len;
uint16_t *pAddress;
uint8_t *xy;

	ImageAddress = Get_Address (Flash_Image_Base, no, &len);
	if (ImageAddress == 0 || len == 0) return;			// 에러
		
	pAddress = (uint16_t *)ImageAddress;

	if((Xpos+Xsize) > LCD_WIDTH)
		{
    	Xsize = LCD_WIDTH - Xpos;
  		}

  	if((Ypos+Ysize) > LCD_HEIGHT)
  		{
    	Ysize = LCD_HEIGHT - Ypos;
  		}


//	printf ("x=%d y=%d  %x %x\r\n", Xsize, Ysize, *pAddress, pAddress);

  	if (Color_Alpha0 != 0)
		{
    	for (int iY = 0; iY < Ysize; iY++)
    		{
      		for (int iX = 0; iX < Xsize; iX++)
      			{
        		if (pAddress[Xsize * iY + iX] != Color_Alpha0)
        			{
          			lcdBuffer[LCD_WIDTH * iY + iX] = pAddress[Xsize * iY + iX];
          			//LCD_DrawPixel(Xpos + iX, Ypos +iY, pAddress[Xsize * iY + iX]);
        			}
      			}
    		}
  		}
  else
  	{
    for (int iY = 0; iY < Ysize; iY++)
    	{
      	for (int iX = 0; iX < Xsize; iX++)
      		{
        	lcdBuffer[LCD_WIDTH * iY + iX] = pAddress[Xsize * iY + iX];
//        	LCD_DrawPixel(Xpos + iX, Ypos + iY, pAddress[Xsize * iY + iX]);
      		}
    	}
  	}
}

/**----------------------------------------------------------------------
  * @brief  LCD 그림 삭제
  * @param  Xpos: 시작 X 위치
  * @param  Ypos: 시작 Y 위치
  * @param  Xsize: 삭제 할 그림 가로축 크기 px
  * @param  Ysize: 삭제 할 그림 세로축 크기 px
  * @retval None
  */
void LCD_ErasePicture(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize)
{
	if((Xpos+Xsize) > LCD_WIDTH)
  		{
    	Xsize = LCD_WIDTH - Xpos;
  		}

  	if((Ypos+Ysize) > LCD_HEIGHT)
  		{
    	Ysize = LCD_WIDTH - Ypos;
  		}

  	__IO uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_WIDTH + Xpos)));

  	for (int iY = 0; iY < Ysize; iY++)
  		{
    	for (int iX = 0; iX < Xsize; iX++)
    		{
      		lcdBuffer[LCD_WIDTH * iY + iX] = DrawProp[ActiveLayer].pBackImage[(Ypos*LCD_WIDTH + (Xpos))+ LCD_WIDTH * iY + iX];
      		//for(int delay=0; delay<100; delay++);
    		}
  		}
}
/**----------------------------------------------------------------------
  * @brief  버퍼에 특정 색상 채우기
  * @param  LayerIndex: 레이어 0 or 1 (기본 0)
  * @param  pDst: 버퍼의 메리모리 주소
  * @param  xSize: 색상 채울 가로축 크기
  * @param  ySize: 색상 채울 세로축 크기
  * @param  OffLine: 
  * @param  ColorIndex: 채울 색상 RGB565
  * @retval None
  */
static void LL_FillBuffer(uint32_t LayerIndex, void *pDst, uint32_t xSize, uint32_t ySize, uint32_t OffLine, uint32_t ColorIndex) 
{
hdma2d.Init.Mode         = DMA2D_R2M; /* 레지스터에서 메모리로 복사 모드 */ 

	if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
		{ /* RGB565 */ 
    	hdma2d.Init.ColorMode    = DMA2D_RGB565;
  		}
  	else
  		{ /* ARGB8888 */
    	hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
  		}
  	hdma2d.Init.OutputOffset = OffLine;      
  	hdma2d.Instance = DMA2D;
  
	if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
  		{
    	if(HAL_DMA2D_ConfigLayer(&hdma2d, LayerIndex) == HAL_OK) 
    		{
      		if (HAL_DMA2D_Start(&hdma2d, ColorIndex, (uint32_t)pDst, xSize, ySize) == HAL_OK)
      			{
        		HAL_DMA2D_PollForTransfer(&hdma2d, 10);        /* Polling For DMA transfer */  
      			}
    		}
  		} 
}
/**----------------------------------------------------------------------
  * @brief  LCD에 문자열 표시
  * @param  Xpos: X 위치 (pixel)
  * @param  Ypos: Y 위치 (pixel)   
  * @param  Text: 표시 할 문자열 시작 메모리 주소 
  * @retval None
  */
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *Text)
{
uint16_t ref_column = 1;
//  uint16_t i = 0;
uint32_t size = 0; 
uint8_t  *ptr = Text;
  
	while (*ptr++) size ++ ;  /* 텍스트 크기 */
  
  	ref_column = Xpos;  /* 1라인의 크기 */
  
  	if ((ref_column < 1) || (ref_column >= 0x8000))  /* 표시 할 시작 메모리에서의 위치 */
  		{
    	ref_column = 1;
  		}

  /* Send the string character by character on LCD */
//  while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width))
//  {
//    /* Display one character on LCD */
//    LCD_DisplayChar(ref_column, Ypos, *Text);
    /* Decrement the column position by 16 */
//    ref_column += DrawProp[ActiveLayer].pFont->Width;
//    /* Point on the next character */
//    Text++;
//    i++;
//  }  
}
/**----------------------------------------------------------------
  * @brief  LCD 표시된 색상 전체 변경 
  * @param  Color: RGB565 생상 
  * @retval None
  */
void LCD_Clear(uint32_t Color)
{ 
	/* Clear the LCD */ 
	LL_FillBuffer(ActiveLayer, (uint32_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress), LCD_GetXSize(), LCD_GetYSize(), 0, Color);
}

/**
  * @brief  LCD 그림 표시
  * @param  Xpos: 시작 X 위치
  * @param  Ypos: 시작 Y 위치
  * @param  pAddress: ARGB8888 그림 시작 주소
  * @param  Xsize: 표시할 그림 가로축 크기 px
  * @param  Ysize: 표시할 그림 세로축 크기 px
  * @retval None
  */
void LCD_DrawNum(uint16_t Xpos, uint16_t Ypos, uint8_t fno, uint8_t bno, uint16_t Xsize, uint16_t Ysize)
{
 __IO uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_WIDTH + Xpos)));
uint32_t *pFgAddress, ImageAddress, len;
uint16_t *pBgAddress, *pAddress;

ARGB8888_TypeDef fg, bg, newPixel;

	ImageAddress = Get_Address (Flash_Image_Base, fno, &len);
	if (ImageAddress == 0 || len == 0) return;
	pFgAddress = (uint32_t *)ImageAddress;						// 표시할 그림의 

	ImageAddress = Get_Address (Flash_Image_Base, bno, &len);
	if (ImageAddress == 0 || len == 0) return;
	pBgAddress = (uint16_t *)(ImageAddress + (2 * (Ypos * LCD_WIDTH + Xpos)));

  if ((Xpos + Xsize) > LCD_WIDTH)
  	{
    Xsize = LCD_WIDTH - Xpos;
  	}

  if ((Ypos + Ysize) > LCD_HEIGHT)
  	{
    Ysize = LCD_HEIGHT - Ypos;
  	}


  for (int iY = 0; iY < Ysize; iY++)
  	{
    for (int iX = 0; iX < Xsize; iX++)
    	{
      	memcpy((void *)&fg, (void *)&pFgAddress[Xsize * iY + iX], 4);
      	uint16_t bgPixel = pBgAddress[LCD_WIDTH * iY + iX];
      	bg.A = (255 - fg.A);

      	fg.R >>= 3;
      	fg.G >>= 2;
      	fg.B >>= 3;

      	bg.R = (bgPixel >> 11) & 0x1F;
      	bg.G = (bgPixel >> 5) & 0x3F;
      	bg.B = bgPixel & 0x1F;

      	newPixel.R = ((fg.R * fg.A / 255) + (bg.R * bg.A / 255)) & 0x1F;
      	newPixel.G = ((fg.G * fg.A / 255) + (bg.G * bg.A / 255)) & 0x3F;
      	newPixel.B = ((fg.B * fg.A / 255) + (bg.B * bg.A / 255)) & 0x1F;

      	lcdBuffer[LCD_WIDTH * iY + iX] = (((newPixel.R) << 11) | ((newPixel.G) << 5) | (newPixel.B));
    	}
  	}
}


