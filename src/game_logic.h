#pragma once

#include "common.h"

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

player_t ship;

//Az aszteroidák száma és az azokat tartalmazó tömb
#define ASTEROID_COUNT 3
vector2_t asteroids[ASTEROID_COUNT];

//Véletlenszerű számot generál a megadott értékek között
int random(int min_num, int max_num);

//A hajót az első kijelző körépső szegmensére helyezi és törli az aszteroidákat
void reset_game();

//Lényegében a reset_game függvény szintlépéskor használt változata
void level_up();

//Nullázza a pontokat
void reset_score();

//Az aszteroidák generálására használt függvény
void create_asteroids();

//A hajó fordulásának irányát írja be a hajó megfelelő változójába
void turn_ship(turn_t turn);

//Ez a függvény írja le a hajó mozgására megadott korlátozásokat és végzi el a hajó koordinátáin való változtatásokat, vagyis ez "mozgatja" a hajót
void move_ship(turn_t turn);

//Figyeli, hogy nekimentünk-e valamelyik aszteroidának
bool is_hit();

//A cél {8. (nem létező) kijelző} elérését figyeli
bool is_goal_reached();

//Növeli a pontot eggyel
void increment_score();
