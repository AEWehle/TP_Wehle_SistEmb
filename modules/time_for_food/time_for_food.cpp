//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_food_system.h"

#include "motor.h"
#include "bowl.h"
#include "date_and_time.h"
#include "sd_card.h"
#include "time_for_food.h"


//=====[Declaration of private defines]========================================

#define food_time_t int

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

const int MAX_TIMES_DAY = 50;
static int timesIndex = 2;
// horarios cada 10 minutos desde 00:00 a 23:50, opción desde 0 a 143 
static food_time_t times_for_food[MAX_TIMES_DAY] = { 48 , 120 }; 
// hora =  (número // 6), div entera de 6
// minutos = (número % 6) *60, el resto*60


//=====[Declaration and initialization of private global variables]============

loop_mode_t food_mode;
const float FOOD_LOAD_DEFAULT = 100;//g

//=====[Declarations (prototypes) of private functions]========================

void sort_times();
int char2int( char the_char );

//=====[Implementations of public functions]===================================

void time_for_food_init()
{    
    food_mode = CLOSED; // Por default de entrega comida hasta que haya MAX_FOOD_LOAD en el bowl a las 8hs y a las 20hs
    set_max_food_load( FOOD_LOAD_DEFAULT );
}

int get_times_q(){
    return timesIndex;
}
int get_time_for_food( int index ){
    return times_for_food[index];
}

// devuelve true si actual time es un times_for_food
bool its_time( char* actual_time )
{
    food_time_t time_numer = 
            char2int( actual_time[11] ) * 10 + char2int( actual_time[12] ) + // (hora*6 +
            char2int( actual_time[14] ) * 10 + char2int( actual_time[15] ); // + minuto)
    for( int i = 0; i < timesIndex ; i++){
        if ( time_numer == times_for_food[i] ) 
            return true;
    }
    return false;
}


void time_for_food_update()
{
    char* actual_time = dateAndTimeRead(); //  devuelve "Sun Sep 16 01:03:52 1973\n\0"
    if( its_time( actual_time ) ){
        bowl_charge();
    }
}


void add_food_time( food_time_t new_time ){
    bool found = false;
    for ( int i = 0 ; i < timesIndex && !found ; i++ )
    {
        if ( times_for_food[ i ] == new_time )
        {
            found = true;
        }
    }
    if (!found) {
        times_for_food[timesIndex++]= new_time;
        // timesIndex++;
    }
}


void change_food_time( food_time_t new_time, food_time_t old_time ){
     bool found = false;
     for ( int i = 0 ; i < timesIndex && !found ; i++ )
    {
        if ( times_for_food[ i ] == old_time )
        {
           found = true;
           times_for_food[i] = new_time;
        }
    }
}


void delete_food_time( food_time_t bad_time )
{
    bool found = false;
    for ( int i = 0 ; i < (timesIndex-1) ; i++ )
    {
        if ( times_for_food[ i ] == bad_time )
           found = true;
        if ( found )
        times_for_food[ i ] = times_for_food[i+1];
    }
}



//=====[Implementations of private functions]==================================

void sort_times(){
    sort(times_for_food, times_for_food + timesIndex);
}

int char2int( char the_char ){
    int number = 0;
    if ( the_char >= '0' && the_char <= '9' ){
        number = (int) the_char - 48;
    }
    return number;
}

