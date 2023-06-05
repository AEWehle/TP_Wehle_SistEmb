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

void pcSerialComUpdate()
{
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
        case 'e': case 'E': commandShowStoredEvents(); break;
        case 'm': case 'M': commandShowCurrentMotorState(); break;
        case 'g': case 'G': commandEventLogSaveToSdCard(); break;
        case 'l': case 'L': commandSdCardListFiles(); break;
        case 'o': case 'O': commandGetFileName(); break;
        default: availableCommands(); break;
    } 
}

static void availableCommands()
{
    pcSerialComStringWrite( "Available commands:\r\n" );
    pcSerialComStringWrite( "Presione 'a' o 'A' para saber si hay bajo almacenamiento\r\n" );
    pcSerialComStringWrite( "Presione 'c' o 'C' para obtener el peso actual de comida en bowl\r\n" );
    pcSerialComStringWrite( "Presione 's' o 'S' para setear fecha y hora\r\n" );
    pcSerialComStringWrite( "Presione 't' o 'T' para obtener la fecha y hora\r\n" );
    pcSerialComStringWrite( "Presione 'e' o 'E' para obtener los eventos guardados\r\n" );
    pcSerialComStringWrite( "Presione 'm' o 'M' para mostrar el estado del motor\r\n" );
    pcSerialComStringWrite( "Presione 'g' o 'G' para guardar los eventos en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "Presione 'l' o 'L' para listar los archivos en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "Presione 'o' o 'O' para mostar las carpetas en la tarjeta SD\r\n" );
    pcSerialComStringWrite( "\r\n" );
}


static void commandShowCurrentUnderStorageDetectorState()
{
    if (underStorageDetectorStateRead() ) {
        pcSerialComStringWrite( "Hay poco almacenamiento de comida\r\n");
    } else {
        pcSerialComStringWrite( "Hay suficiente almacenamiento de comida\r\n");
    }    
}

static void commandShowCurrentFoodLoadState()
{
    char str[100] = "";
    sprintf ( str, "El peso actual de comida en el bowl es %.2f gr.\r\n", get_food_load());
    pcSerialComStringWrite( str );
    
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
    switch ( motorDirectionRead() ) {
        case STOPPED: 
            pcSerialComStringWrite( "El motor está parado\r\n"); break;
        case DIRECTION_1: 
            pcSerialComStringWrite( "Motor girando en la dirección 1\r\n"); break;
        case DIRECTION_2: 
            pcSerialComStringWrite( "Motor girando en la dirección 2\r\n"); break;
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