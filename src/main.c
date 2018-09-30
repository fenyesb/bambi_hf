/***************************************************************************//**
 * @file
 * @brief Simple LED Blink Demo for EFM32GG_STK3700
 * @version 5.2.2
 *******************************************************************************
 * # License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "segmentlcd_spec.h"

////////////////////////////////////////////

typedef struct {
	int8_t x, y;
} vector2_t;

typedef struct {
	vector2_t position, velocity;
} player_t;

player_t ship;

#define ASTEROID_COUNT 3

vector2_t asteroids[ASTEROID_COUNT];

uint16_t score = 0;

////////////////////////////////////////////


volatile uint32_t msTicks; /* counts 1ms timeTicks */

void Delay(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void)
{
  msTicks++;       /* increment counter necessary in Delay()*/
}

/***************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks)
{
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks) ;
}

void delay()
{
	Delay(100);
}

void lcd_demo()
{

	  SegmentLCD_Init(false);

	  SegmentLCD_SegmentData_TypeDef segmentField[7];

	  /* Infinite loop */
	  while (1) {

		  // Clear all segments
		  for (uint8_t p = 0; p < 7; p++) {
			  segmentField[p].raw = 0;
			  displaySegmentField(segmentField);
		  }

		  // Turn on all segments one-by-one
		  // Only one segment is turned on at any given time
		  // Using 14 bit binary value
		  for (uint8_t p = 0; p < 7; p++) {
			  for (uint8_t s = 0; s < 15; s++) {
				  segmentField[p].raw = 1 << s;
				  displaySegmentField(segmentField);
				  delay();
			  }
		  }

		  // Turn on all segments one-by-one
		  // All the previous segments are left turned on
		  // Using dedicated bit field values
		  for (uint8_t p = 0; p < 7; p++) {
			  segmentField[p].a = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].b = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].c = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].d = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].e = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].f = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].g = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].h = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].j = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].k = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].m = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].n = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].p = 1;
			  displaySegmentField(segmentField);
			  delay();

			  segmentField[p].q = 1;
			  displaySegmentField(segmentField);
			  delay();
		  }
	  }
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();

  /* If first word of user data page is non-zero, enable eA Profiler trace */
  BSP_TraceProfilerSetup();

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }
CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);
  /* Initialize LED driver */
  BSP_LedsInit();
  BSP_LedSet(0);
  lcd_demo();
  /* Infinite blink loop */
  uint8_t counter = 0;
  while (1) {
	BSP_LedsSet(counter&0b11);
    Delay(1000);
    counter++;
  }
}
