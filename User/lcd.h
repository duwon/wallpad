#ifndef LCD_H__
#define LCD_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "main.h"

#define MAX_LAYER_NUMBER       ((uint32_t)2)
#define LTDC_ACTIVE_LAYER	     ((uint32_t)0) /* Layer 0 */
#define LCD_FB_START_ADDRESS       ((uint32_t)0xC0000000)

#define LCD_OK                 ((uint8_t)0x00)
#define LCD_ERROR              ((uint8_t)0x01)
#define LCD_TIMEOUT            ((uint8_t)0x02)

#define LCD_HEIGHT             (272)
#define LCD_WIDTH              (480)

/** 
  * @brief  LCD color  
  */ 
#define LCD_COLOR_BLUE          ((uint32_t)0xFF0000FF)
#define LCD_COLOR_GREEN         ((uint32_t)0xFF00FF00)
#define LCD_COLOR_RED           ((uint32_t)0xFFFF0000)
#define LCD_COLOR_CYAN          ((uint32_t)0xFF00FFFF)
#define LCD_COLOR_MAGENTA       ((uint32_t)0xFFFF00FF)
#define LCD_COLOR_YELLOW        ((uint32_t)0xFFFFFF00)
#define LCD_COLOR_LIGHTBLUE     ((uint32_t)0xFF8080FF)
#define LCD_COLOR_LIGHTGREEN    ((uint32_t)0xFF80FF80)
#define LCD_COLOR_LIGHTRED      ((uint32_t)0xFFFF8080)
#define LCD_COLOR_LIGHTCYAN     ((uint32_t)0xFF80FFFF)
#define LCD_COLOR_LIGHTMAGENTA  ((uint32_t)0xFFFF80FF)
#define LCD_COLOR_LIGHTYELLOW   ((uint32_t)0xFFFFFF80)
#define LCD_COLOR_DARKBLUE      ((uint32_t)0xFF000080)
#define LCD_COLOR_DARKGREEN     ((uint32_t)0xFF008000)
#define LCD_COLOR_DARKRED       ((uint32_t)0xFF800000)
#define LCD_COLOR_DARKCYAN      ((uint32_t)0xFF008080)
#define LCD_COLOR_DARKMAGENTA   ((uint32_t)0xFF800080)
#define LCD_COLOR_DARKYELLOW    ((uint32_t)0xFF808000)
#define LCD_COLOR_WHITE         ((uint32_t)0xFFFFFFFF)
#define LCD_COLOR_LIGHTGRAY     ((uint32_t)0xFFD3D3D3)
#define LCD_COLOR_GRAY          ((uint32_t)0xFF808080)
#define LCD_COLOR_DARKGRAY      ((uint32_t)0xFF404040)
#define LCD_COLOR_BLACK         ((uint32_t)0xFF000000)
#define LCD_COLOR_BROWN         ((uint32_t)0xFFA52A2A)
#define LCD_COLOR_ORANGE        ((uint32_t)0xFFFFA500)
#define LCD_COLOR_TRANSPARENT   ((uint32_t)0xFF000000)

typedef struct 
{ 
  uint32_t TextColor; 
  uint32_t BackColor;  
  //sFONT    *pFont;
  uint16_t *pBackImage;
}LCD_DrawPropTypeDef;   

void LCD_Init(void);
void LCD_LayerInit(uint16_t LayerIndex, uint32_t FB_Address);
void LCD_SelectLayer(uint32_t LayerIndex);
void LCD_SetXSize(uint32_t imageWidthPixels);
void LCD_SetYSize(uint32_t imageHeightPixels);
void LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency);
void LCD_DrawPicture(uint16_t Xpos, uint16_t Ypos, uint32_t ImageAddress, uint16_t Xsize, uint16_t Ysize, uint16_t Color_Alpha0);
void LCD_ErasePicture(uint16_t Xpos, uint16_t Ypos, uint16_t Xsize, uint16_t Ysize);

void LCD_SetTextColor(uint32_t Color);
void LCD_SetBackColor(uint32_t Color);
void LCD_SetBackImage(uint32_t BackgroundImageAddress);
void LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);
void LCD_DisplayString(uint16_t Xpos, uint16_t Ypos, uint8_t *Text);
void LCD_Clear(uint32_t Color);

void LCD_DisplayNumPicture(uint16_t Xpos, uint16_t Ypos, uint8_t Num);

#ifdef __cplusplus
}
#endif


#endif
