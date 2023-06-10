//=====[Libraries]=============================================================

#include "mbed.h"

#include "infrared_sensor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalIn infrared(PE_12);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void infraredSensorInit()
{
    infrared.mode(PullDown);
}

void infraredSensorUpdate()
{
}

bool infraredSensorRead()
{
    return infrared.read();
}

//=====[Implementations of private functions]==================================

