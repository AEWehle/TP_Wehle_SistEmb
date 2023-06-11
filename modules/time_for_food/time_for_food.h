//=====[#include guards - begin]===============================================

#ifndef _TIME_FOR_FOOD_H_
#define _TIME_FOR_FOOD_H_


//=====[Declaration of public defines]=========================================

typedef int food_time_t;

typedef enum {
    CLOSED,
    OPEN
}loop_mode_t;

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void timeForFoodInit();
void timeForFoodUpdate();

void set_max_food_load( float new_food_load );

int get_times_q();
int get_time_for_food( int index );
void add_food_time( food_time_t new_time );
void add_food_time( int hour, int minute );
void delete_food_time( food_time_t bad_time );
void delete_food_time_in_position( int bad_time_position );
void set_food_load_required( float new_food_load );
float get_food_load_required( );
loop_mode_t get_food_mode();
void change_to_food_mode( loop_mode_t newMode );
void change2_open_mode();
void change2_closed_mode();


int char2int( char the_char );
//=====[#include guards - end]=================================================

#endif // _TIME_FOR_FOOD_H_