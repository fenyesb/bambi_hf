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
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "bsp.h"
#include "bsp_trace.h"
#include "segmentlcd.h"
#include "segmentlcd_spec.h"

/////////////////////////////////////////////////////////////
////////////////////Globális változók////////////////////////

typedef enum Direction{Up, Down, Forward} Dir;

typedef struct {
	int8_t x, y;
} vector2_t;

typedef struct {
	vector2_t position;
	Dir direction;
} player_t;

player_t ship = {.position = {0,1}, .direction = Forward};

#define ASTEROID_COUNT 3 //ha ezt a számot 18-nál nagyobbra állítjuk, az megöli az aszteroida koordináta generáló függvényt,
						// valamint minél nagyobb ez a szám annál valószínûbb, hogy végigvihetetlen pálya generálódik
vector2_t asteroids[ASTEROID_COUNT];

SegmentLCD_SegmentData_TypeDef segmentField[7];

/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////


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

/////////////////////////////////////////////////////////////////////
////////////////Aszteroida/Pálya generálás///////////////////////////

int random(int min_num, int max_num)
{
    int result = 0, low_num = 0, hi_num = 0;

    if (min_num < max_num)
    {
        low_num = min_num;
        hi_num = max_num + 1; // include max_num in output
    } else {
        low_num = max_num + 1; // include max_num in output
        hi_num = min_num;
    }

    result = (rand() % (hi_num - low_num)) + low_num;
    return result;
}

void create_asteroids(vector2_t* asteroids){
	int8_t x, y;
	x = random(1,6);/*az elsõ 14-szegmens kijelzõre nem teszünk aszteroidát, mert felesleges 1: a hajó mellé kerül, ekkor egyel kevesebb aszteroidát kell kikerülni 2: a hajóval egy mezõre kerül, ekkor azonnal vége a játéknak*/
	y = random(0,4);/*kijelzõnként csak 5 szegmenst(középsõ kettõt egynek véve) használunk, a kijelzésnél az itt sorsolt számot dekódoljuk*/
	asteroids[0].x = x;
	asteroids[0].y = y;

	int a_count[7] = {0};//az egy kijelzõn lévõ aszteroidák száma
	a_count[x]++;

	for(int i = 1; i < ASTEROID_COUNT; i++){
		x = random(1,6);
		y = random(0,4);

		while(3 == a_count[x]){
			x = random(1,6);
		}

		if(0 == a_count[x]){
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		}
		else if(1 == a_count[x]){
			int z = 0;
			while(!(asteroids[z].x == x)){
				z++;
			}
			while(asteroids[z].y == y){
				y = random(0,4);
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		}
		else if(2 == a_count[x]){
			int z = 0;
			int ny[2];
			for(int k = 0; k < i; k++){
				if(asteroids[k].x == x){
					ny[z] = asteroids[k].y;
					z++;
				}
			}
			while((y == ny[0]) || (y == ny[1])){
				y = random(0,4);
			}
			asteroids[i].x = x;
			asteroids[i].y = y;
			a_count[x]++;
		}
	}
}

///////////////////////////////////////////////////////////////////
/////////////////////Megjelenítés//////////////////////////////////
//a megadott vektor  koordinátáinak megfelelõ szegmenst bekapcsolja
void display(vector2_t vector){
	int x = vector.x;
	int y = vector.y;
	switch(y){
		case 0:
			segmentField[x].a = 1;
			displaySegmentField(segmentField);
			break;
		case 1:
			segmentField[x].g = 1;
			segmentField[x].m = 1;
			displaySegmentField(segmentField);
			break;
		case 2:
			segmentField[x].d = 1;
			displaySegmentField(segmentField);
			break;
		case 3:
			segmentField[x].f = 1;
			displaySegmentField(segmentField);
			break;
		case 4:
			segmentField[x].e = 1;
			displaySegmentField(segmentField);
			break;
		default: break;
	}
}

//hajó kijelzése
void display_ship(){
	display(ship.position);
}

//aszteroidák kijelzése
void display_asteroids(){
	for(int i = 0; i < ASTEROID_COUNT; i++)
		display(asteroids[i]);
}

//törli a megadott vektort a kijelzõrõl
void erase(vector2_t vector){
	int x = vector.x;
	int y = vector.y;
	switch(y){
		case 0:
			segmentField[x].a = 0;
			displaySegmentField(segmentField);
			break;
		case 1:
			segmentField[x].g = 0;
			segmentField[x].m = 0;
			displaySegmentField(segmentField);
			break;
		case 2:
			segmentField[x].d = 0;
			displaySegmentField(segmentField);
			break;
		case 3:
			segmentField[x].f = 0;
			displaySegmentField(segmentField);
			break;
		case 4:
			segmentField[x].e = 0;
			displaySegmentField(segmentField);
			break;
		default: break;
	}
}

//törli a hajót a kijelzõrõl
void erase_ship(){
	erase(ship.position);
}

//torli az aszteroidákat a kijelzõrõl
void erase_asteroids(){
	for(int i = 0; i < ASTEROID_COUNT; i++)
		erase(asteroids[i]);
}

///////////////////////////////////////////////////////
///////////////////////Mozgás/////////////////////////

//TODO: gomb érzékelés mondjuk int visszatéréssel és a kész függvény meghívása a turn_f függvény if feltételeiben

int turn_f(int turn){
	int i = turn;
	if(i == -2)//balra gomb kiértékelése még hiányzik
		i--;//balra
	if(i == 2)//jobbra gomb kiértékelése még hiányzik
		i++;//jobbra
	return i;
}

void move(player_t* ship, int turn){
	erase_ship();
	//nem akarunk fordulni
	if(0 == turn){
		if(ship->direction == Up || ship->direction == Down){
			if(ship->position.y == 3)
				ship->position.y = 4;
			if(ship->position.y == 4)
				ship->position.y = 3;
		}
		if(ship->direction == Forward)
			ship->position.x++;
	}

	//balra akarunk fordulni
	if(0 > turn){
		if(ship->direction == Up){
			if(ship->position.y == 3)
				ship->position.y = 4;
			if(ship->position.y == 4)
				ship->position.y = 3;
		}

		if(ship->direction == Down){
			if(ship->position.y == 3)
				ship->position.y = 1;
			if(ship->position.y == 4)
				ship->position.y = 2;
			ship->direction = Forward;
		}

		if(ship->direction == Forward){
			if(ship->position.y == 0){
				ship->position.y = 4;
			}
			if(ship->position.y == 1){
				ship->position.y = 3;
			}
			if(ship->position.y == 2){
				ship->position.y = 4;
			}
			ship->direction = Up;
		}
	}

	//jobbra akarunk fordulni
	if (0 < turn){
		if(ship->direction == Up){
			if(ship->position.y == 3)
				ship->position.y = 0;
			if(ship->position.y == 4)
				ship->position.y = 1;
			ship->direction = Forward;
		}

		if(ship->direction == Down){
			if(ship->position.y == 3)
				ship->position.y = 4;
			if(ship->position.y == 4)
				ship->position.y = 3;
		}

		if(ship->direction == Forward){
			if(ship->position.y == 0){
				ship->position.y = 3;
			}
			if(ship->position.y == 1){
				ship->position.y = 4;
			}
			if(ship->position.y == 2){
				ship->position.y = 3;
			}
			ship->direction = Down;
		}
	}
}

void is_hit(bool* end){
	for(int i = 0; i < ASTEROID_COUNT; i++){
		if((ship.position.x == asteroids[i].x) & (ship.position.y == asteroids[i].y)){
			*end = true;
		}
	}
}

void level_up(player_t* ship, int score){
	if(7 == ship->position.x){
		erase_asteroids();
		ship->position.x = 0;
		ship->position.y = 1;
		ship->direction = Forward;
		score++;
		create_asteroids(asteroids);
		display_asteroids();
		display_ship();
	}
	else{
		display_ship();
	}
}



///////////////////////////////////////////////////////////////////////////////


void over(){
	erase_ship();
	erase_asteroids();
	while(1){
		SegmentLCD_Write("GAME");
		Delay(1000);
		SegmentLCD_Write("OVER");
		Delay(1000);
		//TODO: tizedespont villogtatás
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

  /*Initialize LCD Display*/
  SegmentLCD_Init(false);

  /* Setup SysTick Timer for 1 msec interrupts  */
  if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) {
    while (1) ;
  }

CMU_HFRCOBandSet(cmuHFRCOBand_28MHz);
  /* Initialize LED driver */
  BSP_LedsInit();
  BSP_LedSet(0);



  int score = 0;
  uint16_t turn;
  int a = 0;
  bool end = false;

  create_asteroids(asteroids);

  display_ship();
  display_asteroids();

  while (!end) {
	  turn = 0;
	  a = 0;
	  while(5000000*(1/(score+1)) > a){
		  //fordulás függvény meghívása
		  a++;
	  }

	  move(&ship, turn);
	  level_up(&ship, score);
	  is_hit(&end);
  }

  over();

}
