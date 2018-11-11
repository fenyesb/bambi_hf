#include "game_logic.h"

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

//A cél {8. (nem létező) kijelző} elérését figyeli
bool is_goal_reached() {
	return ship.position.x == 7;
}

//Növeli a pontot eggyel
void increment_score() {
	ship.score++;
	SegmentLCD_Number(ship.score);
}
