//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "pc_serial_com.h"

#include "date_and_time.h"
#include "event_log.h"
#include "motor.h"
#include "sd_card.h"
#include "bowl.h"
#include "food_storage.h"
#include "time_for_food.h"
#include "user_interface.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

typedef enum{
    PC_SERIAL_GET_FILE_NAME,
    PC_SERIAL_COMMANDS,
} pcSerialComMode_t;

//=====[Declaration and initialization of public global objects]===============

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static pcSerialComMode_t pcSerialComMode = PC_SERIAL_COMMANDS;

static int numberOfCharsInFileName = 0;

static char fileName[SD_CARD_FILENAME_MAX_LENGTH] = "";

//=====[Declarations (prototypes) of private functions]========================

// int strlen( const char* str );
// void strcat( char* str, const char* cat_str );

static void pcSerialComCharWrite( char chr );
static void pcSerialComStringRead( char* str, int strLength );

static void pcSerialComGetFileName( char receivedChar );
static void pcSerialComShowSdCardFile( char * fileName );

static void pcSerialComCommandUpdate( char receivedChar );


static void availableCommands();
static void commandShowCurrentUnderStorageDetectorState();
static void commandShowCurrentFoodLoadState();
static void commandSetDateAndTime();
static void commandShowDateAndTime();
static void commandShowReleaseFood();
static void commandShowBowlTare();
static void commandShowFoodTimes();
static void commandShowStoredEvents();
static void commandShowCurrentMotorState();
static void commandEventLogSaveToSdCard();
static void commandSdCardListFiles();
static void commandGetFileName();

//=====[Implementations of public functions]===================================

void pcSerialComInit()
{
    availableCommands();
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}

static void pcSerialComStringRead( char* str, int strLength )
{
    int strIndex;
    for ( strIndex = 0; strIndex < strLength; strIndex++) {
        uartUsb.read( &str[strIndex] , 1 );
        uartUsb.write( &str[strIndex] ,1 );
    }
    str[strLength]='\0';
}

void pcSerialComStringWrite( const char* str )
{
    uartUsb.write( str, strlen(str) );
}

void pcSerialComIntWrite( int number )
{
    char str[4] = "";
    sprintf( str, "%d", number );
    pcSerialComStringWrite( str );
}

void pcSerialComCharWrite( char chr )
{
    char str[3] = "";
    sprintf( str, "%c", chr );
    pcSerialComStringWrite( str );
}

void pcSerialComUpdate()
{  
// pcSerialComStringWrite( "pc serial com update" );
    char receivedChar = pcSerialComCharRead();
    if( receivedChar != '\0' ) {
        switch ( pcSerialComMode ) {
            case PC_SERIAL_GET_FILE_NAME:
                pcSerialComGetFileName( receivedChar );
            break;
            case PC_SERIAL_COMMANDS:
                pcSerialComCommandUpdate( receivedChar );
            break;
            default:
                pcSerialComMode = PC_SERIAL_COMMANDS;
            break;
        }
    }    
}


static void pcSerialComCommandUpdate( char receivedChar )
{
    switch (receivedChar) {
        case 'a': case 'A': commandShowCurrentUnderStorageDetectorState(); break;
        case 'c': case 'C': commandShowCurrentFoodLoadState(); break;
        case 's': case 'S': commandSetDateAndTime(); break;
        case 't': case 'T': commandShowDateAndTime(); break;
        case 'x': case 'X': commandShowReleaseFood(); break;
        case 'b': case 'B': commandShowBowlTare(); break;
        case 'h': case 'H': commandShowFoodTimes(); break;
        case 'e': case 'E': commandShowStoredEvents(); break;
        case 'm': case 'M': commandShowCurrentMotorState(); break;
        case 'g': case 'G': commandEventLogSaveToSdCard(); break;
        case 'l': case 'L': commandSdCardListFiles(); break;
        case 'o': case 'O': commandGetFileName(); break;
        case 'q': case 'Q': downUserPosition(); break;
        case 'w': case 'W': upUserPosition(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "COMANDOS DISPONIBLES:\r\n" );
    pcSerialComStringWrite( "Presione 'a' o 'A' para saber si hay bajo almacenamiento\r\n" );
    pcSerialComStringWrite( "Presione 'c' o 'C' para obtener el peso actual de comida en bowl\r\n" );
    pcSerialComStringWrite( "Presione 's' o 'S' para setear fecha y hora actual\r\n" );
    pcSerialComStringWrite( "Presione 't' o 'T' para obtener la fecha y hora actual\r\n" );
    pcSerialComStringWrite( "Presione 'x' o 'X' para liberar un poco de comida\r\n" );
    pcSerialComStringWrite( "Presione 'b' o 'B' para establecer la tara el bowl\r\n" );
    pcSerialComStringWrite( "Presione 'h' o 'H' para ver o modificar los horarios de comida\r\n" );
    pcSerialComStringWrite( "Presione 'e' o 'E' para obtener los eventos guardados\r\n" );
    pcSerialComStringWrite( "Presione 'm' o 'M' para mostrar el estado del motor\r\n" );
    pcSerialComStringWrite( "Presione 'g' o 'G' para guardar los eventos en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "Presione 'l' o 'L' para listar los archivos en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "Presione 'o' o 'O' para mostar las carpetas en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "Presione 'q' o 'Q' para BAJAR en display\r\n" );
    pcSerialComStringWrite( "Presione 'w' o 'W' para SUBIR en display\r\n" );
    pcSerialComStringWrite( "\r\n" );
}


static void commandShowCurrentUnderStorageDetectorState()
{
    if ( getStorageState() == LOW_STORAGE)
        pcSerialComStringWrite( "Hay poco almacenamiento de comida.\r\n");
    else if( getStorageState() == OK_STORAGE)
        pcSerialComStringWrite( "Hay suficiente almacenamiento de comida.\r\n");
    else
        pcSerialComStringWrite( "El almacenamiento de comida está vacío.\r\n");
}

static void commandShowCurrentFoodLoadState()
{
    char str[100] = "";
    sprintf ( str, "El peso actual de comida en el bowl es %.2f gr.\r\n", get_food_load());
    pcSerialComStringWrite( str );
    
}


static void commandShowReleaseFood()
{
    char str[100] = "";
    sprintf ( str, "Liberando un poco de comida.\r\n");
    bowl_charge( 30.0 );
    pcSerialComStringWrite( str );
}


static void commandShowBowlTare()
{
    char str[100] = "";
    sprintf ( str, "Tara establecida.\n\rSi el bowl no estaba vacío, vacíelo y vuelva a presionar x o X.\r\n");
    pcSerialComStringWrite( str );
    bowl_tare();
}

static void commandShowFoodTimes()
{
    char str[100] = "Horarios:";
    char cat_str[10] = "";
    pcSerialComStringWrite( str );
    int qtimes = get_times_q();
    int food_time;
    for (int i = 0 ; i < qtimes ; i++){
        food_time = get_time_for_food( i );
        sprintf( str, " %d:%d,", (int) food_time/6, food_time % 6 *60 );
        pcSerialComStringWrite( str ); 
    }
    pcSerialComStringWrite( "\r\n" );

    char input;
    pcSerialComStringWrite( "\r\nPara borrar presione x o X, para agregar presione n o N" );
    input = pcSerialComCharRead();
    pcSerialComStringWrite("\r\n");
    switch ( input ) {
        case 'n': case 'N':{
            char str_minute[3] = "";
            char str_hour[3] = "";
            pcSerialComStringWrite("Escriba dos dígitos para la hora (00-23): ");
            pcSerialComStringRead( str_hour, 2);
            pcSerialComStringWrite("\r\n");

            pcSerialComStringWrite("Escriba dos dígitos para el minuto (00-59): ");
            pcSerialComStringRead( str_minute, 2);
            pcSerialComStringWrite("\r\n");
            int hour = char2int( str_hour[0] )*10 + char2int( str_hour[1] );
            int minute = char2int( str_minute[0] )*10 + char2int( str_minute[1] );
            add_food_time( hour, minute );
        break;}
        case 'x': case 'X':{
            pcSerialComStringWrite( "Seleccione desde 1 el horario a borrar" );
            input = pcSerialComCharRead();
            pcSerialComStringWrite("\r\n");
            int position = char2int( input );
            delete_food_time_in_position( position - 1 );
        break;}  
    }
}


static void commandEventLogSaveToSdCard()
{
    eventLogSaveToSdCard();
}

static void commandSdCardListFiles()
{
    char fileListBuffer[SD_CARD_MAX_FILE_LIST*SD_CARD_FILENAME_MAX_LENGTH] = "";
    sdCardListFiles( fileListBuffer, 
                     SD_CARD_MAX_FILE_LIST*SD_CARD_FILENAME_MAX_LENGTH );
    pcSerialComStringWrite( fileListBuffer );
    pcSerialComStringWrite( "\r\n" );
}

static void commandSetDateAndTime()
{
    char year[5] = "";
    char month[3] = "";
    char day[3] = "";
    char hour[3] = "";
    char minute[3] = "";
    char second[3] = "";
    
    pcSerialComStringWrite("\r\nEscriba los cuatro dígitos del año (AAAA): ");
    pcSerialComStringRead( year, 4);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Escriba dos dígitos para el mes actual (01-12): ");
    pcSerialComStringRead( month, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Escriba dos dígitos para el día actual (01-31): ");
    pcSerialComStringRead( day, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Escriba dos dígitos para la hora actual (00-23): ");
    pcSerialComStringRead( hour, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Escriba dos dígitos para el minuto actual (00-59): ");
    pcSerialComStringRead( minute, 2);
    pcSerialComStringWrite("\r\n");

    pcSerialComStringWrite("Escriba dos dígitos para los segundos (00-59): ");
    pcSerialComStringRead( second, 2);
    pcSerialComStringWrite("\r\n");
    
    pcSerialComStringWrite("Se estableció la fecha y hora\r\n");

    dateAndTimeWrite( atoi(year), atoi(month), atoi(day), 
        atoi(hour), atoi(minute), atoi(second) );
}

static void commandShowDateAndTime()
{
    char str[100] = "";
    sprintf ( str, "Fecha y hora = %s", dateAndTimeRead() );
    pcSerialComStringWrite( str );
    pcSerialComStringWrite("\r\n");
}

static void commandShowStoredEvents()
{
    char str[EVENT_STR_LENGTH] = "";
    int i;
    for (i = 0; i < eventLogNumberOfStoredEvents(); i++) {
        eventLogRead( i, str );
        pcSerialComStringWrite( str );   
        pcSerialComStringWrite( "\r\n" );                    
    }
}

static void commandShowCurrentMotorState()
{
    switch ( motorStateRead() ) {
        case STOPPED: 
            pcSerialComStringWrite( "Motor parado\r\n"); break;
        case ACTIVE: 
            pcSerialComStringWrite( "Motor girando.\r\n"); break;
    }
}

static void commandGetFileName()
{
    pcSerialComStringWrite( "Ingresar el nombre del archivo \r\n" );
    pcSerialComMode = PC_SERIAL_GET_FILE_NAME ;
    numberOfCharsInFileName = 0;
}

static void pcSerialComGetFileName( char receivedChar )
{
    if ( (receivedChar == '\r') &&
         (numberOfCharsInFileName < SD_CARD_FILENAME_MAX_LENGTH) ) {
        pcSerialComMode = PC_SERIAL_COMMANDS;
        fileName[numberOfCharsInFileName] = '\0';
        numberOfCharsInFileName = 0;
        pcSerialComShowSdCardFile( fileName );
    } else {
        fileName[numberOfCharsInFileName] = receivedChar;
        pcSerialComCharWrite( receivedChar );
        numberOfCharsInFileName++;
    }
}

static void pcSerialComShowSdCardFile( char* fileName ) 
{
    char fileContentBuffer[EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE] = "";
    pcSerialComStringWrite( "\r\n" );
    if ( sdCardReadFile( fileName, fileContentBuffer,
                         EVENT_STR_LENGTH*EVENT_LOG_MAX_STORAGE ) ) {
        pcSerialComStringWrite( "El contenido del archivo es:\r\n");
        pcSerialComStringWrite( fileContentBuffer );
        pcSerialComStringWrite( "\r\n" );
    }
}