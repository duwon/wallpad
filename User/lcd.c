/**
  ******************************************************************************
  * @file    lcd.c
  * @author  정두원
  * @brief   LCD 제어
  * @details
  1. 사용 방법:
  --------------------------
    + 초기화
      o LCD_Init() 함수로 초기화
      o LCD_LayerInit() 함수를 사용하여 레이어 구성 설정
      o LCD_SelectLayer() 함수를 사용하여 사용할 LCD Layer 설정 (0 또는 1)
    + 사용
  */ 

#include <string.h>
#include "lcd.h"
#include "ltdc.h"
#include "dma2d.h"
#include "17x23NUM.h"

static LTDC_HandleTypeDef  hLtdcHandler;
static DMA2D_HandleTypeDef hDma2dHandler;
static uint32_t            ActiveLayer = 0;
static LCD_DrawPropTypeDef DrawProp[MAX_LAYER_NUMBER];

/** @defgroup 생활정보기_LCD LCD 제어 함수
  * @{
  */

/**
  * @brief  LCD 초기화
  * @retval None
  */
void LCD_Init(void)
{
  hLtdcHandler = hltdc;
  hDma2dHandler = hdma2d;
  HAL_GPIO_WritePin(LCD_PWM_GPIO_Port, LCD_PWM_Pin, GPIO_PIN_SET);
}

/**
  * @brief  LCD의 가로 크기
  * @retval LCD X size
  */
uint32_t LCD_GetXSize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth;
}

/**
  * @brief  LCD의 세로 크기
  * @retval LCD Y size
  */
uint32_t LCD_GetYSize(void)
{
  return hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight;
}

/**
  * @brief  LCD의 가로 크기 설정. 초기값 480px
  * @param  imageWidthPixels : 이미지 가로 크기 (Pixels)
  * @retval None
  */
void LCD_SetXSize(uint32_t imageWidthPixels)
{
  hLtdcHandler.LayerCfg[ActiveLayer].ImageWidth = imageWidthPixels;
}

/**
  * @brief  LCD의 세로 크기 설정. 초기값 272px
  * @param  imageHeightPixels : 이미지 세로 크기 (Pixels)
  * @retval None
  */
void LCD_SetYSize(uint32_t imageHeightPixels)
{
  hLtdcHandler.LayerCfg[ActiveLayer].ImageHeight = imageHeightPixels;
}

/**
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
  //DrawProp[LayerIndex].pBackImage= (uint16_t *)FB_Address; 
}

/**
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

/**
  * @brief  LCD 레이어 선택. 초기값 0
  * @param  LayerIndex: 0 또는 1
  * @retval None
  */
void LCD_SelectLayer(uint32_t LayerIndex)
{
  ActiveLayer = LayerIndex;
} 

/**
  * @brief  픽셀 또는 폰트 색상 설정
  * @param  Color: RGB565 색상
  * @retval None
  */
void LCD_SetTextColor(uint32_t Color)
{
  DrawProp[ActiveLayer].TextColor = Color;
}

/**
  * @brief  현재 설정된 픽셀 또는 폰트 색상 읽어오기
  * @retval RGB565 색상
  */
uint32_t LCD_GetTextColor(void)
{
  return DrawProp[ActiveLayer].TextColor;
}

/**
  * @brief  배경 생삭 설정
  * @param  Color: RGB565 색상
  * @retval None
  */
void LCD_SetBackColor(uint32_t Color)
{
  DrawProp[ActiveLayer].BackColor = Color;
}

/**
  * @brief  현재 설정된 LCD 배경 색상 읽어오기
  * @retval RGB565 색상
  */
uint32_t LCD_GetBackColor(void)
{
  return DrawProp[ActiveLayer].BackColor;
}

/**
  * @brief  배경 이미지 설정.
  * @param  pBackgroundImage: 배경으로 설정 할 이미지 포인터 (480x272 크기 사용*)
  * @retval None
  */
void LCD_SetBackImage(uint32_t BackgroundImageAddress)
{
  //DrawProp[ActiveLayer].pBackImage = (uint16_t*)BackgroundImageAddress;

  hdma2d.Init.Mode         = DMA2D_M2M;
  if(hLtdcHandler.LayerCfg[ActiveLayer].PixelFormat == LTDC_PIXEL_FORMAT_RGB565)
  { /* RGB565 format */ 
    hdma2d.Init.ColorMode    = DMA2D_RGB565;
  }
  else
  { /* ARGB8888 format */
    hdma2d.Init.ColorMode    = DMA2D_ARGB8888;
  }
  hdma2d.Init.OutputOffset = 0;      
  
  hdma2d.Instance = DMA2D;
  
  /* DMA2D Initialization */
  if(HAL_DMA2D_Init(&hdma2d) == HAL_OK) 
  {
    if(HAL_DMA2D_ConfigLayer(&hdma2d, ActiveLayer) == HAL_OK) 
    {
      if (HAL_DMA2D_Start(&hdma2d, BackgroundImageAddress, (uint32_t)hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress, LCD_GetXSize(), LCD_GetYSize()) == HAL_OK)
      {
        /* Polling For DMA transfer */  
        HAL_DMA2D_PollForTransfer(&hdma2d, 10);
      }
    }
  } 
}


/**
  * @brief  LCD에 1픽셀 표시
  * @param  Xpos: X 좌표
  * @param  Ypos: Y 좌표
  * @param  RGB_Code: RGB565 색상
  * @retval None
  */
void LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint16_t RGB_Code)
{
  *(__IO uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos))) = (uint16_t)RGB_Code;
}

/**
  * @brief  LCD 그림 표시
  * @param  Xpos: 시작 X 위치
  * @param  Ypos: 시작 Y 위치
  * @param  pAddress: 그림 시작 주소
  * @param  Xsize: 표시할 그림 가로축 크기 px
  * @param  Ysize: 표시할 그림 세로축 크기 px
  * @param  Color_Alpha0: 표시 할 그림의 뒷 배경 제거 색상. RGB565. 검정색 배경은 제거 불가능하며 0 입력 시 배경 제거 없이 출력
  * @retval None
  */
void LCD_DrawPicture(uint16_t Xpos, uint16_t Ypos, uint16_t *pAddress, uint16_t Xsize, uint16_t Ysize, uint16_t Color_Alpha0)
{
  uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));

  //for( int iY=0; iY<Ysize; iY++)
  //{
  //	memcpy ((void*)&lcdBuffer[480*iY],(void*)&pAddress[iY*Xsize], Xsize * 2);
  //}
  for (int cnt = 0; cnt < 2; cnt++)
  {
    if (Color_Alpha0 != 0)
    {
      for (int iY = 0; iY < Ysize; iY++)
      {
        for (int iX = 0; iX < Xsize; iX++)
        {
          if (pAddress[Xsize * iY + iX] != Color_Alpha0)
          {
            lcdBuffer[LCD_GetXSize() * iY + iX] = pAddress[Xsize * iY + iX];
            //LCD_DrawPixel(Xpos + iX, Ypos * LCD_GetXSize(), pAddress[Xsize * iY + iX]);
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
          lcdBuffer[LCD_GetXSize() * iY + iX] = pAddress[Xsize * iY + iX];
					//LCD_DrawPixel(Xpos + iX, Ypos + (LCD_GetXSize()*iY), pAddress[Xsize * iY + iX]);
        }
      }
    }
  }
}

/**
  * @brief  LCD 그림 삭제
  * @param  Xpos: 시작 X 위치
  * @param  Ypos: 시작 Y 위치
  * @param  Xsize: 삭제 할 그림 가로축 크기 px
  * @param  Ysize: 삭제 할 그림 세로축 크기 px
  * @retval None
  */
void LCD_ErasePicture(uint16_t Xpos, uint16_t Ypos, uint16_t *pAddress, uint16_t Xsize, uint16_t Ysize)
{
  uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));

  for (int iY = 0; iY < Ysize; iY++)
  {
    for (int iX = 0; iX < Xsize; iX++)
    {
      //lcdBuffer[LCD_GetXSize() * iY + iX] = DrawProp[ActiveLayer].pBackImage[(Ypos*LCD_GetXSize() + (Xpos))+ LCD_GetXSize() * iY + iX];
      //lcdBuffer[LCD_GetXSize() * iY + iX] = fish[(Ypos*LCD_GetXSize() + (Xpos))+ LCD_GetXSize() * iY + iX];
      for(int delay=0; delay<100; delay++);
    }
  }
}

/**
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

/**
  * @brief  LCD에 문자 그리기
  * @param  Xpos: 가로축 시작 위치
  * @param  Ypos: 세로축 시작 위치
  * @param  c: 폰트의 해당 문자 시작 포인터 주소
  * @retval None
  */
static void DrawChar(uint16_t Xpos, uint16_t Ypos, const uint8_t *c)
{
  uint32_t i = 0, j = 0;
  uint16_t height, width;
  uint8_t offset;
  uint8_t *pchar;
  uint32_t line;
  uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));

//  height = DrawProp[ActiveLayer].pFont->Height;
//  width = DrawProp[ActiveLayer].pFont->Width;

  offset = 8 * ((width + 7) / 8) - width;

  for (i = 0; i < height; i++)
  {
    pchar = ((uint8_t *)c + (width + 7) / 8 * i);

    switch (((width + 7) / 8))
    {

    case 1:
      line = pchar[0];
      break;

    case 2:
      line = (pchar[0] << 8) | pchar[1];
      break;

    case 3:
    default:
      line = (pchar[0] << 16) | (pchar[1] << 8) | pchar[2];
      break;
    }

    for (j = 0; j < width; j++)
    {
      if (line & (1 << (width - j + offset - 1)))
      {
        LCD_DrawPixel((Xpos + j), Ypos, DrawProp[ActiveLayer].TextColor);
      }
      else
      {
        //BSP_LCD_DrawPixel((Xpos + j), Ypos, fish[Ypos*LCD_GetXSize() + (Xpos + j)]);
      }
    }
    Ypos++;
  }
}

/**
  * @brief  LCD에 문자 표시
  * @param  Xpos: X 위치
  * @param  Ypos: Y 위치
  * @param  Ascii: 표시 할 문자 - 아스키 코드표 참조 0x20~0x7E
  * @retval None
  */
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii)
{
//  DrawChar(Xpos, Ypos, &DrawProp[ActiveLayer].pFont->table[(Ascii-' ') * DrawProp[ActiveLayer].pFont->Height * ((DrawProp[ActiveLayer].pFont->Width + 7) / 8)]);
}

/**
  * @brief  LCD에 문자열 표시
  * @param  Xpos: X 위치 (pixel)
  * @param  Ypos: Y 위치 (pixel)   
  * @param  Text: 표시 할 문자열 시작 메모리 주소 
  * @retval None
  */
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *Text)
{
  uint16_t ref_column = 1, i = 0;
  uint32_t size = 0; 
  uint8_t  *ptr = Text;
  

  while (*ptr++) size ++ ;  /* 텍스트 크기 */
  

  ref_column = Xpos;  /* 1라인의 크기 */
  

  if ((ref_column < 1) || (ref_column >= 0x8000))  /* 표시 할 시작 메모리에서의 위치 */
  {
    ref_column = 1;
  }

  /* Send the string character by character on LCD */
  //while ((*Text != 0) & (((LCD_GetXSize() - (i*DrawProp[ActiveLayer].pFont->Width)) & 0xFFFF) >= DrawProp[ActiveLayer].pFont->Width))
  //{
  //  /* Display one character on LCD */
  //  LCD_DisplayChar(ref_column, Ypos, *Text);
  //  /* Decrement the column position by 16 */
  //  ref_column += DrawProp[ActiveLayer].pFont->Width;
  //  /* Point on the next character */
  //  Text++;
  //  i++;
  //}  
}

/**
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
  * @brief  LCD에 그림 숫자 출력
  * @param  Xpos: X 위치 (pixel)
  * @param  Ypos: Y 위치 (pixel)   
  * @param  Num: 출력 할 숫자 (17x23 px 크기, 표시 문자 : 0~9 :)
  * @retval None
  */
void LCD_DisplayNumPicture(uint16_t Xpos, uint16_t Ypos, uint8_t Num)
{
  #define XTotal 187
  #define Xsize 17
  #define Ysize 23
	
  uint16_t *lcdBuffer = (uint16_t *)(hLtdcHandler.LayerCfg[ActiveLayer].FBStartAdress + (2 * (Ypos * LCD_GetXSize() + Xpos)));

  for (int iY = 0; iY < Ysize; iY++)
  {
    for (int iX = 0; iX < Xsize; iX++)
    {
      if (IMG_17x23NUM[XTotal * iY + iX + Xsize * Num] != 0xFFFF)
      {
        lcdBuffer[LCD_GetXSize() * iY + iX] = IMG_17x23NUM[XTotal * iY + iX + Xsize * Num];
      }
    }
  }
}

/**
  * @}
  */
