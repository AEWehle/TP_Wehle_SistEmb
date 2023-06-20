//=====[Libraries]=============================================================

#include "arm_book_lib.h"
#include "mbed.h"

#include "food_storage.h"
#include "infrared_sensor.h"
#include "siren.h"
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
bool alarmEnable;
//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

void foodStorageInit()
{
    infraredSensorInit();
    storageState = OK_STORAGE;
    alarmEnable = true;
}

void foodStorageUpdate()
{   static time_t storage_accum_time = time(NULL);

    if( sirenStateRead() ) sirenStateWrite( OFF );
    if ( storageState == EMPTY_STORAGE && isAlarmEnable() ){
        if( time(NULL) >= storage_accum_time + 60*30 ){ // cada 30 miutos suena la alarma de almcenamiento vacio
            sirenStateWrite( ON );
            storage_accum_time = time(NULL);
    }}
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
    if ( isAlarmEnable() ) sirenStateWrite( ON );
}

storage_state_t getStorageState()
{
    return storageState;
}

void toggleAlarmEmptyStorage(){
    alarmEnable = !alarmEnable;
}

bool isAlarmEnable(){
    return alarmEnable;
}


//=====[Implementations of private functions]==================================
