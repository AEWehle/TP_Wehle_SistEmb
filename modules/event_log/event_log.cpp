//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "event_log.h"

#include "user_interface.h"
// #include "date_and_time.h"
#include "pc_serial_com.h"
#include "sd_card.h"
#include "bowl.h"
#include "food_storage.h"
#include "time_for_food.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef struct systemEvent {
    time_t seconds;
    char typeOfEvent[EVENT_LOG_NAME_MAX_LENGTH];
    bool storedInSd;
} systemEvent_t;

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============


static bool foodIncreasedLastState          = OFF;
static bool foodDecreasedLastState          = OFF;
static bool emptyStorageLastState           = OFF;
static bool save_times_in_SD_last_state     = OFF;

static int eventsIndex     = 0;
static systemEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

//=====[Declarations (prototypes) of private functions]========================

static void eventLogElementStateUpdate( bool lastState,
                                        bool currentState,
                                        const char* elementName );
static void times_in_SD_Update( bool lastState,
                                        bool currentState);
//=====[Implementations of public functions]===================================

void eventLogUpdate()
{  

    bool currentState = foodIncreasedStateRead();
    eventLogElementStateUpdate( foodIncreasedLastState, currentState, "Aumento comida a " );
    foodIncreasedLastState = currentState;

    currentState = foodDecreasedStateRead();
    eventLogElementStateUpdate( foodDecreasedLastState, currentState, "Disminuye comida a " );
    foodDecreasedLastState = currentState;

    if(getStorageState() == EMPTY_STORAGE) currentState = ON;
    eventLogElementStateUpdate( emptyStorageLastState, currentState, "Comida en bowl " );
    emptyStorageLastState = currentState;

    currentState = get_if_save_times_in_SD();
    times_in_SD_Update( save_times_in_SD_last_state, currentState);
    foodDecreasedLastState = currentState;

    static time_t timeEventLog = time(NULL);
    if (time(NULL) >= timeEventLog + 60){
        timeEventLog = time(NULL);
        eventLogSaveToSdCard();
    }
}

int eventLogNumberOfStoredEvents()
{
    return eventsIndex;
}

void eventLogRead( int index, char* str )
{
    str[0] = '\0';
    // strcat( str, "Evento = " );
    sprintf(str, "%s%s", str, arrayOfStoredEvents[index].typeOfEvent );
    // strcat( str, "\r\nFecha y hora = " );
    sprintf(str, "%s. %s\r\n", str, ctime(&arrayOfStoredEvents[index].seconds) );
}

void eventLogWrite( bool currentState, const char* elementName )
{
    char eventStr[EVENT_LOG_NAME_MAX_LENGTH] = "";
    sprintf(eventStr, "%s", elementName);

    int foodLoad = (int)get_food_load();
    if ( foodLoad > 1500) sprintf( eventStr, "%s+1.5Kg", eventStr);
    else sprintf( eventStr, "%s%3dg", eventStr, foodLoad );


    if ( getStorageState() == LOW_STORAGE )
        sprintf(eventStr, "%s. Almac. BAJO.", eventStr);
    else if( getStorageState() == OK_STORAGE )
        sprintf(eventStr, "%s. Almac. OK.", eventStr);
    else
        sprintf(eventStr, "%s. Almac. VACIO.", eventStr);
    
    arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
    sprintf( arrayOfStoredEvents[eventsIndex].typeOfEvent, "%s", eventStr );
    arrayOfStoredEvents[eventsIndex].storedInSd = false;
    if ( eventsIndex < EVENT_LOG_MAX_STORAGE - 1 ) {
        eventsIndex++;
    } else {
        eventsIndex = 0;
    }

    pcSerialComStringWrite(eventStr);
    pcSerialComStringWrite("\r\n");
}

bool eventLogSaveToSdCard()
{
    char fileName[SD_CARD_FILENAME_MAX_LENGTH];
    char eventStr[EVENT_STR_LENGTH] = "";
    bool eventsStored = false;

    time_t seconds;
    int i;

    seconds = time(NULL);
    fileName[0] = '\0';

    strftime( fileName, SD_CARD_FILENAME_MAX_LENGTH, 
              "%Y_%m_%d", localtime(&seconds) );
    sprintf( fileName, "%s.txt" ,fileName);
    
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        if ( !arrayOfStoredEvents[i].storedInSd ) {
            eventLogRead( i, eventStr );
            if ( sdCardWriteFile( fileName, eventStr ) ){
                arrayOfStoredEvents[i].storedInSd = true;
                eventsStored = true;
            }
        }
    }

    if ( eventsStored ) {
        pcSerialComStringWrite("Guardando eventos en archivo ");
        pcSerialComStringWrite(fileName);
        pcSerialComStringWrite("\r\n");
    }

    return true;
}
//=====[Implementations of private functions]==================================

static void eventLogElementStateUpdate( bool lastState,
                                        bool currentState,
                                        const char* elementName )
{
    if ( (lastState == OFF) && (currentState == ON) ) {        
        eventLogWrite( currentState, elementName );       
    }
}

static void times_in_SD_Update( bool lastState,
                                        bool currentState)
{
    if ( (lastState == OFF) && (currentState == ON) ) {
        set_times_saved();
        int qtimes = get_times_q();
        char times[ MAX_TIMES_DAY * 4 + 3 ] = "";
        for (int i = 0 ; i < qtimes ; i++){
            sprintf(times, "%s%.3d,", times, get_time_for_food( i ) );
        }
        sprintf(times, "%s.", times);
        pcSerialComStringWrite("Tiempos de comida guardados en SD\r\n");
        sdCardCreateWriteFile( "config_time_for_food.txt", times );
    }
}