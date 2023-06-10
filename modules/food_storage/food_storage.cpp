//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "infrared_sensor.h"
    // sacar, solo prueba
// #include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool underStorageDetectorState = OFF;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void foodStorageInit()
{
    infraredSensorInit();
}

void foodStorageUpdate()
{

// pcSerialComStringWrite( "storage update" );
    infraredSensorUpdate();
    underStorageDetectorState = !infraredSensorRead();

}

bool getUnderStorageDetectorState()
{
    return underStorageDetectorState;
}


//=====[Implementations of private functions]==================================
