//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "sd_card.h"

#include "event_log.h"
#include "date_and_time.h"
#include "pc_serial_com.h"

#include "FATFileSystem.h"
#include "SDBlockDevice.h"

#include "platform/mbed_retarget.h"

//=====[Declaration of private defines]========================================

#define SPI3_MOSI   PB_5
#define SPI3_MISO   PB_4
#define SPI3_SCK    PB_3
#define SPI3_CS     PA_4

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

// void strcat( char* str, const char* cat_str );
// int strlen( const char* str );

SDBlockDevice sd( SPI3_MOSI, SPI3_MISO, SPI3_SCK, SPI3_CS );

FATFileSystem sdCardFileSystem("sd", &sd);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

bool sdCardInit( bool SDenable ){
    pcSerialComStringWrite("Buscando un filesystem en la tarjeta SD... \r\n");
    if( !SDenable ) return false;
    sdCardFileSystem.mount(&sd);
    DIR *sdCardListOfDirectories = opendir("/sd/");
    if ( sdCardListOfDirectories != NULL ) {
        pcSerialComStringWrite("Se encontró un filesystem en la tarjeta SD. \r\n");
        closedir(sdCardListOfDirectories);
        return true;
    } else {
        pcSerialComStringWrite("Filesystem no encontrado. \r\n");
        pcSerialComStringWrite("Inserte una tarjeta SD y ");
        pcSerialComStringWrite("reinicie la placa.\r\n");
        return false;
    }
}

bool sdCardWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[SD_CARD_FILENAME_MAX_LENGTH+4] = "";
    
    fileNameSD[0] = '\0';
    sprintf( fileNameSD, "%s/sd/%s", fileNameSD, fileName );

    FILE *sdCardFilePointer = fopen( fileNameSD, "a" );

    if ( sdCardFilePointer != NULL ) {
        fprintf( sdCardFilePointer, "%s", writeBuffer );                       
        fclose( sdCardFilePointer );
        return true;
    } else {
        return false;
    }
}

bool sdCardCreateWriteFile( const char* fileName, const char* writeBuffer )
{
    char fileNameSD[SD_CARD_FILENAME_MAX_LENGTH+4] = "";
    
    fileNameSD[0] = '\0';
    sprintf( fileNameSD, "%s/sd/%s", fileNameSD, fileName );

    FILE *sdCardFilePointer = fopen( fileNameSD, "w" );

    if ( sdCardFilePointer != NULL ) {
        fprintf( sdCardFilePointer, "%s", writeBuffer );                       
        fclose( sdCardFilePointer );
        return true;
    } else {
        return false;
    }
}

bool sdCardReadFile( const char* fileName, char * readBuffer, int readBufferSize )
{
    char fileNameSD[SD_CARD_FILENAME_MAX_LENGTH+4] = "";
    int i;
    
    fileNameSD[0] = '\0';
    sprintf( fileNameSD, "%s/sd/%s", fileNameSD, fileName );
    
    FILE *sdCardFilePointer = fopen( fileNameSD, "r" );
    
    if ( sdCardFilePointer != NULL ) {
        pcSerialComStringWrite( "Abriendo archivo: " );
        pcSerialComStringWrite( fileNameSD );
        pcSerialComStringWrite( "\r\n" );

        i = 0;
        while ( ( !feof(sdCardFilePointer) ) && ( i < readBufferSize - 1 ) ) {
           fread( &readBuffer[i], 1, 1, sdCardFilePointer );
           i++;
        }
        readBuffer[i-1] = '\0';
        fclose( sdCardFilePointer );
        return true;
    } else {
        pcSerialComStringWrite( "Archivo no encontrado\r\n" );
        return false;
    }
}

bool sdCardListFiles( char* fileNamesBuffer, int fileNamesBufferSize )
{
    int NumberOfUsedBytesInBuffer = 0;
    struct dirent *sdCardDirectoryEntryPointer;

    DIR *sdCardListOfDirectories = opendir("/sd/");

    if ( sdCardListOfDirectories != NULL ) {
        pcSerialComStringWrite("Todos los archivos:\r\n");
        sdCardDirectoryEntryPointer = readdir(sdCardListOfDirectories);
        
        while ( ( sdCardDirectoryEntryPointer != NULL ) && 
                ( NumberOfUsedBytesInBuffer + strlen(sdCardDirectoryEntryPointer->d_name) < 
                    fileNamesBufferSize) ) {
            sprintf( fileNamesBuffer, "%s%s\r\n", fileNamesBuffer, sdCardDirectoryEntryPointer->d_name );
            NumberOfUsedBytesInBuffer = NumberOfUsedBytesInBuffer +
                                        strlen(sdCardDirectoryEntryPointer->d_name);
            sdCardDirectoryEntryPointer = readdir(sdCardListOfDirectories);
        }
        
        closedir(sdCardListOfDirectories);
        
        return true;
    } else {
        pcSerialComStringWrite("Ingrese una arjeta SD y ");
        pcSerialComStringWrite("reinicie la placa.\r\n");
        return false;
    }
}
