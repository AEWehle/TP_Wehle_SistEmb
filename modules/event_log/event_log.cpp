//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "event_log.h"

#include "user_interface.h"
#include "date_and_time.h"
#include "pc_serial_com.h"
#include "sd_card.h"
#include "bowl.h"
#include "food_storage.h"

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


static bool foodIncreasedLastState         = OFF;
static bool foodDecreasedLastState         = OFF;
static bool underStorageLastState         = OFF;
static int eventsIndex     = 0;
static systemEvent_t arrayOfStoredEvents[EVENT_LOG_MAX_STORAGE];

//=====[Declarations (prototypes) of private functions]========================

// char* strcat( char* str , const char* strcat );
// char* strcpy( char* str , const char* strcat );

static void eventLogElementStateUpdate( bool lastState,
                                        bool currentState,
                                        const char* elementName );

//=====[Implementations of public functions]===================================

void eventLogUpdate()
{  
// pcSerialComStringWrite( "event log update" );
    bool currentState = foodIncreasedStateRead();
    eventLogElementStateUpdate( foodIncreasedLastState, currentState, "Aumento de comida a " );
    foodIncreasedLastState = currentState;

    currentState = foodDecreasedStateRead();
    eventLogElementStateUpdate( foodDecreasedLastState, currentState, "Dismiución de comida a " );
    foodDecreasedLastState = currentState;

    // currentState = gasDetectorStateRead();
    // eventLogElementStateUpdate( gasLastState, currentState, "GAS_DET" );
    // gasLastState = currentState;

    // currentState = overTemperatureDetectorStateRead();
    // eventLogElementStateUpdate( tempLastState, currentState, "OVER_TEMP" );
    // tempLastState = currentState;

    // currentState = incorrectCodeStateRead();
    // eventLogElementStateUpdate( ICLastState, currentState, "LED_IC" );
    // ICLastState = currentState;

    // currentState = systemBlockedStateRead();
    // eventLogElementStateUpdate( SBLastState ,currentState, "LED_SB" );
    // SBLastState = currentState;

    // currentState = motionSensorRead();
    // eventLogElementStateUpdate( motionLastState ,currentState, "MOTION" );
    // motionLastState = currentState;
}

int eventLogNumberOfStoredEvents()
{
    return eventsIndex;
}

void eventLogRead( int index, char* str )
{
    str[0] = '\0';
    strcat( str, "Evento = " );
    strcat( str, arrayOfStoredEvents[index].typeOfEvent );
    strcat( str, "\r\nFecha y hora = " );
    strcat( str, ctime(&arrayOfStoredEvents[index].seconds) );
    strcat( str, "\r\n" );
}

void eventLogWrite( bool currentState, const char* elementName )
{
    char eventStr[EVENT_LOG_NAME_MAX_LENGTH] = "";

    strcat( eventStr, elementName );
    sprintf( eventStr, " %.2f gr.", get_food_load() );
    if ( getStorageState() == LOW_STORAGE){
        strcat(eventStr, "Almac. BAJO");}
    else if( getStorageState() == OK_STORAGE){
        strcat(eventStr, "Almac. OK.");}
    else{
        strcat(eventStr, "Almac. VACIO.");}
    
    arrayOfStoredEvents[eventsIndex].seconds = time(NULL);
    strcpy( arrayOfStoredEvents[eventsIndex].typeOfEvent, eventStr );
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
              "%Y_%m_%d_%H_%M_%S", localtime(&seconds) );
    strcat( fileName, ".txt" );

    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        if ( !arrayOfStoredEvents[i].storedInSd ) {
            eventLogRead( i, eventStr );
            if ( sdCardWriteFile( fileName, eventStr ) ){
                arrayOfStoredEvents[i].storedInSd = true;
                pcSerialComStringWrite("Guardando evento ");
                pcSerialComIntWrite(i+1);
                pcSerialComStringWrite(" en archivo ");
                pcSerialComStringWrite(fileName);
                pcSerialComStringWrite("\r\n");
                eventsStored = true;
            }
        }
    }

    if ( eventsStored ) {
        pcSerialComStringWrite("Nuevos eventos guardados correctamente en la tarjeta SD\r\n\r\n");
    } else {
        pcSerialComStringWrite("No hay eventos para guardar en la tarjeta SD\r\n\r\n");
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
