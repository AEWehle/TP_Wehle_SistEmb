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
static int timesIndex = 2;
static int times_for_food[MAX_TIMES_DAY] = { 48 , 120 }; // horarios cada 10 minutos desde 00:00 a 23:50, opción desde 0 a 143 
// hora =  (número // 6), div entera de 6
// minutos = (número % 6) *60, el resto*60


//=====[Declaration and initialization of private global variables]============

loop_mode_t food_mode;
const float FOOD_LOAD_DEFAULT = 100;//g

//=====[Declarations (prototypes) of private functions]========================

// devuelve true si actual time es un time_for_food
bool its_time( char* actual_time )
{
    int time_numer = atoi( actual_time[11] ) * 10 + atoi( actual_time[12] ) + atoi( actual_time[14] ) * 10 + atoi( actual_time[15] ); // (hora*6 + minuto)
    for( int i = 0; i < timesIndex ; i++){
        if ( time_numer == times_for_food[i] ) 
            return true;
    }
    return false;
}


//=====[Implementations of public functions]===================================

void time_for_food_init()
{    
    food_mode = CLOSED; // se entrega comida hasta que en el tacho haya MAX_FOOD_LOAD en el bowl
    set_max_food_load( FOOD_LOAD_DEFAULT );
}

void time_for_food_update()
{
    char* actual_time = dateAndTimeRead(); //  devuelve "Sun Sep 16 01:03:52 1973\n\0"
    if( its_time( actual_time, times_for_food[0] ) ){
        bowl_charge();
    }
}

void add_time_for_food( char new_time[6] ){
     if ( timesIndex < MAX_TIMES_DAY )
     {
        timesIndex++;
        for ( int i = 0; i<6 ; i++ )
        {
        times_for_food[timesIndex][I] = new_ime[i]
        }
     }
}

void change_a_time( char time[6], int index ){
     for ( int i = 0; i<6 ; i++ )
     {
     times_for_food[index][i] = time[i];
     }
}



//=====[Implementations of private functions]==================================
