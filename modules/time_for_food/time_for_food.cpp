//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_food_system.h"

#include "motor.h"
#include "bowl.h"
#include "date_and_time.h"
#include "sd_card.h"
#include "time_for_food.h"
// #include "time.h"

// sacar, solo prueba
// #include "pc_serial_com.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============


const int MAX_TIMES_DAY = (int)(24*60/FOOD_TIME_MINUTES_INCREMENT); 
// Como FOOD TIME INCREMENT es 10 minutos, es posible expulsar cada 144 veces en un dia
static int timesIndex = 0;
// horarios cada 10 minutos desde 00:00 a 23:50, opción desde 0 a 143 
static food_time_t times_for_food[MAX_TIMES_DAY]; 
// hora =  (número // 6), div entera de 6
// minutos = (número % 6) *60, el resto*60


//=====[Declaration and initialization of private global variables]============

bool charging = false;
loop_mode_t food_mode = CLOSED;
const float FOOD_LOAD_DEFAULT = 100;//g
static float food_load_required = FOOD_LOAD_DEFAULT;

//=====[Declarations (prototypes) of private functions]========================

void sort_times();
bool its_time( char* actual_time );

//=====[Implementations of public functions]===================================

void timeForFoodInit()
{  
    charging = false;
    // Por default de entrega comida hasta que haya food_load_required en el bowl a las 8hs y a las 20hs
}

void timeForFoodUpdate()
{
    //    pcSerialComStringWrite( "time for food update" );
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);

    
    if( its_time( timeinfo -> tm_hour, timeinfo -> tm_min , timeinfo -> tm_sec ))
    {
        if( !charging )
        {
            charging = true;
            if ( food_mode == OPEN )
                bowl_charge( food_load_required ); 
                // a lazo abierto hay que cargar siempre lo mismo sin importar cuanto haya
            else 
                bowl_charge( food_load_required - get_food_load() );
                // a lazo cerrado hay que cargar hasta que haya food load required en el bowl
        }
        
    }
    else charging = false;
}

food_time_t hour_min_2_food_number( int hour, int min ){
    return (food_time_t) (int)( MAX_TIMES_DAY* hour/24 + min);
}

void set_food_load_required( float new_food_load ){
    food_load_required = new_food_load;
}
float get_food_load_required( ){
    return food_load_required;
}

void change_to_food_mode( loop_mode_t newMode ){
    food_mode = newMode;
}

void change2_open_mode(){
    food_mode = OPEN;
}

void change2_closed_mode(){
    food_mode = CLOSED;
}

loop_mode_t get_food_mode()
{    
    return food_mode;
}

int get_times_q(){
    return timesIndex;
}
int get_time_for_food( int index ){
    return times_for_food[index];
}

// devuelve true si actual time es un times_for_food
bool its_time( int hour, int min, int seconds )
{   
    if ( seconds == 0 ){
        food_time_t time_numer = hour * 6 + min;

        for( int i = 0; i < timesIndex ; i++){
            if ( time_numer == times_for_food[i] ) 
                return true;
        }
    }
    return false;
}



void add_food_time( food_time_t new_time ){
    bool found = false;
    for ( int i = 0 ; i < timesIndex && !found ; i++ )
    {
        if ( times_for_food[ i ] == new_time ) 
            found = true;
    }
    if (!found) {
        times_for_food[timesIndex++]= new_time;
        sort_times();
    }
}

void add_food_time( int hour, int minute )
{
    food_time_t new_time  = hour * (int)(60/FOOD_TIME_MINUTES_INCREMENT)  + (int) (minute/FOOD_TIME_MINUTES_INCREMENT);
    // hora =  (número // 6), div entera de 6
    // minutos = (número % 6) *60, el resto*60
    add_food_time( new_time );
}

void change_food_time( food_time_t new_time, food_time_t old_time ){
    bool found = false;
    if( new_time < 0)
        new_time = 0;
    else if( new_time >= MAX_TIMES_DAY ) 
        new_time = MAX_TIMES_DAY - 1;

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
        if ( times_for_food[ i ] == bad_time && !found)
           found = true;
        else if ( found )
        times_for_food[ i ] = times_for_food[i+1];
    }
    if( found )
        timesIndex--;
}


void delete_food_time_in_position( int bad_time_position )
{
    for ( int i = bad_time_position ; i < (timesIndex-1) ; i++ )
    {
        times_for_food[ i ] = times_for_food[i+1];
    }
    timesIndex--;
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

