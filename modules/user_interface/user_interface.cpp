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
#include "Scroll.h"


//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_REPORT_MS (int) 1000/SYSTEM_TIME_INCREMENT_MS
#define DISPLAY_FAST_REFRESH_TIME_MS (int) 100/SYSTEM_TIME_INCREMENT_MS // para cuando tiene que cambiar según acciones del usuario

//=====[Declaration of private data types]=====================================

typedef enum {
    DISPLAY_REPORT_STATE,
    DISPLAY_AJUSTES_STATE,
    DISPLAY_AJUSTES_SET_DATE_TIME_STATE,
    DISPLAY_AJUSTES_RELEASE_FOOD_STATE,
    DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE,
    DISPLAY_AJUSTES_BOWL_TARE_STATE,
    DISPLAY_AJUSTES_ALARM_STORAGE_STATE,
    DISPLAY_AJUSTES_SET_ACTUAL_DATE_STATE,
    DISPLAY_AJUSTES_SET_ACTUAL_TIME_STATE,
    DISPLAY_MOVING_MOTOR_STATE
} displayState_t;

typedef enum {
    DAY_STATE,
    MONTH_STATE,
    YEAR_STATE
} setDateState_t;


typedef enum {
    HOUR_STATE,
    MINUTE_STATE
} setTimeState_t;


//=====[Declaration and initialization of public global objects]===============

// InterruptIn gateOpenButton(PF_9);
// InterruptIn gateCloseButton(PF_8);// DigitalOut incorrectCodeLed(LED3);
Scroll scroll(PB_7,PB_8,PB_9);


//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

// char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static displayState_t displayState = DISPLAY_REPORT_STATE;
static setDateState_t settingDateState = YEAR_STATE;
static setTimeState_t settingTimeState = HOUR_STATE;
static int displayUserPosition = 0;
// static int displayIntruderAlarmGraphicSequence = 0;
static int settingValuePosition = 0;
static int displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;
bool alarmLowStorageActivation = true;

//=====[Declarations (prototypes) of private functions]========================

char* strcat( char* str , const char* strcat );

void set_user_cursor( int user_position );

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

static void userInterfaceDisplayReportStateInit();
static void userInterfaceDisplayReportStateUpdate();

static void userInterfaceDisplayAjustesStateInit();
static void userInterfaceDisplayAjustesStateUpdate();

static void userInterfaceDisplaySetDateTimeStateInit();
static void userInterfaceDisplaySetDateTimeStateUpdate();
static void displaySetDateState();
static void displaySetTimeState();

static void userInterfaceDisplayReleaseFoodStateInit();
static void userInterfaceDisplayReleaseFoodStateUpdate();

static void userInterfaceDisplaySetFoodTimesStateInit();
static void userInterfaceDisplaySetFoodTimesStateUpdate();

static void userInterfaceDisplayBowlTareStateInit();
static void userInterfaceDisplayBowlTareStateUpdate();

static void userInterfaceDisplayAlarmStorageStateUpdate();

//=====[Implementations of public functions]===================================


void userInterfaceInit()
{
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    userInterfaceDisplayUpdate();
} 

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_TYPE_LCD_HD44780, DISPLAY_CONNECTION_GPIO_4BITS );
    userInterfaceDisplayReportStateInit();
}
 

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
        
    if ( scroll.Up() )
        displayUserPosition++;
    else if( scroll.Down() )
        displayUserPosition--;

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
        case DISPLAY_AJUSTES_ALARM_STORAGE_STATE:
            userInterfaceDisplayAlarmStorageStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_ACTUAL_DATE_STATE:
            displaySetTimeState();
            break;
        case DISPLAY_AJUSTES_SET_ACTUAL_TIME_STATE:
            displaySetDateState();
            break;
        case DISPLAY_MOVING_MOTOR_STATE:
            // userInterfaceDisplayAlarmStorageStateUpdate();
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


static void userInterfaceDisplayReportStateInit()
{
    displayState = DISPLAY_REPORT_STATE;
    displayRefreshTimeMs = DISPLAY_REFRESH_TIME_REPORT_MS;

    displayUserPosition = 0;
    
    displayModeWrite( DISPLAY_MODE_CHAR );

    displayClear();

    displayPositionStringWrite ( 0,0 , "  /  /     :  |  :  " );
    displayPositionStringWrite ( 0,1 , "Peso:         |  :  " );
    displayPositionStringWrite ( 0,2 , "Alm.          |  :  " );
    displayPositionStringWrite ( 0,3 , "  *Ajustes    |  :  " );
}
 


static void userInterfaceDisplayReportStateUpdate()
{
    if(  scroll.Pressed() ){
        displayState = DISPLAY_AJUSTES_STATE;
        return;
    }

    // date and time
    char lineString[21] = "";
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    sprintf(lineString, "%d/%d/%d", timeinfo->tm_mday, timeinfo->tm_mon, 1900 + timeinfo->tm_year);
    displayPositionStringWrite ( 0,0 , lineString );   

    sprintf(lineString, "%d:%d", timeinfo->tm_hour, timeinfo->tm_min);
    displayPositionStringWrite ( 9,0 , lineString );   

    // food load
    sprintf(lineString, " %.0f g", get_food_load());
    displayPositionStringWrite ( 5,1 , lineString );
 
//  food storage state
    if ( getUnderStorageDetectorState() )
        sprintf(lineString, "BAJO");
    else{
        sprintf(lineString, "OK  ");}   
    displayPositionStringWrite ( 4,2 , lineString );
 
//  times for food
    int qtimes = get_times_q();
    int food_time;
    for (int i = 0 ; i < qtimes && i < 4 ; i++){
        food_time = get_time_for_food( i );
        sprintf(lineString, "%d:%d", (int) food_time/6, food_time % 6 *60); 
        displayPositionStringWrite ( 15,i , lineString );
    }
}

// DISPLAY EN AJUSTES
static void userInterfaceDisplayAjustesStateInit()
{
    displayState = DISPLAY_AJUSTES_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;

    displayClear();
    displayUserPosition = 1;
}

static void userInterfaceDisplayAjustesStateUpdate()
    {
    if( displayUserPosition > 6) 
        displayUserPosition = 6;
    else if( displayUserPosition < 1)
        displayUserPosition = 1;

    if(  scroll.Pressed() ){
            displayState =  ( displayState_t ) displayUserPosition;
            return;
        }
    
        char ajustesString[21] = "";
        switch( displayUserPosition )
        {
        case 1:case 2:case 3:case 4:
            sprintf(ajustesString, "Ajustes");
            displayPositionStringWrite ( 0,0 , ajustesString );
        
            sprintf(ajustesString, "Cambiar fecha/hora");
            displayPositionStringWrite ( 1,1 , ajustesString );
            
            sprintf(ajustesString, "Liberar alimento");
            displayPositionStringWrite ( 1,2 , ajustesString );
            
            sprintf(ajustesString, "Programar horario");
            displayPositionStringWrite ( 1,3 , ajustesString );
            
            set_user_cursor( displayUserPosition );    
        break;
        case 5:case 6:
            displayClear();
            sprintf(ajustesString, "Tara de bowl");
            displayPositionStringWrite ( 1,0 , ajustesString );

            sprintf(ajustesString, "Alarma almacen.");
            if( alarmLowStorageActivation )
                strcat(ajustesString, "  ON");
            else
                strcat(ajustesString, " OFF");
            displayPositionStringWrite ( 1,1 , ajustesString );

            set_user_cursor( displayUserPosition - 5);
        break;
    }
}

// DISPLAY en cambiar fecha y hora
static void userInterfaceDisplaySetDateTimeStateInit()
{
    displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition = 1;
    
    char setDateTimeString[21] = "";
    sprintf(setDateTimeString, "Cambiar fecha/hora");
    displayPositionStringWrite ( 0,0 , setDateTimeString );

    sprintf(setDateTimeString, "Fecha    DD/MM/AAAA");
    displayPositionStringWrite ( 1,1 , setDateTimeString );

    sprintf(setDateTimeString, "Hora     HH:MM");
    displayPositionStringWrite ( 1,2 , setDateTimeString );

    sprintf(setDateTimeString, "Listo");
    displayPositionStringWrite ( 14,3 , setDateTimeString );
}

static void displaySetDateState()
{
    char setDateString[21] = "";

    switch ( settingDateState ){
    case YEAR_STATE:
    if ( scroll.Up() ) {
        // aumentar un año a la fecha actúa con la librería de time o alguna
        }
    else if (scroll.Down() )
    // disminuir un año
        if (scroll.Pressed() ) settingDateState = MONTH_STATE;
    break;

    case MONTH_STATE:
        if (scroll.Pressed() ) settingDateState = DAY_STATE;
    break;

    case DAY_STATE:
        if (scroll.Pressed() ){
            settingDateState = YEAR_STATE;
            displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
        }
    break;
    }
    sprintf(setDateString, "*Fecha    DD/MM/AAAA");
    displayPositionStringWrite ( 0,1 , setDateString );    
}


static void displaySetTimeState()
{
    char setDateString[21] = "";

    switch ( settingTimeState ){
    case HOUR_STATE:
        if ( scroll.Up() ) {
        // aumentar una a la actual actúa con la librería de time o alguna sumando segundos
        }

        else if ( scroll.Down() ){
    // disminuir una hora
        }
        if ( scroll.Pressed() ) settingTimeState = MINUTE_STATE;
    break;

    case MINUTE_STATE:
        if ( scroll.Pressed() ){
            settingTimeState = HOUR_STATE;
            displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
        }
    break;
    }
    char setTimeString[21] = "";
    sprintf(setTimeString, "*Hora     HH:MM");
    displayPositionStringWrite ( 0,2 , setTimeString );
}

static void userInterfaceDisplaySetDateTimeStateUpdate()
{
    if( displayUserPosition > 3) 
        displayUserPosition = 3;
    else if( displayUserPosition < 1)
        displayUserPosition = 1;

    set_user_cursor( displayUserPosition );

    if(  scroll.Pressed() ){
        switch ( displayUserPosition ){
        case 3: 
            displayState = DISPLAY_AJUSTES_STATE;
        break;
        case 2:    
            displayState = DISPLAY_AJUSTES_SET_ACTUAL_TIME_STATE;
            settingTimeState = HOUR_STATE;
        break;
        case 1:
            settingDateState = YEAR_STATE;
            displayState = DISPLAY_AJUSTES_SET_ACTUAL_DATE_STATE;
        break;
        }
    }
}

// DISPLAY en liberar comida
static void userInterfaceDisplayReleaseFoodStateInit()
{
    displayState = DISPLAY_AJUSTES_RELEASE_FOOD_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition = 0;
    
    char releaseFoodString[21] = "";
    sprintf(releaseFoodString, "Liberar alimento");
    displayPositionStringWrite ( 0,0 , releaseFoodString );

    sprintf(releaseFoodString, "Gire la perilla");
    displayPositionStringWrite ( 2,1 , releaseFoodString );

    sprintf(releaseFoodString, "Presione para salir");
    displayPositionStringWrite ( 0,2 , releaseFoodString );

    sprintf(releaseFoodString, "Peso: %.0f g", get_food_load() );
    displayPositionStringWrite ( 3,3 , releaseFoodString );
}

static void userInterfaceDisplayReleaseFoodStateUpdate()
{
    if(  scroll.Pressed() )
    {
        displayState = DISPLAY_AJUSTES_STATE;
        return;
    }
    if( scroll.Down() || scroll.Up() ) 
        bowl_charge( 30.0 ); // le suma 30g al tacho 
    
    char releaseFoodString[21] = "";
    sprintf(releaseFoodString, "Peso: %.0f g", get_food_load() );
    displayPositionStringWrite ( 3,3 , releaseFoodString );
}

// DISPLAY en setear horarios
static void userInterfaceDisplaySetFoodTimesStateInit()
{
    displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();
    displayUserPosition = 0;
    userInterfaceDisplaySetFoodTimesStateUpdate();
}

static void userInterfaceDisplaySetFoodTimesStateUpdate()
{
    int qtimes = get_times_q();
    if( displayUserPosition > 4 + qtimes) 
        displayUserPosition = 4 + qtimes;
    else if( displayUserPosition < 1)
        displayUserPosition = 1;

    set_user_cursor( displayUserPosition );

    char setFoodTimesString[21] = "";
    switch( displayUserPosition ){
    case 1: case 2: case 3: case 4:
        sprintf(setFoodTimesString, "Establecer horarios");
        displayPositionStringWrite ( 0,0 , setFoodTimesString );

        sprintf(setFoodTimesString, "Liberar hasta:");
        displayPositionStringWrite ( 1,1 , setFoodTimesString );

        sprintf(setFoodTimesString, "Liberar siempre:");
        displayPositionStringWrite ( 1,2 , setFoodTimesString );

        sprintf(setFoodTimesString, "Agregar horario++");
        displayPositionStringWrite ( 1,3 , setFoodTimesString );

        int food_time;
        for (int i = 0 ; i < qtimes ; i++){
            food_time = get_time_for_food( i );
            sprintf(setFoodTimesString, "%d:%d", (int) food_time/6, food_time % 6 *60); 
            displayPositionStringWrite ( 1,i+4 , setFoodTimesString );
        }    
    break;
    }
}

// DISPLAY en establecer tara del bowl
static void userInterfaceDisplayBowlTareStateInit()
{
    displayState = DISPLAY_AJUSTES_BOWL_TARE_STATE;
    displayRefreshTimeMs = DISPLAY_FAST_REFRESH_TIME_MS;
    displayClear();

    displayUserPosition =0;
    
    char tareBowlString[21] = "";
    sprintf(tareBowlString, "Tara del bowl");
    displayPositionStringWrite ( 0,0 , tareBowlString );

    sprintf(tareBowlString, "Coloque el bowl de");
    displayPositionStringWrite ( 2,1 , tareBowlString );

    sprintf(tareBowlString, "su mascota vacio.");
    displayPositionStringWrite ( 1,2 , tareBowlString );

    sprintf(tareBowlString, "*Listo");
    displayPositionStringWrite ( 4,3 , tareBowlString );
}

static void userInterfaceDisplayBowlTareStateUpdate()
{
    if( scroll.Pressed() ){
        bowl_tare();
        displayState = DISPLAY_AJUSTES_STATE;
    }
}

static void userInterfaceDisplayAlarmStorageStateUpdate(){
    alarmLowStorageActivation = !alarmLowStorageActivation;
}

void set_user_cursor( int user_position ){
    displayPositionStringWrite ( 0, user_position , "*" );
}

