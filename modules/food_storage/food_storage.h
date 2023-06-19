//=====[#include guards - begin]===============================================

#ifndef _FOOD_STORAGE_H_
#define _FOOD_STORAGE_H_

//=====[Declaration of public defines]=========================================

typedef enum{
    EMPTY_STORAGE,
    LOW_STORAGE,
    OK_STORAGE
} storage_state_t;

//=====[Declaration of public data types]======================================

//=====[Declarations (prototypes) of public functions]=========================

void foodStorageInit();
void foodStorageUpdate();

storage_state_t getStorageState();
void setEmptyStorage();

//=====[#include guards - end]=================================================

#endif // _FOOD_STORAGE_H_
