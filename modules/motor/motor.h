//=====[#include guards - begin]===============================================

#ifndef _MOTOR_H_
#define _MOTOR_H_

//=====[Declaration of public defines]=========================================

//=====[Declaration of public data types]======================================

// typedef enum {
//     DIRECTION_1,
//     DIRECTION_2,
//     STOPPED
// } motorDirection_t;

typedef enum {
    ACTIVE,
    STOPPED
} motorDirection_t;

//=====[Declarations (prototypes) of public functions]=========================

void motorControlInit();
// void motorDirectionWrite( motorDirection_t direction );
void motorActivation();
void motorDeactivation();

motorDirection_t motorStateRead();

void motorControlUpdate();

//=====[#include guards - end]=================================================

#endif // _MOTOR_H_
