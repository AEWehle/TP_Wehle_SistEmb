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

//=====[#include guards - end]=================================================

#endif // _TIME_FOR_FOOD_H_