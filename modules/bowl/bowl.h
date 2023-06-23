//=====[#include guards - begin]===============================================

#ifndef _BOWL_H_
#define _BOWL_H_

#include "smart_food_system.h"

//=====[Declaration of public defines]=========================================

#define MINUTE_BOWL_SECONDS  10
#define FOOD_LOAD_LIMIT               1000.0
#define TOLERANCIA 10
#define MAX_TIME_RELEASING_FOOD_SECONDS 10

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void bowlInit();
void bowlUpdate();
void bowl_charge( float food_to_add );
float  get_food_load();
float  get_last_minute_food_load();
//void set_max_food_load(float max_food_load);
bool foodIncreasedStateRead();
bool foodDecreasedStateRead();

void bowl_tare();
void bowl_calibrate();

//=====[#include guards - end]=================================================

#endif // _BOWL_H_
