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
#include "time.h"
#include "Scroll.h"


//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_REPORT_MS 1000
#define DISPLAY_FAST_REFRESH_TIME_MS  100 // para cuando tiene que cambiar según acciones del usuario

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
    DISPLAY_AJUSTES_SET_LOAD_STATE,
    DISPLAY_AJUSTES_ADD_FOOD_TIME_STATE,
    DISPLAY_AJUSTES_MODIFY_FOOD_TIME_STATE
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

typedef enum {
    FOOD_HOUR_STATE,
    FOOD_MINUTE_STATE,
    ASK_DELETE_TIME_STATE,
    ASK_OK_TIME_STATE
} setFoodTimeState_t;

//=====[Declaration and initialization of public global objects]===============

Scroll scroll(PE_15, PE_14, PE_12); //CLK  DT  SW

//=====[Declaration of external public global variables]=======================

const int display_refresh_time_report =  (int) DISPLAY_REFRESH_TIME_REPORT_MS/ SYSTEM_TIME_INCREMENT_MS;
const int display_fast_refresh_time = (int) DISPLAY_FAST_REFRESH_TIME_MS/ SYSTEM_TIME_INCREMENT_MS;

//=====[Declaration and initialization of public global variables]=============

// char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static int index_food_time_selected;
static displayState_t displayState = DISPLAY_REPORT_STATE;
static setDateState_t settingDateState = YEAR_STATE;
static setTimeState_t settingTimeState = HOUR_STATE;
static setFoodTimeState_t settingFoodTimeState = FOOD_HOUR_STATE;
static int displayUserPosition = 0;
static int displayRefreshTimeMs = display_refresh_time_report;
bool alarmLowStorageActivation = true;

//=====[Declarations (prototypes) of private functions]========================

// char* strcat( char* str , const char* strcat );

void set_user_cursor( int user_position );
static void userPositionUpdate ();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

static void userInterfaceDisplayReportStateInit();
static void userInterfaceDisplayReportStateUpdate();

static void userInterfaceDisplayAjustesStateInit();
static void userInterfaceDisplayAjustesStateUpdate();

static void userInterfaceDisplaySetDateTimeStateUpdate();
static void displaySetDateState();
static void displaySetTimeState();

static void userInterfaceDisplayReleaseFoodStateUpdate();

static void userInterfaceDisplaySetFoodTimesStateUpdate();
static void userInterfaceSetFoodLoad();            
static void userInterfaceAddFoodTime();
static void userInterfaceModifyFoodTime();

static void userInterfaceDisplayBowlTareStateUpdate();

static void userInterfaceDisplayAlarmStorageStateUpdate();

//=====[Implementations of public functions]===================================


void userInterfaceInit()
{
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
 {
// pcSerialComStringWrite( "user interface update" );
    scroll.Update();
    userInterfaceDisplayUpdate();
} 

static void userInterfaceDisplayInit()
{
    displayInit( DISPLAY_TYPE_LCD_HD44780, DISPLAY_CONNECTION_GPIO_4BITS );
    userInterfaceDisplayReportStateInit();
}
 
static void userPositionUpdate ()
{
    if ( scroll.Up() ){
        scroll.disableUp();
        displayUserPosition++;
    }
    else if( scroll.Down() ){
        scroll.disableDown();
        displayUserPosition--;
    }
}


static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
        

    if( accumulatedDisplayTime >=
        displayRefreshTimeMs ) { 

        switch ( displayState ) {
        case DISPLAY_REPORT_STATE:
            displayUserPosition = 0;
            userInterfaceDisplayReportStateInit();
            break;
        case DISPLAY_AJUSTES_STATE:
            userPositionUpdate(); 
            userInterfaceDisplayAjustesStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_DATE_TIME_STATE:
            userPositionUpdate(); 
            userInterfaceDisplaySetDateTimeStateUpdate();
            break;
        case DISPLAY_AJUSTES_RELEASE_FOOD_STATE:
            userInterfaceDisplayReleaseFoodStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE:
            userPositionUpdate(); 
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
        case DISPLAY_AJUSTES_SET_LOAD_STATE:
            userInterfaceSetFoodLoad();            
            break;
        case DISPLAY_AJUSTES_ADD_FOOD_TIME_STATE:
            userInterfaceAddFoodTime();
            break;
        case DISPLAY_AJUSTES_MODIFY_FOOD_TIME_STATE:
            userInterfaceModifyFoodTime();
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
    displayRefreshTimeMs = display_refresh_time_report;

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
        scroll.disablePressed();
        userInterfaceDisplayAjustesStateInit();
        displayState = DISPLAY_AJUSTES_STATE;
        displayUserPosition = 0;
        return;
    }

    // date and time
    char lineString[21] = "";
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    sprintf(lineString, "%d/%d/%d ", timeinfo->tm_mday, timeinfo->tm_mon, 1900 + timeinfo->tm_year);
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
    int actual_time_number = timeinfo->tm_hour * (int)(60/FOOD_TIME_MINUTES_INCREMENT)  + (int) (timeinfo->tm_min/FOOD_TIME_MINUTES_INCREMENT);
    int inicial = 0;
    for ( ; inicial < qtimes && inicial < 4 ; inicial++){
        food_time = get_time_for_food( inicial );
        if ( actual_time_number > food_time ) inicial++;
        else break;
    }
    int posicion = 0;
    for ( int i = inicial ; i < qtimes && posicion < 4 ; i++){
        food_time = get_time_for_food( i );
        sprintf(lineString, "%d:%d", (int) food_time/6, food_time % 6 *60); 
        displayPositionStringWrite ( 15, posicion++ , lineString );
    }
}

// DISPLAY EN AJUSTES
static void userInterfaceDisplayAjustesStateUpdate()
{
    displayRefreshTimeMs = display_fast_refresh_time;

    displayClear();
    if( displayUserPosition > 5) 
        displayUserPosition = 5;
    else if( displayUserPosition < 0)
        displayUserPosition = 0;

    if(  scroll.Pressed() ){
        scroll.disablePressed();
        if ( displayUserPosition == 0 ){
            displayState = DISPLAY_REPORT_STATE;
        }
        else{
            displayState =  ( displayState_t ) displayUserPosition;
            displayClear();
            if( displayUserPosition != 5 )
            displayUserPosition = 0;
        }
        return;
        }
    
        char ajustesString[21] = "";
        switch( displayUserPosition )
        {
        case 0: case 1:case 2:case 3:
            sprintf(ajustesString, " Volver  AJUSTES    ");
            displayPositionStringWrite ( 0,0 , ajustesString );
        
            sprintf(ajustesString, " Cambiar fecha/hora ");
            displayPositionStringWrite ( 0,1 , ajustesString );
            
            sprintf(ajustesString, " Liberar alimento   ");
            displayPositionStringWrite ( 0,2 , ajustesString );
            
            sprintf(ajustesString, " Programar horarios ");
            displayPositionStringWrite ( 0,3 , ajustesString );
            
            set_user_cursor( displayUserPosition );    
        break;
        default:
            displayClear();
            sprintf(ajustesString, " Tara de bowl       ");
            displayPositionStringWrite ( 0,0 , ajustesString );

            sprintf(ajustesString, "Alarma almacen.");
            if( alarmLowStorageActivation )
                strcat(ajustesString, "  ON");
            else
                strcat(ajustesString, " OFF");
            displayPositionStringWrite ( 1,1 , ajustesString );

            set_user_cursor( displayUserPosition - 4);
        break;
    }
}

// DISPLAY en cambiar fecha y hora

static void userInterfaceDisplaySetDateTimeStateUpdate()
{
    if( displayUserPosition > 3) 
        displayUserPosition = 3;
    else if( displayUserPosition < 1)
        displayUserPosition = 1;

    
    time_t rawtime;
    time (&rawtime);
    struct tm * timeinfo;
    timeinfo = localtime (&rawtime);
    char setDateTimeString[21] = "";

    sprintf(setDateTimeString, "Cambiar fecha/hora  ");
    displayPositionStringWrite ( 0,0 , setDateTimeString );

    sprintf(setDateTimeString, " Fecha    %d/$d/%d  ", timeinfo->tm_mday, timeinfo->tm_mon, 1900 + timeinfo->tm_year);
    displayPositionStringWrite ( 0,1 , setDateTimeString );

    sprintf(setDateTimeString, " Hora     %d:%d     ", timeinfo->tm_hour, timeinfo->tm_min);
    displayPositionStringWrite ( 0,2 , setDateTimeString );

    sprintf(setDateTimeString, "              Listo ");
    displayPositionStringWrite ( 0,3 , setDateTimeString );

    set_user_cursor( displayUserPosition );

    if(  scroll.Pressed() ){
        scroll.disablePressed();
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

static void displaySetDateState()
{
    char setDateString[21] = "";

    time_t rawtime;
    time( &rawtime );
    struct tm* rtcTime = localtime(&rawtime);

    switch ( settingDateState ){
    case YEAR_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_year++; // aumentar un año
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_year--; // disminuir un año
            scroll.disableDown();
        }
        if (scroll.Pressed() ) {
            scroll.disablePressed();
            settingDateState = MONTH_STATE;
        }
    break;

    case MONTH_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_mon++; // aumentar un mes
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_mon--; // disminuir un mes
            scroll.disableDown();
        }
        if (scroll.Pressed() ) {
            scroll.disablePressed();
            settingDateState = DAY_STATE;
        }
    break;

    case DAY_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_mday++; // aumentar un dia
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_mday--; // disminuir un dia
            scroll.disableDown();
        }
        if (scroll.Pressed() ){
            scroll.disablePressed();
            settingDateState = YEAR_STATE;
            displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
        }
    break;
    }
    set_time( mktime( rtcTime ) );
    sprintf(setDateString, "Fecha   * %d/$d/%d  ", rtcTime -> tm_mday, rtcTime -> tm_mon, 1900 + rtcTime -> tm_year);
    displayPositionStringWrite ( 0,1 , setDateString );    
}


static void displaySetTimeState()
{
    time_t rawtime;
    time( &rawtime );
    struct tm* rtcTime = localtime(&rawtime);

    switch ( settingTimeState ){
    case HOUR_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_hour++; // aumentar una hora
        }
        else if (scroll.Down() ){
            rtcTime -> tm_hour--; // disminuir una hora
            scroll.disableDown();
        }
        if ( scroll.Pressed() ) {
            scroll.disablePressed();
            settingTimeState = MINUTE_STATE;
        }
    break;

    case MINUTE_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_min++; // aumentar un minuto
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_min--; // disminuir un minuto
            scroll.disableDown();
        }
        if ( scroll.Pressed() ){
            scroll.disablePressed();
            settingTimeState = HOUR_STATE;
            displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
        }
    break;
    }

    set_time( mktime( rtcTime ) );
    char setTimeString[21] = "";
    sprintf(setTimeString, " Hora     %d:%d     ", rtcTime -> tm_hour, rtcTime -> tm_min);
    displayPositionStringWrite ( 0,2 , setTimeString );
}


// DISPLAY en liberar comida
static void userInterfaceDisplayReleaseFoodStateUpdate()
{
    displayClear();
    
    char releaseFoodString[21] = "";
    sprintf(releaseFoodString, "Liberar alimento    ");
    displayPositionStringWrite ( 0,0 , releaseFoodString );

    sprintf(releaseFoodString, "  Gire la perilla   ");
    displayPositionStringWrite ( 0,1 , releaseFoodString );

    sprintf(releaseFoodString, "Presione para salir ");
    displayPositionStringWrite ( 0,2 , releaseFoodString );

    if(  scroll.Pressed() )
    {
        scroll.disablePressed();
        displayState = DISPLAY_AJUSTES_STATE;
        return;
    }
    if( scroll.Down() || scroll.Up() ) {
        bowl_charge( 30.0 ); // le suma 30g al tacho 
        scroll.disableUp();
        scroll.disableDown();
    }
    
    sprintf(releaseFoodString, "   Peso: %.0f g", get_food_load() );
    displayPositionStringWrite ( 0,3 , releaseFoodString );
}

// DISPLAY en setear horarios
static void userInterfaceDisplaySetFoodTimesStateInit()
{
    
}



static void userInterfaceDisplaySetFoodTimesStateUpdate()
{
    displayRefreshTimeMs = display_fast_refresh_time;
    displayClear();
    
    int qtimes = get_times_q();
    if( displayUserPosition > 3 + qtimes) 
        displayUserPosition = 3 + qtimes;
    else if( displayUserPosition < 0)
        displayUserPosition = 0;    
    
    
    if(  scroll.Pressed() )
    {
        scroll.disablePressed();
        switch( displayUserPosition ){
        case 0: 
            displayState = DISPLAY_AJUSTES_STATE; 
        break;
        case 1: 
            change2_closed_mode();
            displayState = DISPLAY_AJUSTES_SET_LOAD_STATE;
        break;
        case 2: 
            change2_open_mode(); 
            displayState = DISPLAY_AJUSTES_SET_LOAD_STATE;
        break;
        case 3: 
            displayState = DISPLAY_AJUSTES_ADD_FOOD_TIME_STATE;
        break;
        default:{
            index_food_time_selected = displayUserPosition - 4;
            displayState = DISPLAY_AJUSTES_MODIFY_FOOD_TIME_STATE;
        break;}
        }
        return;
    }

    char setFoodTimesString[21] = "";
    switch( displayUserPosition ){
    case 0: case 1: case 2: case 3:
        sprintf(setFoodTimesString, " Volver   HORARIOS  ");
        displayPositionStringWrite ( 0,0 , setFoodTimesString );

        if( get_food_mode() == CLOSED ){
            sprintf(setFoodTimesString, " Liberar hasta:%.0fg", get_food_load_required());
            displayPositionStringWrite ( 0,1 , setFoodTimesString );
            sprintf(setFoodTimesString, " Liberar siempre:OFF");
            displayPositionStringWrite ( 0,2 , setFoodTimesString );
        }
        else{
            sprintf(setFoodTimesString, " Liberar hasta:  OFF");
            displayPositionStringWrite ( 0,1 , setFoodTimesString );
            sprintf(setFoodTimesString, " Liberar siempre:%.0fg", get_food_load_required());
            displayPositionStringWrite ( 0,2 , setFoodTimesString );
        }

        sprintf(setFoodTimesString, " Agregar horario++");
        displayPositionStringWrite ( 0,3 , setFoodTimesString );

        set_user_cursor( displayUserPosition );
    break;
    default:{
        displayClear();
        int food_time;
        int inicial =  displayUserPosition - 4;
        for (int i = 0 ; i < qtimes && i < 3 ; i++){
            food_time = get_time_for_food( i + inicial );
            sprintf(setFoodTimesString, " %d:%d", (int) food_time/6, food_time % 6 *60); 
            displayPositionStringWrite ( 0, i , setFoodTimesString );
        }
        set_user_cursor( displayUserPosition % 4 );
    break;}
    }
}



static void userInterfaceAddFoodTime(){
    time_t rawtime;
    time( &rawtime );
    struct tm* rtcTime = localtime(&rawtime);
    rtcTime -> tm_min = (int) (rtcTime -> tm_min/10) *10;

    switch ( settingTimeState ){
    case HOUR_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_hour++; // aumentar una hora
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_hour--; // disminuir una hora
            scroll.disableDown();
        }
        if ( scroll.Pressed() ) {
            scroll.disablePressed();
            settingTimeState = MINUTE_STATE;
        }
    break;

    case MINUTE_STATE:
        if ( scroll.Up() ) {
            rtcTime -> tm_min = rtcTime -> tm_min + 10; // aumentar
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            rtcTime -> tm_min = rtcTime -> tm_min - 10; // disminuir
            scroll.disableDown();
        }
        if ( scroll.Pressed() ){
            settingTimeState = HOUR_STATE;
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
            add_food_time( rtcTime -> tm_hour, rtcTime -> tm_min );
        }
    break;
    }

    char setTimeString[21] = "";
    sprintf(setTimeString, "Nueva Hora *%d:%d", rtcTime -> tm_hour, rtcTime -> tm_min);
    displayPositionStringWrite ( 0,3 , setTimeString );
}



void userInterfaceSetFoodLoad(){
    if( scroll.Up() ) {
        set_food_load_required( get_food_load_required() + 5 );
        scroll.disableUp();
    }
    if( scroll.Down() ) {
        set_food_load_required( get_food_load_required() - 5 );
        scroll.disableDown();
    }
    if( scroll.Pressed() ) {
        scroll.disablePressed();
        set_food_load_required( get_food_load_required() );
        displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
        return;
    }
    char setFoodTimesString[21] = "";
    if( get_food_mode() == CLOSED ){
        sprintf(setFoodTimesString, "Liberar hasta:*%.0fg", get_food_load_required());
        displayPositionStringWrite ( 0,1 , setFoodTimesString );
        sprintf(setFoodTimesString, " Liberar siempre:OFF");
        displayPositionStringWrite ( 0,2 , setFoodTimesString );
    }
    else{
        sprintf(setFoodTimesString, " Liberar hasta:  OFF");
        displayPositionStringWrite ( 0,1 , setFoodTimesString );
        sprintf(setFoodTimesString, "Liberar siempre:*%.0fg", get_food_load_required());
        displayPositionStringWrite ( 0,2 , setFoodTimesString );
    }
}

static void userInterfaceModifyFoodTime( ){
    food_time_t food_time_selected = get_time_for_food( index_food_time_selected );
    switch ( settingFoodTimeState ){
    case FOOD_HOUR_STATE:
        displayPositionStringWrite ( 6, index_food_time_selected%4 , "*   spr  Listo" );
        if ( scroll.Up() ) {
            food_time_selected = food_time_selected + (int)(60/FOOD_TIME_MINUTES_INCREMENT); 
            // aumentar 60 minutos
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            food_time_selected = food_time_selected - (int)(60/FOOD_TIME_MINUTES_INCREMENT); 
            // disminuir 60 minutos
            scroll.disableDown();
        }
        if ( scroll.Pressed() ) {
            scroll.disablePressed();
            settingFoodTimeState = FOOD_MINUTE_STATE;
        }
        change_food_time( food_time_selected , get_time_for_food( index_food_time_selected ));
    break;

    case FOOD_MINUTE_STATE:
        displayPositionStringWrite ( 6, index_food_time_selected%4 , "*   spr  Listo" );
        if ( scroll.Up() ) {
            food_time_selected = food_time_selected + FOOD_TIME_MINUTES_INCREMENT; 
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            food_time_selected = food_time_selected - FOOD_TIME_MINUTES_INCREMENT; 
            scroll.disableDown();
        }
        if ( scroll.Pressed() ){
            scroll.disablePressed();
            settingFoodTimeState = ASK_DELETE_TIME_STATE;
            
        }
        change_food_time( food_time_selected , get_time_for_food( index_food_time_selected ));
    break;
    case ASK_DELETE_TIME_STATE:
        displayPositionStringWrite ( 6, index_food_time_selected%4 , "   *spr  Listo" );
        if ( scroll.Pressed() ) {
            delete_food_time_in_position( index_food_time_selected );
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
        }
        else if( scroll.Up() ) {
            settingFoodTimeState = ASK_OK_TIME_STATE;
        }
        
    break;
    case ASK_OK_TIME_STATE:
        displayPositionStringWrite ( 6, index_food_time_selected%4 , "    spr *Listo" );
        if ( scroll.Pressed() ) {
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
        }
        else if( scroll.Down() ) {
            settingFoodTimeState = ASK_DELETE_TIME_STATE;
        }
    break;
    }

    char setTimeString[21] = "";
    
    sprintf(setTimeString, "*%d:%d", food_time_selected, food_time_selected);
    displayPositionStringWrite ( 0, index_food_time_selected%4 , setTimeString );

}


// DISPLAY en establecer tara del bowl


static void userInterfaceDisplayBowlTareStateUpdate()
{
    displayState = DISPLAY_AJUSTES_BOWL_TARE_STATE;
    displayClear();

    
    char tareBowlString[21] = "";
    sprintf(tareBowlString, "Tara del bowl       ");
    displayPositionStringWrite ( 0,0 , tareBowlString );

    sprintf(tareBowlString, "  Coloque el bowl de");
    displayPositionStringWrite ( 0,1 , tareBowlString );

    sprintf(tareBowlString, " su mascota vacio.  ");
    displayPositionStringWrite ( 0,2 , tareBowlString );

    sprintf(tareBowlString, "    *Listo          ");
    displayPositionStringWrite ( 0,3 , tareBowlString );

    if( scroll.Pressed() ){
        scroll.disablePressed();
        bowl_tare();
        displayState = DISPLAY_AJUSTES_STATE;
    }
}

static void userInterfaceDisplayAlarmStorageStateUpdate(){
    if ( scroll.Pressed() ){
         scroll.disablePressed();
         alarmLowStorageActivation = !alarmLowStorageActivation;
         displayState = DISPLAY_AJUSTES_STATE;
    }
}

void set_user_cursor( int user_position ){
    displayPositionStringWrite ( 0, user_position , "*" );
}

