//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_food_system.h"

#include "user_interface.h"
#include "pc_serial_com.h"
#include "event_log.h"
#include "motor.h"
#include "sd_card.h"
#include "bowl.h"
#include "food_storage.h"
#include "time_for_food.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================
extern bool print_display;

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============
int time_increment_update;
const int system_time_update = (int)(SYSTEM_TIME_UPDATE_MS/SYSTEM_TIME_INCREMENT_MS);
time_t miTime ;
//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================


// cree las carpetas load sensor, bowl, food storage y infrared-sensor

void smartFoodSystemInit()
{
    time_increment_update = system_time_update;
    // agregados para el dispenser
    foodStorageInit();
    sdCardInit( true );
    timeForFoodInit();
    bowlInit();
    motorControlInit();

    pcSerialComInit();
    userInterfaceInit();
    miTime = time(NULL);
}

void smartFoodSystemUpdate()
{
    if( time(NULL) != miTime ){ // actualización cada 1 segundo
        timeForFoodUpdate();
        foodStorageUpdate();
        bowlUpdate();
        eventLogUpdate();
        miTime = time(NULL);
    }
    
    if( time_increment_update >=  system_time_update ){ // actualización cada 10 ms
        pcSerialComUpdate();
        time_increment_update = 0;
    }
    
    userInterfaceUpdate();
    time_increment_update++;
    
    delay(SYSTEM_TIME_INCREMENT_MS);
}

//=====[Implementations of private functions]==================================
