/***************************************************************************/
#include "em_lcd.h"
#if defined(LCD_COUNT) && (LCD_COUNT > 0)
#include "em_assert.h"
#include "em_bus.h"

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void LCD_Init(const LCD_Init_TypeDef *lcdInit)
{
  uint32_t dispCtrl = LCD->DISPCTRL;

  EFM_ASSERT(lcdInit != (void *) 0);

  /* Disable controller before reconfiguration */
  LCD_Enable(false);

  /* Make sure we don't touch other bit fields (i.e. voltage boost) */
  dispCtrl &= ~(0
#if defined(LCD_DISPCTRL_MUXE)
                | _LCD_DISPCTRL_MUXE_MASK
#endif
                | _LCD_DISPCTRL_MUX_MASK
                | _LCD_DISPCTRL_BIAS_MASK
                | _LCD_DISPCTRL_WAVE_MASK
                | _LCD_DISPCTRL_VLCDSEL_MASK
                | _LCD_DISPCTRL_CONCONF_MASK);

  /* Configure controller according to initialization structure */
  dispCtrl |= lcdInit->mux; /* also configures MUXE */
  dispCtrl |= lcdInit->bias;
  dispCtrl |= lcdInit->wave;
  dispCtrl |= lcdInit->vlcd;
  dispCtrl |= lcdInit->contrast;

  /* Update display controller */
  LCD->DISPCTRL = dispCtrl;

  /* Enable controller if wanted */
  if (lcdInit->enable)
  {
    LCD_Enable(true);
  }
}


/***************************************************************************/
void LCD_VLCDSelect(LCD_VLCDSel_TypeDef vlcd)
{
  uint32_t dispctrl = LCD->DISPCTRL;

  /* Select VEXT or VDD */
  dispctrl &= ~_LCD_DISPCTRL_VLCDSEL_MASK;
  switch (vlcd)
  {
    case lcdVLCDSelVExtBoost:
      dispctrl |= LCD_DISPCTRL_VLCDSEL_VEXTBOOST;
      break;
    case lcdVLCDSelVDD:
      dispctrl |= LCD_DISPCTRL_VLCDSEL_VDD;
      break;
    default:
      break;
  }

  LCD->DISPCTRL = dispctrl;
}


/***************************************************************************/
void LCD_UpdateCtrl(LCD_UpdateCtrl_TypeDef ud)
{
  LCD->CTRL = (LCD->CTRL & ~_LCD_CTRL_UDCTRL_MASK) | ud;
}


/***************************************************************************/
void LCD_FrameCountInit(const LCD_FrameCountInit_TypeDef *fcInit)
{
  uint32_t bactrl = LCD->BACTRL;

  EFM_ASSERT(fcInit != (void *) 0);

  /* Verify FC Top Counter to be within limits */
  EFM_ASSERT(fcInit->top < 64);

  /* Reconfigure frame count configuration */
  bactrl &= ~(_LCD_BACTRL_FCTOP_MASK
              | _LCD_BACTRL_FCPRESC_MASK);
  bactrl |= (fcInit->top << _LCD_BACTRL_FCTOP_SHIFT);
  bactrl |= fcInit->prescale;

  /* Set Blink and Animation Control Register */
  LCD->BACTRL = bactrl;

  LCD_FrameCountEnable(fcInit->enable);
}


/***************************************************************************/
void LCD_AnimInit(const LCD_AnimInit_TypeDef *animInit)
{
  uint32_t bactrl = LCD->BACTRL;

  EFM_ASSERT(animInit != (void *) 0);

  /* Set Animation Register Values */
  LCD->AREGA = animInit->AReg;
  LCD->AREGB = animInit->BReg;

  /* Configure Animation Shift and Logic */
  bactrl &= ~(_LCD_BACTRL_AREGASC_MASK
              | _LCD_BACTRL_AREGBSC_MASK
              | _LCD_BACTRL_ALOGSEL_MASK);

  bactrl |= (animInit->AShift << _LCD_BACTRL_AREGASC_SHIFT);
  bactrl |= (animInit->BShift << _LCD_BACTRL_AREGBSC_SHIFT);
  bactrl |= animInit->animLogic;

#if defined(LCD_BACTRL_ALOC)
  bactrl &= ~(_LCD_BACTRL_ALOC_MASK);

  if(animInit->startSeg == 0)
  {
    bactrl |= LCD_BACTRL_ALOC_SEG0TO7;
  }
  else if(animInit->startSeg == 8)
  {
    bactrl |= LCD_BACTRL_ALOC_SEG8TO15;
  }
#endif

  /* Reconfigure */
  LCD->BACTRL = bactrl;

  /* Enable */
  LCD_AnimEnable(animInit->enable);
}


/***************************************************************************/
void LCD_SegmentRangeEnable(LCD_SegmentRange_TypeDef segmentRange, bool enable)
{
  if (enable)
  {
    LCD->SEGEN |= segmentRange;
  }
  else
  {
    LCD->SEGEN &= ~((uint32_t)segmentRange);
  }
}


/***************************************************************************/
void LCD_SegmentSet(int com, int bit, bool enable)
{
#if defined(_LCD_SEGD7L_MASK)
  /* Tiny and Giant Family supports up to 8 COM lines */
  EFM_ASSERT(com < 8);
#else
  /* Gecko Family supports up to 4 COM lines */
  EFM_ASSERT(com < 4);
#endif

#if defined(_LCD_SEGD0H_MASK)
  EFM_ASSERT(bit < 40);
#else
  /* Tiny Gecko Family supports only "low" segment registers */
  EFM_ASSERT(bit < 32);
#endif

  /* Use bitband access for atomic bit set/clear of segment */
  switch (com)
  {
    case 0:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD0L), bit, enable);
      }
#if defined(_LCD_SEGD0H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD0H), bit, enable);
      }
#endif
      break;
    case 1:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD1L), bit, enable);
      }
#if defined(_LCD_SEGD1H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD1H), bit, enable);
      }
#endif
      break;
    case 2:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD2L), bit, enable);
      }
#if defined(_LCD_SEGD2H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD2H), bit, enable);
      }
#endif
      break;
    case 3:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD3L), bit, enable);
      }
#if defined(_LCD_SEGD3H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD3H), bit, enable);
      }
#endif
      break;
#if defined(_LCD_SEGD4L_MASK)
    case 4:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD4L), bit, enable);
      }
#if defined(_LCD_SEGD4H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD4H), bit, enable);
      }
#endif
      break;
#endif
#if defined(_LCD_SEGD5L_MASK)
    case 5:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD5L), bit, enable);
      }
#if defined(_LCD_SEGD5H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD5H), bit, enable);
      }
#endif
      break;
#endif
    case 6:
#if defined(_LCD_SEGD6L_MASK)
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD6L), bit, enable);
      }
#if defined(_LCD_SEGD6H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD6H), bit, enable);
      }
#endif
      break;
#endif
#if defined(_LCD_SEGD7L_MASK)
    case 7:
      if (bit < 32)
      {
        BUS_RegBitWrite(&(LCD->SEGD7L), bit, enable);
      }
#if defined(_LCD_SEGD7H_MASK)
      else
      {
        bit -= 32;
        BUS_RegBitWrite(&(LCD->SEGD7H), bit, enable);
      }
#endif
      break;
#endif

    default:
      EFM_ASSERT(0);
      break;
  }
}


/***************************************************************************/
void LCD_SegmentSetLow(int com, uint32_t mask, uint32_t bits)
{
  uint32_t segData;

  /* Maximum number of com lines */
#if defined(_LCD_SEGD7L_MASK)
  EFM_ASSERT(com < 8);
#else
  /* Gecko Family supports up to 4 COM lines */
  EFM_ASSERT(com < 4);
#endif

  switch (com)
  {
    case 0:
      segData     = LCD->SEGD0L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD0L = segData;
      break;
    case 1:
      segData     = LCD->SEGD1L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD1L = segData;
      break;
    case 2:
      segData     = LCD->SEGD2L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD2L = segData;
      break;
    case 3:
      segData     = LCD->SEGD3L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD3L = segData;
      break;
#if defined(_LCD_SEGD4L_MASK)
    case 4:
      segData     = LCD->SEGD4L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD4L = segData;
      break;
#endif
#if defined(_LCD_SEGD5L_MASK)
    case 5:
      segData     = LCD->SEGD5L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD5L = segData;
      break;
#endif
#if defined(_LCD_SEGD6L_MASK)
    case 6:
      segData     = LCD->SEGD6L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD6L = segData;
      break;
#endif
#if defined(_LCD_SEGD7L_MASK)
    case 7:
      segData     = LCD->SEGD7L;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD7L = segData;
      break;
#endif
    default:
      EFM_ASSERT(0);
      break;
  }
}


#if defined(_LCD_SEGD0H_MASK)
/***************************************************************************/
void LCD_SegmentSetHigh(int com, uint32_t mask, uint32_t bits)
{
  uint32_t segData;

#if defined(_LCD_SEGD7H_MASK)
  EFM_ASSERT(com < 8);
#else
  EFM_ASSERT(com < 4);
#endif

  /* Maximum number of com lines */
  switch (com)
  {
    case 0:
      segData     = LCD->SEGD0H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD0H = segData;
      break;
    case 1:
      segData     = LCD->SEGD1H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD1H = segData;
      break;
    case 2:
      segData     = LCD->SEGD2H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD2H = segData;
      break;
    case 3:
      segData     = LCD->SEGD3H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD3H = segData;
      break;
#if defined(_LCD_SEGD4H_MASK)
    case 4:
      segData     = LCD->SEGD4H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD4H = segData;
      break;
#endif
#if defined(_LCD_SEGD5H_MASK)
    case 5:
      segData     = LCD->SEGD5H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD5H = segData;
      break;
#endif
#if defined(_LCD_SEGD6H_MASK)
    case 6:
      segData     = LCD->SEGD6H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD6H = segData;
      break;
#endif
#if defined(_LCD_SEGD7H_MASK)
    case 7:
      segData     = LCD->SEGD7H;
      segData    &= ~(mask);
      segData    |= (mask & bits);
      LCD->SEGD7H = segData;
      break;
#endif
    default:
      break;
  }
}
#endif

/***************************************************************************/
void LCD_ContrastSet(int level)
{
  EFM_ASSERT(level < 32);

  LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_CONLEV_MASK)
                  | (level << _LCD_DISPCTRL_CONLEV_SHIFT);
}


/***************************************************************************/
void LCD_VBoostSet(LCD_VBoostLevel_TypeDef vboost)
{
  /* Reconfigure Voltage Boost */
  LCD->DISPCTRL = (LCD->DISPCTRL & ~_LCD_DISPCTRL_VBLEV_MASK) | vboost;
}


#if defined(LCD_CTRL_DSC)
/***************************************************************************/
void LCD_BiasSegmentSet(int segmentLine, int biasLevel)
{
  int               biasRegister;
  int               bitShift;
  volatile uint32_t *segmentRegister;

#if !defined(_LCD_SEGD0H_MASK)
  EFM_ASSERT(segmentLine < 20);

  /* Bias config for 8 segment lines per SEGDnL register */
  biasRegister = segmentLine / 8;
  bitShift     = (segmentLine % 8) * 4;

  switch (biasRegister)
  {
    case 0:
      segmentRegister = &LCD->SEGD0L;
      break;
    case 1:
      segmentRegister = &LCD->SEGD1L;
      break;
    case 2:
      segmentRegister = &LCD->SEGD2L;
      break;
    case 3:
      segmentRegister = &LCD->SEGD3L;
      break;
    default:
      segmentRegister = (uint32_t *)0x00000000;
      EFM_ASSERT(0);
      break;
  }
#else
  EFM_ASSERT(segmentLine < 40);

  /* Bias config for 10 segment lines per SEGDn L+H registers */
  biasRegister = segmentLine / 10;
  bitShift     = (segmentLine % 10) * 4;

  switch (biasRegister)
  {
    case 0:
      if (bitShift < 32)
      {
        segmentRegister = &LCD->SEGD0L;
      }
      else
      {
        segmentRegister = &LCD->SEGD0H;
        bitShift       -= 32;
      }
      break;
    case 1:
      if (bitShift < 32)
      {
        segmentRegister = &LCD->SEGD1L;
      }
      else
      {
        segmentRegister = &LCD->SEGD1H;
        bitShift       -= 32;
      }
      break;
    case 2:
      if (bitShift < 32)
      {
        segmentRegister = &LCD->SEGD2L;
      }
      else
      {
        segmentRegister = &LCD->SEGD1H;
        bitShift       -= 32;
      }
      break;
    case 3:
      if (bitShift < 32)
      {
        segmentRegister = &LCD->SEGD3L;
      }
      else
      {
        segmentRegister = &LCD->SEGD3H;
        bitShift       -= 32;
      }
      break;
    default:
      segmentRegister = (uint32_t *)0x00000000;
      EFM_ASSERT(0);
      break;
  }
#endif

  /* Configure new bias setting */
  *segmentRegister = (*segmentRegister & ~(0xF << bitShift)) | (biasLevel << bitShift);
}
#endif


#if defined(LCD_CTRL_DSC)
/***************************************************************************/
void LCD_BiasComSet(int comLine, int biasLevel)
{
  int bitShift;
  EFM_ASSERT(comLine < 8);

  bitShift    = comLine * 4;
  LCD->SEGD4L = (LCD->SEGD4L & ~(0xF << bitShift)) | (biasLevel << bitShift);
}
#endif

#endif /* defined(LCD_COUNT) && (LCD_COUNT > 0) */
