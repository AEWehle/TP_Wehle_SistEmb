//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"

//=====[Declaration of private defines]========================================

// #define MOTOR_UPDATE_TIME 9

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalInOut motorPin(PF_2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

motorDirection_t motorState;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorPin.mode(OpenDrain);
    
    motorPin.output();

    motorState = STOPPED;
}

motorDirection_t motorStateRead()
{
    return motorState;
}

void motorActivation()
{
    motorState = ACTIVE;
}

void motorDeactivation()
{
    motorState = STOPPED;
}

void motorControlUpdate()
{
    if( motorState == ACTIVE ){
        motorPin = HIGH;
    }
    else{ 
        motorPin = LOW;
    }
}