//Beágyazott és Ambiens Rendszerek házi feladat
//Aszteroidaövezet (nyomógomb)
//Készítette: Fényes Balázs, Fliegl Attila

#include "common.h"
#include "game_logic.h"
#include "input.h"
#include "display.h"
#include "timing.h"

int main(){
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

	//felfutó élek figyelése
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_LEFT_PIN, BUTTON_LEFT_PIN, 1, 0, 1);
	GPIO_ExtIntConfig(BUTTON_PORT, BUTTON_RIGHT_PIN, BUTTON_RIGHT_PIN, 1, 0, 1);
	NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	NVIC_EnableIRQ(GPIO_ODD_IRQn);

	show_message_and_wait("WELCOME  PRESS ANY KEY", false);
	do {
		reset_score();
		reset_game();
start_new_level:
		level_up();
		keypress_read_and_clear();
		create_asteroids();
		update_display();
		animate_timer();
		do {
			Delay(get_delay_ticks(ship.score));
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
