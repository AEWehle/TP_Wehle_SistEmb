#include "Scroll.h"
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

#define DEBOUNCE_TIME_SCROLL_MS  SYSTEM_TIME_INCREMENT_MS

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============
bool print_display = false;

//=====[Declaration and initialization of private global variables]============

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

Scroll::Scroll(DigitalIn CLK_pin, DigitalIn DT_pin, DigitalIn SW_pin): CLK(CLK_pin), DT(DT_pin), SW(SW_pin){
    CLK.mode(PullUp);
    DT.mode(PullUp);
    SW.mode(PullUp);
    buttonState = HIGH_STATE;
    scrollState = HIGH_STATE;
	this->SWPressed = false;
	this->DTState = false;
	this->CLKState = false;
    this -> scrollUp = false;
    this -> scrollDown = false;
    this -> count = 0;
    this -> state = 0;
}

void Scroll::proc_change(int s){
    static int scrollState2 = s;
    switch(scrollState2){
        case 0:
            if(s == 2) count++;
        break;
        case 1:
            if(s == 3) count--;
        break;
        case 2:
            if(s == 0) count--;
        break;
        case 3:
            if(s == 1) count++;
        break;
    }
    scrollState2 = s;
}


void Scroll::Update(){
    static int s = 0, s_ant = 0;
    
    s = CLK.read() << 1 | DT.read();
    if(s != s_ant){
        delay(1);
        if(s == (CLK.read() << 1 | DT.read())){
            proc_change(s);
        }
    }
    s_ant = s;

	switch ( buttonState ) {
    case HIGH_STATE:
        if ( !this -> SW.read() ){
            buttonState = EDGE_STATE;
        }
        break;
    case LOW_STATE:
        if ( this -> SW.read() ){
            buttonState = EDGE_STATE;
        }
        break;
    case EDGE_STATE:
        if ( !this -> SW.read() ){
            SWPressed = true;
            print_display = true;
            buttonState = LOW_STATE;
        }
        else {
            buttonState = HIGH_STATE;
        }
        break;
    }
}

void Scroll::disablePressed(){
	this -> SWPressed = false;
}

void Scroll::enablePressed(){
	this -> SWPressed = true;
}

bool Scroll::Pressed(){
	return this -> SWPressed;
}

void Scroll::disableUp(){
	// this -> scrollUp = false;
    count = 0;
}
void Scroll::enableUp(){
	// this -> scrollUp = true;
    count = 100;
}

bool Scroll::Up(){
    return count > 0;
	return this -> scrollUp;
}

void Scroll::disableDown(){
	// this -> scrollDown = false;
    count = 0;
}
void Scroll::enableDown(){
	// this -> scrollDown = true;
    count = -100;
}

bool Scroll::Down(){
    return count < 0;
	// return this -> scrollDown;
}

// Scroll::~Scroll() {
// 	// TODO Auto-generated destructor stub
// }
