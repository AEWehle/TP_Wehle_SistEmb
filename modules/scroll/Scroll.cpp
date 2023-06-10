#include "Scroll.h"


//=====[Declaration of private defines]========================================

#define DEBOUNCE_TIME_SCROLL_MS  SYSTEM_TIME_INCREMENT_MS

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

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
    this -> scrollUp = false;
    this -> scrollDown = false;
}

void Scroll::Update(){
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
            buttonState = LOW_STATE;
        }
        else {
            SWPressed = false;
            buttonState = HIGH_STATE;
        }
        break;
    }
    
	switch ( scrollState ) {
    case HIGH_STATE:
        if(  !this->CLK.read() ){ // si clk apretado entra
            DTState = this->DT.read() ;
            scrollState = EDGE_STATE;
        }
        this -> scrollDown = false;
        this -> scrollUp = false;
        break;
    case LOW_STATE:
    case EDGE_STATE: 
        if( !this->CLK.read() && DTState == this->DT.read() ){ //si clk sigue apretado y DT no cambió
            if( DTState ){ // horario
                this -> scrollDown = false;
                this -> scrollUp = true;  
            }
            else{          // antihorario
                this -> scrollDown = true;                
                this -> scrollUp = false;
            }
        }
        else
            scrollState = HIGH_STATE;
        break;
    }
}

bool Scroll::Pressed(){
	return SWPressed;
}

bool Scroll::Up(){
	return scrollUp;
}

bool Scroll::Down(){
	return scrollDown;
}

// Scroll::~Scroll() {
// 	// TODO Auto-generated destructor stub
// }
