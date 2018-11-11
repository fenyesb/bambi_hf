#pragma once

#include "common.h"
#include "game_logic.h"

SegmentLCD_LowerCharSegments_TypeDef segmentField[7];

//Friss�ti a kijelz� �llapot�t a v�ltoz�soknak megfelel�en
void update_display();

//A megadott vektor koordin�t�inak megfelel� szegmenst bekapcsolja
void display_segment(vector2_t vector);

//A pontokat villogtatja
void show_dots(bool value);

//Ki�rja a megadott �zenetet, a kijelz�n jobbr�l balra �sztatva, �s v�r egy k�ls� akci�ra
void show_message_and_wait(const char* msg, bool blink_dots);

//Anim�ci� a p�lya elej�n
void animate_timer();
