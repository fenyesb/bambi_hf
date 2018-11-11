#pragma once

#include "common.h"
#include "game_logic.h"

#define BUTTON_PORT gpioPortB
#define BUTTON_LEFT_PIN 9 //PB0
#define BUTTON_RIGHT_PIN 10 //PB1

typedef struct {
	bool left, right;
} keypress_t;

//Figyeli, hogy megnyumtuk-e bármelyik irányváltó gombot
bool is_any_key_pressed();

//A játék indításakor kért gombnyomásra vár
void wait_for_player();

volatile bool key_pressed_left;
volatile bool key_pressed_right;

//Le van nyomva bármelyik gomb?
bool keypress_any();

//Innentõl figyeljük újra a gombok lenyomását
void keypress_reset();

//Megadja, hogy melyik gomb lenyomása történt meg, és újrakezdi a figyelést
keypress_t keypress_read_and_clear();

//Beállítja a hajó fordulásának irányát a gombnyomásoknak megfelelõen
turn_t get_turn(keypress_t keys);

//Jobb gomb lenyomását kezelõ IRQHandler
void GPIO_EVEN_IRQHandler();

//Bal gomb lenyomását kezelõ IRQHandler
void GPIO_ODD_IRQHandler();
