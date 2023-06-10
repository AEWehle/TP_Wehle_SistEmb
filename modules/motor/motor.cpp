//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "motor.h"
    // sacar, solo prueba
// #include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

// #define MOTOR_UPDATE_TIME 9

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalInOut motorM1Pin(PF_2);
DigitalInOut motorM2Pin(PE_3);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

motorDirection_t motorState;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void motorControlInit()
{
    motorM1Pin.mode(OpenDrain);
    motorM2Pin.mode(OpenDrain);
    
    motorM1Pin.input();
    motorM2Pin.input();

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
            // pcSerialComStringWrite( "motor update" );
    if( motorState == ACTIVE ){
        motorM1Pin.input();
        motorM2Pin.input();
        // motorState = STOPPED;
    }
    else{ // ( motorState == STOPPED )
        motorM2Pin.input();
        motorM1Pin.output();
        motorM1Pin = LOW;
        // motorState = ACTIVE;
    }
}