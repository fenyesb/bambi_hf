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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "segmentlcd_spec.h"

/////////////////////////////////////////////////////////////
////////////////////Glob�lis v�ltoz�k////////////////////////

typedef enum {
	DIRECTION_UP, DIRECTION_DOWN, DIRECTION_RIGHT
} direction_t;

typedef enum {
	TURN_LEFT, TURN_RIGHT, TURN_NONE
} turn_t;

typedef enum {
	SEGMENT_TOP,
	SEGMENT_MIDDLE,
	SEGMENT_BOTTOM,
	SEGMENT_UPPER,
	SEGMENT_LOWER,
	SEGMENT_INVALID
} segment_t;

typedef struct {
	int8_t x, y;
} vector2_t;

typedef struct {
	vector2_t position;
	direction_t direction;
	uint16_t score;
} player_t;

player_t ship;

#define ASTEROID_COUNT 3 //ha ezt a sz�mot 18-n�l nagyobbra �ll�tjuk, az meg�li az aszteroida koordin�ta gener�l� f�ggv�nyt,
// valamint min�l nagyobb ez a sz�m ann�l val�sz�n�bb, hogy v�gigvihetetlen p�lya gener�l�dik
vector2_t asteroids[ASTEROID_COUNT];

SegmentLCD_SegmentData_TypeDef segmentField[7];

#define BUTTON_PORT gpioPortB
#define BUTTON_LEFT_PIN 9 //PB0
#define BUTTON_RIGHT_PIN 10 //PB1

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void Delay(uint32_t dlyTicks);

/***************************************************************************//**
 * @brief SysTick_Handler
 * Interrupt Service Routine for system tick counter
 ******************************************************************************/
void SysTick_Handler(void) {
	msTicks++; /* increment counter necessary in Delay()*/
}

volatile bool key_pressed_left;
volatile bool key_pressed_right;

bool keypress_any() {
	return key_pressed_left || key_pressed_right;
}

typedef struct {
	bool left, right;
} keypress_t;

void keypress_reset() {
	key_pressed_left = false;
	key_pressed_right = false;
}

keypress_t keypress_read_and_clear()
{
	NVIC_DisableIRQ(GPIO_EVEN_IRQn);
	NVIC_DisableIRQ(GPIO_ODD_IRQn);
	keypress_t keys;
	keys.left = key_pressed_left;
	keys.right = key_pressed_right;
	keypress_reset();
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);
	return keys;
}

turn_t get_turn(keypress_t keys) {
	if (keys.left) {
		return TURN_LEFT;
	} else if (keys.right) {
		return TURN_RIGHT;
	} else {
		return TURN_NONE;
	}
}

void GPIO_EVEN_IRQHandler() {
	GPIO->IFC = GPIO->IF; //clear interrupt flag
	key_pressed_left = false;
	key_pressed_right = true;
}

void GPIO_ODD_IRQHandler() {
	GPIO->IFC = GPIO->IF; //clear interrupt flag
	key_pressed_left = true;
	key_pressed_right = false;
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

/////////////////////////////////////////////////////////////////////
////////////////Aszteroida/P�lya gener�l�s///////////////////////////

int random(int min_num, int max_num) {
	int result = 0, low_num = 0, hi_num = 0;

	if (min_num < max_num) {
		low_num = min_num;
		hi_num = max_num + 1; // include max_num in output
	} else {
		low_num = max_num + 1; // include max_num in output
		hi_num = min_num;
	}

	result = (rand() % (hi_num - low_num)) + low_num;
	return result;
}

void reset_game() {
	ship.position.x = 0;
	ship.position.y = SEGMENT_MIDDLE;
	ship.direction = DIRECTION_RIGHT;
	for (int32_t i = 0; i < ASTEROID_COUNT; i++) {
		asteroids[i].x = -1;
		asteroids[i].y = SEGMENT_INVALID;
	}
}

void reset_score() {
	ship.score = 0;
	SegmentLCD_Number(ship.score);
}

void create_asteroids() {
	int8_t x, y;
	x = random(1, 6);/*az els� 14-szegmens kijelz�re nem tesz�nk aszteroid�t, mert felesleges 1: a haj� mell� ker�l, ekkor egyel kevesebb aszteroid�t kell kiker�lni 2: a haj�val egy mez�re ker�l, ekkor azonnal v�ge a j�t�knak*/
	y = random(0, 4);/*kijelz�nk�nt csak 5 szegmenst(k�z�ps� kett�t egynek v�ve) haszn�lunk, a kijelz�sn�l az itt sorsolt sz�mot dek�doljuk*/
	asteroids[0].x = x;
	asteroids[0].y = y;

	int a_count[7] = { 0 }; //az egy kijelz�n l�v� aszteroid�k sz�ma
	a_count[x]++;

	for (int i = 1; i < ASTEROID_COUNT; i++) {
		x = random(1, 6);
		y = random(0, 4);

		while (a_count[x] == 3) {
			x = random(1, 6);
		}

		if (a_count[x] == 0) {
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		} else if (a_count[x] == 1) {
			int z = 0;
			while (!(asteroids[z].x == x)) {
				z++;
			}
			while (asteroids[z].y == y) {
				y = random(0, 4);
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		} else if (a_count[x] == 2) {
			int z = 0;
			int ny[2];
			for (int k = 0; k < i; k++) {
				if (asteroids[k].x == x) {
					ny[z] = asteroids[k].y;
					z++;
				}
			}
			while ((y == ny[0]) || (y == ny[1])) {
				y = random(0, 4);
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		}
	}
}

///////////////////////////////////////////////////////////////////
/////////////////////Megjelen�t�s//////////////////////////////////
//a megadott vektor  koordin�t�inak megfelel� szegmenst bekapcsolja
void display_segment(vector2_t vector) {
	int x = vector.x;
	int y = vector.y;
	switch (y) {
	case SEGMENT_TOP:
		segmentField[x].a = 1;
		break;
	case SEGMENT_MIDDLE:
		segmentField[x].g = 1;
		segmentField[x].m = 1;
		break;
	case SEGMENT_BOTTOM:
		segmentField[x].d = 1;
		break;
	case SEGMENT_UPPER:
		segmentField[x].f = 1;
		break;
	case SEGMENT_LOWER:
		segmentField[x].e = 1;
		break;
	}
}

void update_display() {
//minden szegmens t�rl�se
	for (int32_t i = 0; i < 7; i++) {
		segmentField[i].raw = 0;
	}

//haj� kijelz�se
	display_segment(ship.position);

//aszteroid�k kijelz�se
	for (int32_t i = 0; i < ASTEROID_COUNT; i++) {
		display_segment(asteroids[i]);
	}

	displaySegmentField(segmentField);
}

//be�ll�tja a LED-eket annak megfelel�en, hogy merre fog fordulni a haj�
void turn_signal(turn_t turn) {
	return;
	BSP_LedClear(0);
	BSP_LedClear(1);
	switch (turn) {
	case TURN_LEFT:
		BSP_LedSet(1);
		break;
	case TURN_RIGHT:
		BSP_LedSet(0);
		break;
	case TURN_NONE:
		break;
	}
}

///////////////////////////////////////////////////////
///////////////////////Mozg�s/////////////////////////

bool is_any_key_pressed() {
	bool is_left_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_LEFT_PIN);
	bool is_right_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_RIGHT_PIN);
	return is_left_pressed || is_right_pressed;
}

void wait_for_player() {
	while (!is_any_key_pressed())
		;
	while (is_any_key_pressed())
		;
	while (!is_any_key_pressed())
		;
}

void turn_ship(turn_t turn) {
	switch (turn) {
	case TURN_LEFT:
		switch (ship.direction) {
		case DIRECTION_UP:
			break;
		case DIRECTION_RIGHT:
			ship.direction = DIRECTION_UP;
			break;
		case DIRECTION_DOWN:
			ship.direction = DIRECTION_RIGHT;
			break;
		}
		break;
	case TURN_RIGHT:
		switch (ship.direction) {
		case DIRECTION_UP:
			ship.direction = DIRECTION_RIGHT;
			break;
		case DIRECTION_RIGHT:
			ship.direction = DIRECTION_DOWN;
			break;
		case DIRECTION_DOWN:
			break;
		}
		break;
	case TURN_NONE:
		break;
	}
}

void move_ship(turn_t turn) {
	switch (ship.direction) {
	case DIRECTION_UP:
		switch (ship.position.y) {
		case SEGMENT_TOP:
		case SEGMENT_BOTTOM:
			ship.position.y = SEGMENT_LOWER;
			ship.position.x++;
			break;
		case SEGMENT_MIDDLE:
			ship.position.y = SEGMENT_UPPER;
			ship.position.x++;
			break;
		case SEGMENT_UPPER:
			ship.position.y = SEGMENT_LOWER;
			break;
		case SEGMENT_LOWER:
			ship.position.y = SEGMENT_UPPER;
			break;
		}
		break;
	case DIRECTION_DOWN:
		switch (ship.position.y) {
		case SEGMENT_TOP:
		case SEGMENT_BOTTOM:
			ship.position.y = SEGMENT_UPPER;
			ship.position.x++;
			break;
		case SEGMENT_MIDDLE:
			ship.position.y = SEGMENT_LOWER;
			ship.position.x++;
			break;
		case SEGMENT_UPPER:
			ship.position.y = SEGMENT_LOWER;
			break;
		case SEGMENT_LOWER:
			ship.position.y = SEGMENT_UPPER;
			break;
		}
		break;
	case DIRECTION_RIGHT:
		switch (ship.position.y) {
		case SEGMENT_TOP:
		case SEGMENT_MIDDLE:
		case SEGMENT_BOTTOM:
			ship.position.x++;
			break;
		case SEGMENT_UPPER:
			switch (turn) {
			case TURN_LEFT:
				ship.position.y = SEGMENT_MIDDLE;
				break;
			case TURN_RIGHT:
				ship.position.y = SEGMENT_TOP;
				break;
			case TURN_NONE:
				ship.position.y = SEGMENT_LOWER;
				break;
			}
			break;
		case SEGMENT_LOWER:
			switch (turn) {
			case TURN_LEFT:
				ship.position.y = SEGMENT_BOTTOM;
				break;
			case TURN_RIGHT:
				ship.position.y = SEGMENT_MIDDLE;
				break;
			case TURN_NONE:
				ship.position.y = SEGMENT_UPPER;
				break;
			}
			break;
		}
		break;
	}
}

bool is_hit() {
	for (int i = 0; i < ASTEROID_COUNT; i++) {
		if ((ship.position.x == asteroids[i].x)
				&& (ship.position.y == asteroids[i].y)) {
			return true;
		}
	}
	return false;
}

void show_dots(bool value) {
	static uint32_t dot_ids[] = { LCD_SYMBOL_DP2, LCD_SYMBOL_DP3,
			LCD_SYMBOL_DP4, LCD_SYMBOL_DP5, LCD_SYMBOL_DP6, 0 };
	for (uint32_t i = 0; dot_ids[i] != 0; i++) {
		SegmentLCD_Symbol(dot_ids[i], value ? 1 : 0);
	}
}

void show_message_and_wait(const char* msg, bool blink_dots) {
	static char text_scroller[100];
#define WHITESPACE "   "
	uint32_t msg_len = strlen(msg);
	if (msg_len * 2 + strlen(WHITESPACE) * 2 + 1 > sizeof(text_scroller)) {
		return; //message is too large for the text_scroller buffer
	}
	snprintf(text_scroller, sizeof(text_scroller),
	WHITESPACE "%s" WHITESPACE "%s", msg, msg);
	uint32_t text_scroller_len = strlen(WHITESPACE) + msg_len;
	keypress_reset();
	for (uint32_t i = 0; !keypress_any(); i++) {
		static char screen_text[8];
		snprintf(screen_text, sizeof(screen_text),
				text_scroller + (i % text_scroller_len));
		SegmentLCD_Write(screen_text);
		show_dots(blink_dots && (i % 2) == 0);
		Delay(1000);
	}
}

void animate_timer() {
	for (int32_t ring_segment = 0; ring_segment < 8; ring_segment++) {
		SegmentLCD_ARing(ring_segment, 1);
	}
	for (int32_t time = 7; time >= 0; time--) {
		Delay(400);
		SegmentLCD_ARing(time, 0);
	}
}

bool is_goal_reached() {
	return ship.position.x == 7;
}

void increment_score() {
	ship.score++;
	SegmentLCD_Number(ship.score);
}

uint32_t get_delay_ticks()
{
	uint32_t ticks = 2000-ship.score*20;
	if(ticks < 1000)
	{
		ticks = 1000;
	}
	return ticks;
}

/***************************************************************************//**
 * @brief  Main function
 ******************************************************************************/
int main(void) {
	/* Chip errata */
	CHIP_Init();

	/*Initialize LCD Display*/
	SegmentLCD_Init(false);

	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
		while (1)
			;
	}

	CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);

	// Enable GPIO peripheral clock
	CMU_ClockEnable(cmuClock_GPIO, true);

	GPIO_PinModeSet(BUTTON_PORT, BUTTON_LEFT_PIN, gpioModeInput, 0);
	GPIO_PinModeSet(BUTTON_PORT, BUTTON_RIGHT_PIN, gpioModeInput, 0);

	/* Initialize LED driver */
	BSP_LedsInit();

	//felfut� �lek figyel�se
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_LEFT_PIN, BUTTON_LEFT_PIN, 1, 0, 1);
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_RIGHT_PIN, BUTTON_RIGHT_PIN, 1, 0, 1);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	show_message_and_wait("WELCOME  PRESS ANY KEY", false);
start_new_game:
	reset_score();
start_new_level:
	reset_game();
	keypress_read_and_clear();
	create_asteroids();
	update_display();
	animate_timer();
	do {
		Delay(get_delay_ticks());
		keypress_t keys = keypress_read_and_clear();
		turn_t turn = get_turn(keys);
		turn_ship(turn);
		move_ship(turn);
		update_display();
		if (is_goal_reached()) {
			increment_score();
			goto start_new_level;
		}
	} while (!is_hit());
	reset_game();
	show_message_and_wait("GAME OVER", true);
	show_dots(false);
	goto start_new_game;
}
