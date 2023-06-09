//=====[#include guards - begin]===============================================

#ifndef _SCROLL_H_
#define _SCROLL_H_

#include "smart_food_system.h"

//=====[Declaration of public defines]=========================================

#define MINUTE_SCROLL  (60000/SYSTEM_TIME_INCREMENT_MS)
#define FOOD_LOAD_LIMIT               1000.0
#define TOLERANCIA 5

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void scrollInit();
void scrollUpdate();
bool scrollPressed();
bool scrollDown();
bool scrollUp();

//=====[#include guards - end]=================================================

#endif // _SCROLL_H_
