#include "input.h"

//Figyeli, hogy megnyumtuk-e b�rmelyik ir�nyv�lt� gombot
bool is_any_key_pressed() {
	bool is_left_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_LEFT_PIN);
	bool is_right_pressed = !GPIO_PinInGet(BUTTON_PORT, BUTTON_RIGHT_PIN);
	return is_left_pressed || is_right_pressed;
}

//A j�t�k ind�t�sakor k�rt gombnyom�sra v�r
void wait_for_player() {
	while (!is_any_key_pressed())
		;
	while (is_any_key_pressed())
		;
	while (!is_any_key_pressed())
		;
}

//Le van nyomva b�rmelyik gomb?
bool keypress_any() {
	return key_pressed_left || key_pressed_right;
}

//Innent�l figyelj�k �jra a gombok lenyom�s�t
void keypress_reset() {
	key_pressed_left = false;
	key_pressed_right = false;
}

//Megadja, hogy melyik gomb lenyom�sa t�rt�nt meg, �s �jrakezdi a figyel�st
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

//Be�ll�tja a haj� fordul�s�nak ir�ny�t a gombnyom�soknak megfelel�en
turn_t get_turn(keypress_t keys) {
	if (keys.left) {
		return TURN_LEFT;
	} else if (keys.right) {
		return TURN_RIGHT;
	} else {
		return TURN_NONE;
	}
}

//Jobb gomb lenyom�s�t kezel� IRQHandler
void GPIO_EVEN_IRQHandler() {
	GPIO->IFC = GPIO->IF; //clear interrupt flag
	key_pressed_left = false;
	key_pressed_right = true;
}

//Bal gomb lenyom�s�t kezel� IRQHandler
void GPIO_ODD_IRQHandler() {
	GPIO->IFC = GPIO->IF; //clear interrupt flag
	key_pressed_left = true;
	key_pressed_right = false;
}
