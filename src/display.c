#include "display.h"
#include "input.h"
#include "timing.h"

//Friss�ti a kijelz� �llapot�t a v�ltoz�soknak megfelel�en
void update_display() {
	//Minden szegmens t�rl�se
	for (uint8_t i = 0; i < 7; i++) {
		segmentField[i].raw = 0;
	}

	//Haj� kijelz�se
	display_segment(ship.position);

	//Aszteroid�k kijelz�se
	for (uint8_t i = 0; i < ASTEROID_COUNT; i++) {
		display_segment(asteroids[i]);
	}

	SegmentLCD_LowerSegments(segmentField);
}
//A megadott vektor koordin�t�inak megfelel� szegmenst bekapcsolja
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


//A pontokat villogtatja
void show_dots(bool value) {
	static uint32_t dot_ids[] = { LCD_SYMBOL_DP2, LCD_SYMBOL_DP3,
			LCD_SYMBOL_DP4, LCD_SYMBOL_DP5, LCD_SYMBOL_DP6, 0 };
	for (uint32_t i = 0; dot_ids[i] != 0; i++) {
		SegmentLCD_Symbol(dot_ids[i], value ? 1 : 0);
	}
}

//Ki�rja a megadott �zenetet, a kijelz�n jobbr�l balra �sztatva, �s v�r egy k�ls� akci�ra
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

//Anim�ci� a p�lya elej�n
void animate_timer() {
	for (int32_t ring_segment = 0; ring_segment < 8; ring_segment++) {
		SegmentLCD_ARing(ring_segment, 1);
	}
	for (int32_t time = 7; time >= 0; time--) {
		Delay(400);
		SegmentLCD_ARing(time, 0);
	}
}
