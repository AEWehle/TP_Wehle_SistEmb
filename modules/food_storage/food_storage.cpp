//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "infrared_sensor.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool underStorageDetected = OFF;
static bool underStorageDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void foodStorageInit()
{
    infraredSensorInit();
}

void foodStorageUpdate()
{
    infraredSensorUpdate();

    underStorageDetectorState = !infraredSensorRead();

    if ( underStorageDetectorState ) {
        underStorageDetected = ON;
    }
}

bool underStorageDetectorStateRead()
{
    return underStorageDetectorState;
}


bool underStorageDetectedRead()
{
    return underStorageDetected;
}


void foodStorageDeactivate()
{
    underStorageDetected = OFF;    
}

//=====[Implementations of private functions]==================================
