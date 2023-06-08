//=====[Libraries]=============================================================


#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "smart_food_system.h"
#include "date_and_time.h"
#include "display.h"
#include "motor.h"
#include "bowl.h"
#include "food_storage.h"
#include "time_for_food.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_REPORT_MS 1000
#define DISPLAY_FAST_REFRESH_TIME_MS 100 // para cuando tiene que cambiar según acciones del usuario

//=====[Declaration of private data types]=====================================

typedef enum {
    DISPLAY_REPORT_STATE,
    DISPLAY_AJUSTES_STATE,
    DISPLAY_AJUSTES_SET_DATE_TIME_STATE,
    DISPLAY_AJUSTES_RELEASE_FOOD_STATE,
    DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE,
    DISPLAY_AJUSTES_BOWL_TARE_STATE
} displayState_t;
//=====[Declaration and initialization of public global objects]===============

// InterruptIn gateOpenButton(PF_9);
// InterruptIn gateCloseButton(PF_8);// DigitalOut incorrectCodeLed(LED3);
DigitalIn encoderA(PB_7);
DigitalIn encoderB(PB_8);
DigitalIn encoderSW(PB_9);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

// char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static displayState_t displayState = DISPLAY_REPORT_STATE;
static int displayUserPosition = 0;
// static int displayIntruderAlarmGraphicSequence = 0;
static int displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

static void userInterfaceDisplayReportStateInit();
static void userInterfaceDisplayReportStateUpdate();

static void userInterfaceDisplayAjustesStateInit();
static void userInterfaceDisplayAjustesStateUpdate();

static void userInterfaceDisplaySetDateTimeStateInit();
static void userInterfaceDisplaySetDateTimeStateUpdate();

static void userInterfaceDisplayReleaseFoodStateInit();
static void userInterfaceDisplayReleaseFoodStateUpdate();

static void userInterfaceDisplaySetFoodTimesStateInit();
static void userInterfaceDisplaySetFoodTimesStateUpdate();

static void userInterfaceDisplayBowlTareStateInit();
static void userInterfaceDisplayBowlTareStateUpdate();
// static void gateOpenButtonCallback();
// static void gateCloseButtonCallback();

//=====[Implementations of public functions]===================================


void set_cursor( int user_position ){
    displayCharPositionWrite ( 0, displayUserPosition )
    displayStringWrite( "*" );
}

void userInterfaceInit()
{
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    userInterfaceDisplayUpdate();
} 

static void userInterfaceDisplayReportStateInit()
{
    displayState = DISPLAY_REPORT_STATE;
    displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;

    displayUserPosition =0;
    
    displayModeWrite( DISPLAY_MODE_CHAR );

    displayClear();

    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "  /  /     :  |  :  " );
    displayCharPositionWrite ( 0,1 );
    displayStringWrite( "Peso:         |  :  " );
    displayCharPositionWrite ( 0,2 );
    displayStringWrite( "Alm.          |  :  " );
    displayCharPositionWrite ( 0,3 );
    displayStringWrite( " *Ajustes     |  :  " );
}
 
static void userInterfaceDisplayReportStateUpdate()
{
    if( scroll_pressed() ){
        displayState = DISPLAY_AJUSTES_STATE;
        return;
    }

    // date and time
    char lineString[21] = "";
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    displayCharPositionWrite ( 0,0 );
    sprintf(lineString, "%d/%d/%d", timeinfo->tm_mday, timeinfo->tm_mon, 1900 + timeinfo->tm_year);
    displayCharPositionWrite ( 9,0 );
    sprintf(lineString, "%d:&d", timeinfo->tm_hour, timeinfo->tm_min);
    displayStringWrite( lineString );   

    // food load
    sprintf(lineString, " %.0f g", get_food_load());
    displayCharPositionWrite ( 5,1 );
    displayStringWrite( lineString );
 
//  food storage state
    displayCharPositionWrite ( 4,2 );
    if ( getUnderStorageDetectorState() )
        sprintf(lineString, "BAJO");
    else{
        sprintf(lineString, "OK  ");}   
    displayStringWrite( lineString );
 
//  times for food
    int qtimes = get_times_q();
    int food_time;
    for (int i = 0 ; i < qtimes && i < 4 ; i++){
        food_time = get_time_for_food( i );
        sprintf(lineString, "%d:%d", (int) food_time/6, food_time % 6 *60); 
        displayCharPositionWrite ( 15,i );
        displayStringWrite( lineString );
    }
}

static void userInterfaceDisplayInit()
{
    encoderSW.mode(PullUp);
    displayInit( DISPLAY_TYPE_LCD_HD44780, DISPLAY_CONNECTION_GPIO_4BITS );
    userInterfaceDisplayReportStateInit();
}
 

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
        
    if ( scroll_up() )
        userPositionDisplay++;
    else if( scroll_down() )
        userPositionDisplay--;

    if( accumulatedDisplayTime >=
        displayRefreshTimeMs ) { 
        accumulatedDisplayTime = 0; 
        switch ( displayState ) {
        case DISPLAY_REPORT_STATE:
             userInterfaceDisplayReportStateUpdate();
            break;
        case DISPLAY_AJUSTES_STATE:
            userInterfaceDisplayAjustesStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_DATE_TIME_STATE:
            userInterfaceDisplaySetDateTimeStateUpdate();
            break;
        case DISPLAY_AJUSTES_RELEASE_FOOD_STATE:
            userInterfaceDisplayReleaseFoodStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE:
            userInterfaceDisplaySetFoodTimesStateUpdate();
            break;
        case DISPLAY_AJUSTES_BOWL_TARE_STATE:
            userInterfaceDisplayBowlTareStateUpdate();
            break;

        default:
            userInterfaceDisplayReportStateInit();
            break;
        }
    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;
    }
}

// DISPLAY EN AJUSTES
static void userInterfaceDisplayAjustesStateInit()
{
    displayState = DISPLAY_AJUSTES_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;

    displayClear();
   
    displayUserPosition =0;
    
    char ajustesString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(ajustesString, "Ajustes");
    displayStringWrite( ajustesString );

    displayCharPositionWrite ( 0,1 );
    sprintf(ajustesString, "*Cambiar fecha/hora");
    displayStringWrite( ajustesString );
    
    displayCharPositionWrite ( 0,2 );
    sprintf(ajustesString, " Liberar alimento");
    displayStringWrite( ajustesString );
    
    displayCharPositionWrite ( 0,3 );
    sprintf(ajustesString, " Programar horario");
    displayStringWrite( ajustesString );
    
    // sprintf(ajustesString, " Tara de bowl       ");
    // sprintf(ajustesString, " Alarma bajo alm.OFF");
}

static void userInterfaceDisplayAjustesStateUpdate()
{
if( scroll_pressed() ){
        displayState = displayUserPosition;
        return;
    }
if( displayUserPosition > 5) 
    displayUserPosition = 5;
else if( displayUserPosition < 1)
    displayUserPosition = 1;

switch( displayUserPosition )
{
    case 1: case 2: case 3: case 4:
    char ajustesString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(ajustesString, "Ajustes");
    displayStringWrite( ajustesString );
   
    displayCharPositionWrite ( 1,1 );
    sprintf(ajustesString, "Cambiar fecha/hora");
    displayStringWrite( ajustesString );
    
    displayCharPositionWrite ( 1,2 );
    sprintf(ajustesString, "Liberar alimento");
    displayStringWrite( ajustesString );
    
    displayCharPositionWrite ( 1,3 );
    sprintf(ajustesString, "Programar horario");
    displayStringWrite( ajustesString );
    
    set_cursor( displayUserPosition );
    
    break;

    case 5: case 6: 
    displayCharPositionWrite ( 1,0 );
    sprintf(ajustesString, "Tara de bowl");
    displayStringWrite( ajustesString );

    displayCharPositionWrite ( 1,2 );
    sprintf(ajustesString, "Alarma bajo alm.OFF");
    displayStringWrite( ajustesString );

    set_cursor( displayUserPosition -4);
    break;
}
}



// DISPLAY en cambiar fecha y hora
static void userInterfaceDisplaySetDateTimeStateInit()
{
    displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition =0;
    
    char setDateTimeString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(setDateTimeString, "Cambiar fecha/hora");
    displayStringWrite( setDateTimeString );

    displayCharPositionWrite ( 1,1 );
    sprintf(setDateTimeString, "Fecha    DD/MM/AAAA");
    displayStringWrite( setDateTimeString );

    displayCharPositionWrite ( 1,2 );
    sprintf(setDateTimeString, "Hora     HH:MM");
    displayStringWrite( setDateTimeString );

    displayCharPositionWrite ( 14,3 );
    sprintf(setDateTimeString, "Listo");
    displayStringWrite( setDateTimeString );
}
static void userInterfaceDisplaySetDateTimeStateUpdate()
{

}

// DISPLAY en liberar comida
static void userInterfaceDisplayReleaseFoodStateInit()
{
    displayState = DISPLAY_AJUSTES_RELEASE_FOOD_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition =0;
    
    char releaseFoodString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(releaseFoodString, "Liberar alimento");
    displayStringWrite( releaseFoodString );

    displayCharPositionWrite ( 2,1 );
    sprintf(releaseFoodString, "Gire la perilla");
    displayStringWrite( releaseFoodString );

    displayCharPositionWrite ( 0,1 );
    sprintf(releaseFoodString, "Presione para salir");
    displayStringWrite( releaseFoodString );
}

static void userInterfaceDisplayReleaseFoodStateUpdate()
{

}

// DISPLAY en setear horarios
static void userInterfaceDisplaySetFoodTimesStateInit()
{
    displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    char setFoodTimesString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(setFoodTimesString, "Establecer horarios");
    displayStringWrite( setFoodTimesString );

    displayCharPositionWrite ( 1,1 );
    sprintf(setFoodTimesString, "Liberar hasta:");
    displayStringWrite( setFoodTimesString );

    displayCharPositionWrite ( 1,2 );
    sprintf(setFoodTimesString, "Liberar siempre");
    displayStringWrite( setFoodTimesString );

    displayCharPositionWrite ( 1,3 );
    sprintf(setFoodTimesString, "Eliminar horario");
    displayStringWrite( setFoodTimesString );

    displayCharPositionWrite ( 1,4 );
    sprintf(setFoodTimesString, "Modificar horario");
    displayStringWrite( setFoodTimesString );

    displayCharPositionWrite ( 1,5 );
    sprintf(setFoodTimesString, "Aregar horario++");
    displayStringWrite( setFoodTimesString );

    int qtimes = get_times_q();
    int food_time;
    for (int i = 0 ; i < qtimes ; i++){
        food_time = get_time_for_food( i );
        sprintf(setFoodTimesString, "%d:%d", (int) food_time/6, food_time % 6 *60); 
        displayCharPositionWrite ( 1,i+6 );
        displayStringWrite( setFoodTimesString );
    }
}

static void userInterfaceDisplaySetFoodTimesStateUpdate()
{

}

// DISPLAY en establecer tara del bowl
static void userInterfaceDisplayBowlTareStateInit()
{
    displayState = DISPLAY_AJUSTES_BOWL_TARE_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition =0;
    
    char tareBowlString[21] = "";
    displayCharPositionWrite ( 0,0 );
    sprintf(tareBowlString, "Tara del bowl");
    displayStringWrite( tareBowlString );

    displayCharPositionWrite ( 2,1 );
    sprintf(tareBowlString, "Coloque vacío el ¿");
    displayStringWrite( tareBowlString );

    displayCharPositionWrite ( 1,2 );
    sprintf(tareBowlString, "bowl de su mascota");
    displayStringWrite( tareBowlString );

    displayCharPositionWrite ( 4,3 );
    sprintf(tareBowlString, "*Listo");
    displayStringWrite( tareBowlString );
}

static void userInterfaceDisplayBowlTareStateUpdate()
{

}



