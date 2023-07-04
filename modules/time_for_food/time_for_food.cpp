//=====[Libraries]=============================================================

#include "arm_book_lib.h"

#include "smart_food_system.h"

#include "motor.h"
#include "bowl.h"
// #include "date_and_time.h"
#include "sd_card.h"
#include "time_for_food.h"
#include "pc_serial_com.h"


//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

bool save_in_SD = false;
// const int MAX_TIMES_DAY = (int)(24*60/FOOD_TIME_MINUTES_INCREMENT); 
// Como FOOD TIME INCREMENT es 10 minutos, es posible expulsar cada 144 veces en un dia
static int timesIndex = 0;
// horarios cada 10 minutos desde 00:00 a 23:50, opción desde 0 a 143 
static food_time_t times_for_food[MAX_TIMES_DAY]; 
// hora   (int) food_time/(60/FOOD_TIME_MINUTES_INCREMENT)
// minutos   food_time % (60/FOOD_TIME_MINUTES_INCREMENT)*FOOD_TIME_MINUTES_INCREMENT)
// food number  (food_time_t) (int)( (hour*60 + min) / FOOD_TIME_MINUTES_INCREMENT);


//=====[Declaration and initialization of private global variables]============

bool charging = false;
loop_mode_t food_mode = CLOSED;
const float FOOD_LOAD_DEFAULT = 15;//g
static float food_load_required = FOOD_LOAD_DEFAULT;

//=====[Declarations (prototypes) of private functions]========================

void sort_times();
bool its_time( char* actual_time );
void charge_times_from_SD();

//=====[Implementations of public functions]===================================

void timeForFoodInit()
{  
    charging = false;
    pcSerialComStringWrite("Buscando archivo de tiempos\r\n");
    charge_times_from_SD();
    save_in_SD = false;
    // Por default de entrega comida hasta que haya food_load_required en el bowl a las 8hs y a las 20hs
}

void timeForFoodUpdate()
{
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
    return (food_time_t) (int) (hour*60 / FOOD_TIME_MINUTES_INCREMENT) + min ;
     // no se divide a los minutos por el incremento porque sino repetiria la expulsion 
     //los 10 minutos correspondientes a su decena
}

bool get_if_save_times_in_SD(){
    return save_in_SD;
}

void set_times_saved(){
    save_in_SD = false;
}

void set_food_load_required( float new_food_load ){
    if ( new_food_load >= 5 && new_food_load <( FOOD_LOAD_LIMIT - 10 ))
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
        food_time_t time_numer = hour_min_2_food_number( hour, min );

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
    save_in_SD = true;
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
            save_in_SD = true;
        }
    }
    if(found) sort_times();
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
    if( found ){
        timesIndex--;
        save_in_SD = true;
    }
}


void delete_food_time_in_position( int bad_time_position )
{
    for ( int i = bad_time_position ; i < (timesIndex-1) ; i++ )
    {
        times_for_food[ i ] = times_for_food[i+1];
    }
    save_in_SD = true;
    timesIndex--;
}


//=====[Implementations of private functions]==================================

void sort_times(){
    sort(times_for_food, times_for_food + timesIndex);
}

int char2int( char the_char ){
    int number = 0;
    if ( the_char >= '0' && the_char <= '9' ){
        number = (int)( the_char ) - 48;
    }
    return number;
}

void charge_times_from_SD(){
    char readBuffer[MAX_TIMES_DAY*4 + 3];
    if ( sdCardReadFile( "config_time_for_food.txt", readBuffer, MAX_TIMES_DAY * 4 + 3 ) ){
    char aux[15] = "";
    for(int i = 0 ; readBuffer[i] != '.' && i < (MAX_TIMES_DAY*4 + 3); ){
        add_food_time( char2int( readBuffer[i] )*100 + char2int( readBuffer[i+1] )*10 + char2int( readBuffer[i+2] ) );
        sprintf( aux, "%i:%d\r\n", i/4, char2int( readBuffer[i] )*100 + char2int( readBuffer[i+1] )*10 + char2int( readBuffer[i+2] ) );
        i += 4;
        pcSerialComStringWrite( aux );
    }
    }
}