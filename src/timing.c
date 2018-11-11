#include "timing.h"

//A hajó sebességének szabályozása a pontszám alapján
uint32_t get_delay_ticks(uint16_t score) {
	uint32_t ticks = 2000 - score * 20;
	if (ticks < 1000) {
		ticks = 1000;
	}
	return ticks;
}

void SysTick_Handler() {
	msTicks++; /* increment counter necessary in Delay()*/
}

/***************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks) {
	uint32_t curTicks;

	curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks)
		;
}
