//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "mbed.h"

#include "food_storage.h"
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
storage_state_t storageState;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void foodStorageInit()
{
    infraredSensorInit();
    storageState = OK_STORAGE;
}

void foodStorageUpdate()
{

// pcSerialComStringWrite( "storage update" );
    infraredSensorUpdate();
    if( infraredSensorRead() ){
        if (storageState != EMPTY_STORAGE )
            storageState = LOW_STORAGE;
    }
    else storageState = OK_STORAGE;

}

void setEmptyStorage(){
    storageState = EMPTY_STORAGE;
}

storage_state_t getStorageState()
{
    return storageState;
}


//=====[Implementations of private functions]==================================
