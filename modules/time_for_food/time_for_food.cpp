//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_food_system.h"

#include "motor.h"
#include "bowl.h"
#include "date_and_time.h"
#include "sd_card.h"
#include "time_for_food.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

const int MAX_TIMES_DAY = 50;
static char* times_for_food[MAX_TIMES_DAY];

//=====[Declaration and initialization of private global variables]============

loop_mode_t food_mode;
const float FOOD_LOAD_DEFAULT = 100;//g

//=====[Declarations (prototypes) of private functions]========================

// devuelve true si la hora y minuto son iguales
bool its_time( char* actual_time, char* a_time)
{
    for ( int i = 0 ; i < 5 ; i++ )
    {
        if ( actual_time[i + 11] != a_time[i] ) 
            return false;
    }
    return true;
}


//=====[Implementations of public functions]===================================

void time_for_food_init()
{    
    food_mode = CLOSED; // se entrega comida hasta que en el tacho haya MAX_FOOD_LOAD en el bowl
    set_max_food_load( FOOD_LOAD_DEFAULT );
    times_for_food[0] = "08:00";
    times_for_food[1] = "20:00";
}

void time_for_food_update()
{
    char* actual_time = dateAndTimeRead(); //  devuelve "Sun Sep 16 01:03:52 1973\n\0"
    if( its_time( actual_time, times_for_food[0] ) ){
        bowl_charge();
    }
}

void set_time_for_food( int hora, int minute){

}

//=====[Implementations of private functions]==================================
