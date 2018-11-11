#pragma once

#include "common.h"
#include "game_logic.h"

SegmentLCD_LowerCharSegments_TypeDef segmentField[7];

//Frissíti a kijelzõ állapotát a változásoknak megfelelõen
void update_display();

//A megadott vektor koordinátáinak megfelelõ szegmenst bekapcsolja
void display_segment(vector2_t vector);

//A pontokat villogtatja
void show_dots(bool value);

//Kiírja a megadott üzenetet, a kijelzõn jobbról balra úsztatva, és vár egy külsõ akcióra
void show_message_and_wait(const char* msg, bool blink_dots);

//Animáció a pálya elején
void animate_timer();
