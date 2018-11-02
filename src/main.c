//Beágyazott és Ambiens Rendszerek házi feladat
//Aszteroidaövezet (nyomógomb)
//Készítette: Fényes Balázs, Fliegl Attila

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "em_chip.h"
#include "em_cmu.h"
#include "bsp.h"
#include "segmentlcd.h"
#include "segmentlcd_spec.h"

//A mozgásirány változói
typedef enum {
	DIRECTION_UP, DIRECTION_DOWN, DIRECTION_RIGHT
} direction_t;

//A fordulás változói
typedef enum {
	TURN_LEFT, TURN_RIGHT, TURN_NONE
} turn_t;

//A szegmenseket leíró változók
typedef enum {
	SEGMENT_TOP,
	SEGMENT_MIDDLE,
	SEGMENT_BOTTOM,
	SEGMENT_UPPER_LEFT,
	SEGMENT_LOWER_LEFT,
	SEGMENT_INVALID
} segment_t;

//Vektor struktúra
typedef struct {
	int8_t x, y;
} vector2_t;

//A játékost/hajót leíró struktúra, mely tartalmazza a hajó
//koordinátáit, irányát és a szerzett pontokat
typedef struct {
	vector2_t position;
	direction_t direction;
	uint16_t score;
} player_t;

player_t ship = {{0,SEGMENT_MIDDLE}, DIRECTION_RIGHT, 0};

//Az aszteroidák száma és az azokat tartalmazó tömb
#define ASTEROID_COUNT 3
vector2_t asteroids[ASTEROID_COUNT];

SegmentLCD_SegmentData_TypeDef segmentField[7];

#define BUTTON_PORT gpioPortB
#define BUTTON_LEFT_PIN 9 //PB0
#define BUTTON_RIGHT_PIN 10 //PB1

volatile uint32_t msTicks; /* counts 1ms timeTicks */

void SysTick_Handler() {
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

//A gomb megnyomásának megfelelően állítja a keys megfelelő elemeit
keypress_t keypress_read_and_clear() {
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

//Beállítja a hajó fordulásának irányát
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

//Véletlenszerű számot generál a megadott értékek között
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

//A hajót az első kijelző körépső szegmensére helyezi és törli az aszteroidákat
void reset_game() {
	ship.position.x = 0;
	ship.position.y = SEGMENT_MIDDLE;
	ship.direction = DIRECTION_RIGHT;
	for (int32_t i = 0; i < ASTEROID_COUNT; i++) {
		asteroids[i].x = -1;
		asteroids[i].y = SEGMENT_INVALID;
	}
}

//Lényegében a reset_game függvény szintlépéskor használt változata
void level_up(){
	ship.position.x = 0;
	for (int32_t i = 0; i < ASTEROID_COUNT; i++) {
		asteroids[i].x = -1;
		asteroids[i].y = SEGMENT_INVALID;
	}
}

//Nullázza a pontokat
void reset_score() {
	ship.score = 0;
	SegmentLCD_Number(ship.score);
}

//Az aszteroidák generálására használt függvény
void create_asteroids() {
	int8_t x, y;
	x = random(1, 6); //Az első 14-szegmens kijelzõre nem teszünk aszteroidát, mert felesleges:
					//1.: a hajó mellé kerül, ekkor egyel kevesebb aszteroidát kell kikerülni 2.: a hajóval egy mezõre kerül, ekkor azonnal vége a játéknak
	y = random(0, 4); //Kijelzőnként csak 5 szegmenst(középsõ kettõt egynek véve) használunk, a kijelzésnél az itt sorsolt számot dekódoljuk
	asteroids[0].x = x;
	asteroids[0].y = y;

	int a_count[7] = { 0 }; //Az egy kijelzőn lévõ aszteroidák száma
	a_count[x]++;//A tömb indexelése 0-tól 6-ig megy éppen mint a kijelzőké, így használható az aszteroida x koordinátája mint index

	//A maradék két aszteroida koordinátáinak sorsolása
	for (int i = 1; i < ASTEROID_COUNT; i++) {
		x = random(1, 6);
		y = random(0, 4);

		while (a_count[x] == 3) {
			x = random(1, 6);
		}//maximum 3 aszteroida lehet egy kijelzőn, ezért azt figyeljük, hogy van-e annyi
		//ez abban az esetben nyer igazán értelmet, ha feljebb állítjuk az aszteroidák számát

		if (a_count[x] == 0) {//Nulla aszteroida van az adott kijelzőn
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		} else if (a_count[x] == 1) {//Egy aszteroida van az adott kijelzőn
			int z = 0;
			while (!(asteroids[z].x == x)) {//Kikeressük azt az aszteroidát a tömbből, amelyiknek az x koordínátája megegyezik az újonnan sorsolt x koordinátával
				z++;
			}
			while (asteroids[z].y == y) {//Ellenőrizzük, hogy a már meglévő aszteroida y koordinátája megegyezik-e az új y-nal
				y = random(0, 4);//Ha igen, akkor új y-t sorsolunk
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		} else if (a_count[x] == 2) {//Már kettő aszteroida is van az adott kijelzőn
			int z = 0;
			int ny[2];
			for (int k = 0; k < i; k++) {//Az említett két aszteroida kikeresése a tömbből
				if (asteroids[k].x == x) {
					ny[z] = asteroids[k].y;//A két aszteroida y koordínátáinak eltárolása egy tömbben
					z++;
				}
			}
			while ((y == ny[0]) || (y == ny[1])) {//Ha a már meglévő aszteroidák bármelyikének y koordinitájával egyezik az új y, akkor újat generálunk
				y = random(0, 4);
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		}
	}
}

//A megadott vektor koordinátáinak megfelelő szegmenst bekapcsolja
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
	case SEGMENT_UPPER_LEFT:
		segmentField[x].f = 1;
		break;
	case SEGMENT_LOWER_LEFT:
		segmentField[x].e = 1;
		break;
	}
}

//Frissíti a kijelző állapotát a változásoknak megfelelően
void update_display() {
	//Minden szegmens törlése
	for (int32_t i = 0; i < 7; i++) {
		segmentField[i].raw = 0;
	}

	//Hajó kijelzése
	display_segment(ship.position);

	//Aszteroidák kijelzése
	for (int32_t i = 0; i < ASTEROID_COUNT; i++) {
		display_segment(asteroids[i]);
	}

	displaySegmentField(segmentField);
}

//Figyeli, hogy megnyumtuk-e bármelyik irányváltó gombot
bool is_any_key_pressed() {
	bool is_left_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_LEFT_PIN);
	bool is_right_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_RIGHT_PIN);
	return is_left_pressed || is_right_pressed;
}

//A játék indításakor kért gombnyomásra vár
void wait_for_player() {
	while (!is_any_key_pressed())
		;
	while (is_any_key_pressed())
		;
	while (!is_any_key_pressed())
		;
}

//A hajó fordulásának irányát írja be a hajó megfelelő változójába
void turn_ship(turn_t turn) {
	switch (turn) {
	case TURN_LEFT:
		switch (ship.direction) {
		case DIRECTION_UP://Ha felfelé mozog a hajó nem fordulhat balra
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
		case DIRECTION_DOWN://Ha lefelé mozog a hajó nem fordulhat jobbra
			break;
		}
		break;
	case TURN_NONE:
		break;
	}
}

//Ez a függvény írja le a hajó mozgására megadott korlátozásokat és végzi el a hajó koordinátáin való változtatásokat, vagyis ez "mozgatja" a hajót
void move_ship(turn_t turn) {
	switch (ship.direction) {
	case DIRECTION_UP:
		switch (ship.position.y) {
		case SEGMENT_TOP:
		case SEGMENT_BOTTOM:
			ship.position.y = SEGMENT_LOWER_LEFT;
			ship.position.x++;
			break;
		case SEGMENT_MIDDLE:
			ship.position.y = SEGMENT_UPPER_LEFT;
			ship.position.x++;
			break;
		case SEGMENT_UPPER_LEFT:
			ship.position.y = SEGMENT_LOWER_LEFT;
			break;
		case SEGMENT_LOWER_LEFT:
			ship.position.y = SEGMENT_UPPER_LEFT;
			break;
		}
		break;
	case DIRECTION_DOWN:
		switch (ship.position.y) {
		case SEGMENT_TOP:
		case SEGMENT_BOTTOM:
			ship.position.y = SEGMENT_UPPER_LEFT;
			ship.position.x++;
			break;
		case SEGMENT_MIDDLE:
			ship.position.y = SEGMENT_LOWER_LEFT;
			ship.position.x++;
			break;
		case SEGMENT_UPPER_LEFT:
			ship.position.y = SEGMENT_LOWER_LEFT;
			break;
		case SEGMENT_LOWER_LEFT:
			ship.position.y = SEGMENT_UPPER_LEFT;
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
		case SEGMENT_UPPER_LEFT:
			switch (turn) {
			case TURN_LEFT:
				ship.position.y = SEGMENT_MIDDLE;
				break;
			case TURN_RIGHT:
				ship.position.y = SEGMENT_TOP;
				break;
			case TURN_NONE:
				ship.position.y = SEGMENT_LOWER_LEFT;
				break;
			}
			break;
		case SEGMENT_LOWER_LEFT:
			switch (turn) {
			case TURN_LEFT:
				ship.position.y = SEGMENT_BOTTOM;
				break;
			case TURN_RIGHT:
				ship.position.y = SEGMENT_MIDDLE;
				break;
			case TURN_NONE:
				ship.position.y = SEGMENT_UPPER_LEFT;
				break;
			}
			break;
		}
		break;
	}
}

//Figyeli, hogy nekimentünk-e valamelyik aszteroidának
bool is_hit() {
	for (int i = 0; i < ASTEROID_COUNT; i++) {
		if ((ship.position.x == asteroids[i].x)
				&& (ship.position.y == asteroids[i].y)) {
			return true;
		}
	}
	return false;
}

//A pontokat villogtatja
void show_dots(bool value) {
	static uint32_t dot_ids[] = { LCD_SYMBOL_DP2, LCD_SYMBOL_DP3,
			LCD_SYMBOL_DP4, LCD_SYMBOL_DP5, LCD_SYMBOL_DP6, 0 };
	for (uint32_t i = 0; dot_ids[i] != 0; i++) {
		SegmentLCD_Symbol(dot_ids[i], value ? 1 : 0);
	}
}

//Kiírja a megadott üzenetet, a kijelzőn jobbról balra úsztatva, és vár egy külső akcióra
void show_message_and_wait(const char* msg, bool blink_dots) {
	static char text_scroller[100];
#define WHITESPACE "   "
	uint32_t msg_len = strlen(msg);
	/*if (msg_len * 2 + strlen(WHITESPACE) * 2 + 1 > sizeof(text_scroller)) {
	 return; //message is too large for the text_scroller buffer
	 }*/
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

//A cél {8. (nem létező) kijelző} elérését figyeli
bool is_goal_reached() {
	return ship.position.x == 7;
}

//Növeli a pontot eggyel
void increment_score() {
	ship.score++;
	SegmentLCD_Number(ship.score);
}

//A hajó sebességének szabályozása
uint32_t get_delay_ticks() {
	uint32_t ticks = 2000 - ship.score * 20;
	if (ticks < 1000) {
		ticks = 1000;
	}
	return ticks;
}

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

	//felfutó élek figyelése
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_LEFT_PIN, BUTTON_LEFT_PIN, 1, 0, 1);
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_RIGHT_PIN, BUTTON_RIGHT_PIN, 1, 0, 1);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	show_message_and_wait("WELCOME  PRESS ANY KEY", false);
	do {
		reset_score();
start_new_level:
		level_up();
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
	} while (true);
}
