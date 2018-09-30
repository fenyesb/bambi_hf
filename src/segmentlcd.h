/**************************************************************************/
#ifndef SEGMENTLCD_H
#define SEGMENTLCD_H

#include "segmentlcdconfig.h"

/***************************************************************************/
/***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define SEGMENT_LCD_BLOCK_MODE_BLANK          0
#define SEGMENT_LCD_BLOCK_MODE_FILL           1
#define SEGMENT_LCD_BLOCK_MODE_OUTLINE        2
#define SEGMENT_LCD_BLOCK_MODE_OUTLINE_FILL   3

#define SEGMENT_LCD_NUM_BLOCK_COLUMNS         7

/*******************************************************************************
 ********************************   ENUMS   ************************************
 ******************************************************************************/

typedef enum
{
  segmentLCDBlockModeBlank = SEGMENT_LCD_BLOCK_MODE_BLANK,
  segmentLCDBlockModeFill = SEGMENT_LCD_BLOCK_MODE_FILL,
  segmentLCDBlockModeOutline = SEGMENT_LCD_BLOCK_MODE_OUTLINE,
  segmentLCDBlockModeOutlineFill = SEGMENT_LCD_BLOCK_MODE_OUTLINE_FILL,
} SegmentLCD_BlockMode_TypeDef;

/*******************************************************************************
 *******************************  FUNCTIONS  ***********************************
 ******************************************************************************/

/* Regular functions */
void SegmentLCD_AllOff(void);
void SegmentLCD_AllOn(void);
void SegmentLCD_AlphaNumberOff(void);
void SegmentLCD_ARing(int anum, int on);
void SegmentLCD_Battery(int batteryLevel);
void SegmentLCD_Block(
    SegmentLCD_BlockMode_TypeDef topMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS],
    SegmentLCD_BlockMode_TypeDef botMode[SEGMENT_LCD_NUM_BLOCK_COLUMNS]);
void SegmentLCD_Disable(void);
void SegmentLCD_EnergyMode(int em, int on);
void SegmentLCD_Init(bool useBoost);
void SegmentLCD_LowerHex( uint32_t num );
void SegmentLCD_LowerNumber( int num );
void SegmentLCD_Number(int value);
void SegmentLCD_NumberOff(void);
void SegmentLCD_Symbol(lcdSymbol s, int on);
void SegmentLCD_UnsignedHex(uint16_t value);
void SegmentLCD_Write(const char *string);

#ifdef __cplusplus
}
#endif

#endif
