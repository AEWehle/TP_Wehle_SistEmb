//=====[#include guards - begin]===============================================

#ifndef _SCROLL_H_
#define _SCROLL_H_

#include "mbed.h"
#include "arm_book_lib.h"

//=====[Declaration of public defines]=========================================

typedef enum {
    HIGH_STATE,
    LOW_STATE ,
    EDGE_STATE
} encoderState_t;

//=====[Declaration of public data types]======================================

class Scroll{
private:
	DigitalIn CLK;	//pin CLK
	DigitalIn DT;	//pin DT
	DigitalIn SW;	//pin del pulsador
	bool SWPressed;	
	bool DTState;	
	bool CLKState;	
    encoderState_t buttonState;
    encoderState_t scrollState;
    bool scrollUp;
    bool scrollDown;

//=====[Declarations (prototypes) of public functions]=========================
public:

Scroll(DigitalIn CLK_pin, DigitalIn DT_pin, DigitalIn SW_pin);
void Update();
bool Pressed();
bool Down();
bool Up();
void disablePressed();
void disableUp();
void disableDown();
void enableUp();
void enableDown();
void enablePressed();
};

//=====[#include guards - end]=================================================

#endif // _SCROLL_H_
