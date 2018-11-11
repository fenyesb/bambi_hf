#pragma once

#include "common.h"

//A haj� sebess�g�nek szab�lyoz�sa a pontsz�m alapj�n
uint32_t get_delay_ticks(uint16_t score);

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void SysTick_Handler();

/***************************************************************************//**
 * @brief Delays number of msTick Systicks (typically 1 ms)
 * @param dlyTicks Number of ticks to delay
 ******************************************************************************/
void Delay(uint32_t dlyTicks);
