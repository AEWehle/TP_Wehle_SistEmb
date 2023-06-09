//=====[#include guards - begin]===============================================

#ifndef _TIME_FOR_FOOD_H_
#define _TIME_FOR_FOOD_H_

//=====[Declaration of public defines]=========================================

typedef enum {
    CLOSED,
    OPEN
} loop_mode_t;

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void timeForFoodInit();
void timeForFoodUpdate();

void set_max_food_load( float new_food_load );

int get_times_q();
int get_time_for_food( int index );

//=====[#include guards - end]=================================================

#endif // _TIME_FOR_FOOD_H_