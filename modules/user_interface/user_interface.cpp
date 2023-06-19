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
#include "pc_serial_com.h"


//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_REPORT_MS 1000
#define DISPLAY_FAST_REFRESH_TIME_MS  1000 // para cuando tiene que cambiar según acciones del usuario
// #define DISPLAY_REFRESH_TIME_REPORT_MS 10
// #define DISPLAY_FAST_REFRESH_TIME_MS  1 // para cuando tiene que cambiar según acciones del usuario

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

const int display_refresh_time_report =  (int) DISPLAY_REFRESH_TIME_REPORT_MS/ SYSTEM_TIME_UPDATE_MS;
const int display_fast_refresh_time = (int) DISPLAY_FAST_REFRESH_TIME_MS/ SYSTEM_TIME_UPDATE_MS;

//=====[Declaration and initialization of public global variables]=============

// char codeSequenceFromUserInterface[CODE_NUMBER_OF_KEYS];

//=====[Declaration and initialization of private global variables]============

static int index_food_time_selected;
static displayState_t displayState = DISPLAY_REPORT_STATE;
static setDateState_t settingDateState = DAY_STATE;
static setTimeState_t settingTimeState = HOUR_STATE;
static setFoodTimeState_t settingFoodTimeState = FOOD_HOUR_STATE;
static int displayUserPosition = 0;
static int displayRefreshTimeMs = display_refresh_time_report;
static bool alarmLowStorageActivation = true;

int adding_hour = 0;
int adding_minute = 0;

//=====[Declarations (prototypes) of private functions]========================

// char* strcat( char* str , const char* strcat );

void set_user_cursor( int user_position );
static void userPositionUpdate ();

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

static void userInterfaceDisplayReportStateUpdate();

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
    displayModeWrite( DISPLAY_MODE_CHAR );
    displayInit( DISPLAY_TYPE_LCD_HD44780, DISPLAY_CONNECTION_GPIO_4BITS );
    userInterfaceDisplayReportStateUpdate();
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

void printDisplay();

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
        

    if( accumulatedDisplayTime >=
        displayRefreshTimeMs ) { 

        printDisplay();
        switch ( displayState ) {
        case DISPLAY_REPORT_STATE:
            userInterfaceDisplayReportStateUpdate();
            break;
        case DISPLAY_AJUSTES_STATE:
            userPositionUpdate(); 
            userInterfaceDisplayAjustesStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_DATE_TIME_STATE:{
            userPositionUpdate(); 
            userInterfaceDisplaySetDateTimeStateUpdate();
            break;}
        case DISPLAY_AJUSTES_RELEASE_FOOD_STATE:
            userInterfaceDisplayReleaseFoodStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE:{
            userPositionUpdate(); 
            userInterfaceDisplaySetFoodTimesStateUpdate();
            break;}
        case DISPLAY_AJUSTES_BOWL_TARE_STATE:
            userInterfaceDisplayBowlTareStateUpdate();
            break;
        case DISPLAY_AJUSTES_ALARM_STORAGE_STATE:
            userInterfaceDisplayAlarmStorageStateUpdate();
            break;
        case DISPLAY_AJUSTES_SET_ACTUAL_DATE_STATE:
            displaySetDateState();
            break;
        case DISPLAY_AJUSTES_SET_ACTUAL_TIME_STATE:
            displaySetTimeState();
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
            userInterfaceDisplayReportStateUpdate();
            break;
        }
    }
    else accumulatedDisplayTime++;
}


static void userInterfaceDisplayReportStateUpdate()
{
    displayRefreshTimeMs = display_refresh_time_report;

    if(  scroll.Pressed() ){
        scroll.disablePressed();
        userInterfaceDisplayAjustesStateUpdate();
        displayState = DISPLAY_AJUSTES_STATE;
        displayUserPosition = 0;
        displayClear();
        return;
    }

    // date and time
    char lineString[21] = "";
    time_t rawtime;
    struct tm * timeinfo;
    time (&rawtime);
    timeinfo = localtime (&rawtime);
    
    sprintf(lineString, "%.2d/%.2d/%.2d %2d:%.2d", timeinfo->tm_mday, timeinfo->tm_mon, 1900 -2000 + timeinfo->tm_year, timeinfo->tm_hour, timeinfo->tm_min);
    displayPositionStringWrite ( 0,0 , lineString );   

    // food load
    sprintf(lineString, "Peso: %3d g", (int)get_food_load());
    displayPositionStringWrite ( 0,1 , lineString );
 
//  food storage state
    if ( getUnderStorageDetectorState() )
        sprintf(lineString, "Almacen BAJO");
    else{
        sprintf(lineString, "Almacen   OK");}   
    displayPositionStringWrite ( 0,2 , lineString );
 
    displayPositionStringWrite ( 4,3 , "* Ajustes" );

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
        sprintf(lineString, "|%2d:%.2d", (int) food_time/6, food_time % 6 *60); 
        displayPositionStringWrite ( 14, posicion++ , lineString );
    }
}

// DISPLAY EN AJUSTES
static void userInterfaceDisplayAjustesStateUpdate()
{
    displayRefreshTimeMs = display_fast_refresh_time;

    if( displayUserPosition > 5) 
        displayUserPosition = 5;
    else if( displayUserPosition < 0)
        displayUserPosition = 0;

    if(  scroll.Pressed() ){
        scroll.disablePressed();
        if ( displayUserPosition == 0 ){
            displayClear();
            displayState = DISPLAY_REPORT_STATE;
        }
        else{
            displayState =  ( displayState_t ) (displayUserPosition + 1);
            if( displayUserPosition != 5 ){
                displayUserPosition = 0;
                displayClear();
            }
        }
        return;
        }
    
        char ajustesString[21] = "";
        switch( displayUserPosition )
        {
        case 0: case 1:case 2:case 3:
            sprintf(ajustesString, " Volver  AJUSTES");
            displayPositionStringWrite ( 0,0 , ajustesString );
        
            sprintf(ajustesString, " Cambiar fecha/hora");
            displayPositionStringWrite ( 0,1 , ajustesString );
            
            sprintf(ajustesString, " Liberar alimento");
            displayPositionStringWrite ( 0,2 , ajustesString );
            
            sprintf(ajustesString, " Programar horarios");
            displayPositionStringWrite ( 0,3 , ajustesString );
            
            set_user_cursor( displayUserPosition );    
        break;
        default:
            displayClear();
            sprintf(ajustesString, " Tara de bowl");
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

    sprintf(setDateTimeString, "Cambiar fecha/hora");
    displayPositionStringWrite ( 0,0 , setDateTimeString );

    sprintf(setDateTimeString, " Fecha   %.2d/%.2d/%4d", timeinfo->tm_mday, timeinfo->tm_mon, 1900 + timeinfo->tm_year);
    displayPositionStringWrite ( 0,1 , setDateTimeString );

    sprintf(setDateTimeString, " Hora     %2d:%.2d", timeinfo->tm_hour, timeinfo->tm_min);
    displayPositionStringWrite ( 0,2 , setDateTimeString );

    sprintf(setDateTimeString, "Listo");
    displayPositionStringWrite ( 14,3 , setDateTimeString );

    set_user_cursor( displayUserPosition );

    if(  scroll.Pressed() ){
        scroll.disablePressed();
        switch ( displayUserPosition ){
        case 3: {
            displayClear();
            displayState = DISPLAY_AJUSTES_STATE;
        break;}
        case 2:  {  
            displayState = DISPLAY_AJUSTES_SET_ACTUAL_TIME_STATE;
            settingTimeState = HOUR_STATE;
        break;}
        case 1:{
            settingDateState = DAY_STATE;
            displayState = DISPLAY_AJUSTES_SET_ACTUAL_DATE_STATE;
        break;}
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
            settingDateState = MONTH_STATE;
        }
        sprintf(setDateString, "*%.2d/%.2d/%4d", rtcTime -> tm_mday, rtcTime -> tm_mon, 1900 + rtcTime -> tm_year);
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
            settingDateState = YEAR_STATE;
        }
        sprintf(setDateString, "%.2d/*%.2d/%4d", rtcTime -> tm_mday, rtcTime -> tm_mon, 1900 + rtcTime -> tm_year);
    break;
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
            settingDateState = DAY_STATE;
            displayState = DISPLAY_AJUSTES_SET_DATE_TIME_STATE;
        }
        sprintf(setDateString, "%.2d/%.2d/*%4d", rtcTime -> tm_mday, rtcTime -> tm_mon, 1900 + rtcTime -> tm_year);
    break;
    }

    set_time( mktime( rtcTime ) );
    sprintf(setDateString, "%s%s", " Fecha  ", setDateString);
    displayPositionStringWrite ( 0,1 , setDateString );    
}


static void displaySetTimeState()
{
    time_t rawtime;
    time( &rawtime );
    struct tm* rtcTime = localtime(&rawtime);

    char setTimeString[21] = "";

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
        sprintf(setTimeString, "*%2d:%.2d", rtcTime -> tm_hour, rtcTime -> tm_min);
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
        sprintf(setTimeString, "%2d:*%.2d", rtcTime -> tm_hour, rtcTime -> tm_min);
    break;
    }

    set_time( mktime( rtcTime ) );
    sprintf(setTimeString, "%s%s", " Hora   ", setTimeString);
    displayPositionStringWrite ( 0,2 , setTimeString );
}


// DISPLAY en liberar comida
static void userInterfaceDisplayReleaseFoodStateUpdate()
{    
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
    
    sprintf(releaseFoodString, "Peso: %3d g", (int)get_food_load() );
    displayPositionStringWrite ( 2,3 , releaseFoodString );
}

// DISPLAY en setear horarios
static void userInterfaceDisplaySetFoodTimesStateUpdate()
{
    displayRefreshTimeMs = display_fast_refresh_time;
    
    int qtimes = get_times_q();
    if( displayUserPosition > 3 + qtimes) 
        displayUserPosition = 3 + qtimes;
    else if( displayUserPosition < 0)
        displayUserPosition = 0;    
    
    
    if(  scroll.Pressed() )
    {
        scroll.disablePressed();
        switch( displayUserPosition ){
        case 0: {
            displayClear();
            displayState = DISPLAY_AJUSTES_STATE; 
        break;}
        case 1: {
            change2_closed_mode();
            displayState = DISPLAY_AJUSTES_SET_LOAD_STATE;
        break;}
        case 2: {
            change2_open_mode(); 
            displayState = DISPLAY_AJUSTES_SET_LOAD_STATE;
        break;}
        case 3: 
            adding_hour = 0;
            adding_minute = 0;
            settingTimeState = HOUR_STATE;
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
        sprintf(setFoodTimesString, " Volver   HORARIOS");
        displayPositionStringWrite ( 0,0 , setFoodTimesString );

        if( get_food_mode() == CLOSED ){
            sprintf(setFoodTimesString, " Liberar hasta:%3dg", (int)get_food_load_required());
            displayPositionStringWrite ( 0,1 , setFoodTimesString );
            sprintf(setFoodTimesString, " Liberar siempre:OFF");
            displayPositionStringWrite ( 0,2 , setFoodTimesString );
        }
        else{
            sprintf(setFoodTimesString, " Liberar hasta:  OFF");
            displayPositionStringWrite ( 0,1 , setFoodTimesString );
            sprintf(setFoodTimesString, " Libera siempre:%3dg", (int)get_food_load_required());
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
            sprintf(setFoodTimesString, " %2d:%.2d", (int) food_time/6, food_time % 6 *60); 
            displayPositionStringWrite ( 0, i , setFoodTimesString );
        }
        set_user_cursor( displayUserPosition % 4 );
    break;}
    }
}



static void userInterfaceAddFoodTime(){
    char setTimeString[21] = "";
    // sprintf(setTimeString, "Nueva Hora *%2d:%.2d ", adding_hour, adding_minute);

    switch ( settingTimeState ){
    case HOUR_STATE:{
        if ( scroll.Up() ) {
            if( adding_hour < 59 )
                adding_hour++; // aumentar una hora
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            if( adding_hour > 0 )
                adding_hour--; // disminuir una hora
            scroll.disableDown();
        }
        if ( scroll.Pressed() ) {
            scroll.disablePressed();
            settingTimeState = MINUTE_STATE;
        }
        sprintf(setTimeString, "*%2d:%.2d ", adding_hour, adding_minute);
    break;}

    case MINUTE_STATE:{
        if ( scroll.Up() ) {
            if( adding_minute < 50)
                adding_minute = adding_minute + 10; // aumentar
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            if(adding_minute > 9 )
                adding_minute = adding_minute - 10; // disminuir
            scroll.disableDown();
        }
        if ( scroll.Pressed() ){
            settingTimeState = HOUR_STATE;
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
            add_food_time( adding_hour, adding_minute );
            displayClear();
            scroll.disablePressed();
            return;
        }
        sprintf(setTimeString, " %2d:*%.2d", adding_hour, adding_minute);
    break;}
    }

    sprintf(setTimeString, "%s%s","Nueva Hora ", setTimeString );
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
        sprintf(setFoodTimesString, "Liberar hasta:*%3d g", (int)get_food_load_required());
        displayPositionStringWrite ( 0,1 , setFoodTimesString );
        sprintf(setFoodTimesString, " Liberar siempre:OFF");
        displayPositionStringWrite ( 0,2 , setFoodTimesString );
    }
    else{
        sprintf(setFoodTimesString, " Liberar hasta:  OFF");
        displayPositionStringWrite ( 0,1 , setFoodTimesString );
        sprintf(setFoodTimesString, "Libera siempre:*%3dg", (int)get_food_load_required());
        displayPositionStringWrite ( 0,2 , setFoodTimesString );
    }
}

static void userInterfaceModifyFoodTime( ){
    char setTimeString[21] = "";
    food_time_t food_time_selected = get_time_for_food( index_food_time_selected );

    switch ( settingFoodTimeState ){
    case FOOD_HOUR_STATE:
        if ( scroll.Up() ) {
            food_time_selected = food_time_selected + hour_min_2_food_number( 1 , 0 ); 
            // aumentar 60 minutos
            scroll.disableUp();
        }
        else if ( scroll.Down() ){
            food_time_selected = food_time_selected - hour_min_2_food_number( 1 , 0 ); 
            // disminuir 60 minutos
            scroll.disableDown();
        }
        if ( scroll.Pressed() ) {
            scroll.disablePressed();
            settingFoodTimeState = FOOD_MINUTE_STATE;
        }
        change_food_time( food_time_selected , get_time_for_food( index_food_time_selected ));
        sprintf(setTimeString, "*%2d:%.2d    Spr  Listo", food_time_selected, food_time_selected);
    break;

    case FOOD_MINUTE_STATE:
        if ( scroll.Up() ) {
            food_time_selected = food_time_selected + hour_min_2_food_number( 0 , FOOD_TIME_MINUTES_INCREMENT ); 
            scroll.disableUp();
        }
        else if (scroll.Down() ){
            food_time_selected = food_time_selected - hour_min_2_food_number( 0, FOOD_TIME_MINUTES_INCREMENT ); 
            scroll.disableDown();
        }
        if ( scroll.Pressed() ){
            scroll.disablePressed();
            settingFoodTimeState = ASK_DELETE_TIME_STATE;
            
        }
        change_food_time( food_time_selected , get_time_for_food( index_food_time_selected ));
        sprintf(setTimeString, " %2d:*%.2d   Spr  Listo", food_time_selected, food_time_selected);
    break;
    case ASK_DELETE_TIME_STATE:
        if ( scroll.Pressed() ) {
            delete_food_time_in_position( index_food_time_selected );
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
            scroll.disablePressed();
            displayClear();
        }
        else if( scroll.Up() ) {
            scroll.disableUp();
            settingFoodTimeState = ASK_OK_TIME_STATE;
        }
        else if( scroll.Down() ) {
            scroll.disableDown();
            settingFoodTimeState = FOOD_MINUTE_STATE;
        }
        sprintf(setTimeString, "*%2d:%.2d   *Spr  Listo", food_time_selected, food_time_selected);
    break;
    case ASK_OK_TIME_STATE:
        if ( scroll.Pressed() ) {
            displayState = DISPLAY_AJUSTES_SET_FOOD_TIMES_STATE;
            scroll.disablePressed();
            displayClear();
        }
        else if( scroll.Down() ) {
            scroll.disableDown();
            settingFoodTimeState = ASK_DELETE_TIME_STATE;
        }
        sprintf(setTimeString, "*%2d:%.2d    Spr *Listo", food_time_selected, food_time_selected);
    break;
    }

    displayPositionStringWrite ( 0, index_food_time_selected%4 , setTimeString );
}


// DISPLAY en establecer tara del bowl
static void userInterfaceDisplayBowlTareStateUpdate()
{
    displayState = DISPLAY_AJUSTES_BOWL_TARE_STATE;

    char tareBowlString[21] = "";
    sprintf(tareBowlString, "Tara del bowl");
    displayPositionStringWrite ( 0,0 , tareBowlString );

    sprintf(tareBowlString, "  Coloque el bowl de");
    displayPositionStringWrite ( 0,1 , tareBowlString );

    sprintf(tareBowlString, " su mascota vacio");
    displayPositionStringWrite ( 0,2 , tareBowlString );

    sprintf(tareBowlString, "*Listo");
    displayPositionStringWrite ( 4,3 , tareBowlString );

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

void scrollUpdate(){
    scroll.Update();
}

void downUserPosition(){
    scroll.enableDown();
}

void upUserPosition(){
    scroll.enableUp();
}
