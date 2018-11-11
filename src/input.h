#pragma once

#include "common.h"
#include "game_logic.h"

#define BUTTON_PORT gpioPortB
#define BUTTON_LEFT_PIN 9 //PB0
#define BUTTON_RIGHT_PIN 10 //PB1

typedef struct {
	bool left, right;
} keypress_t;

//Figyeli, hogy megnyumtuk-e b�rmelyik ir�nyv�lt� gombot
bool is_any_key_pressed();

//A j�t�k ind�t�sakor k�rt gombnyom�sra v�r
void wait_for_player();

volatile bool key_pressed_left;
volatile bool key_pressed_right;

//Le van nyomva b�rmelyik gomb?
bool keypress_any();

//Innent�l figyelj�k �jra a gombok lenyom�s�t
void keypress_reset();

//Megadja, hogy melyik gomb lenyom�sa t�rt�nt meg, �s �jrakezdi a figyel�st
keypress_t keypress_read_and_clear();

//Be�ll�tja a haj� fordul�s�nak ir�ny�t a gombnyom�soknak megfelel�en
turn_t get_turn(keypress_t keys);

//Jobb gomb lenyom�s�t kezel� IRQHandler
void GPIO_EVEN_IRQHandler();

//Bal gomb lenyom�s�t kezel� IRQHandler
void GPIO_ODD_IRQHandler();
