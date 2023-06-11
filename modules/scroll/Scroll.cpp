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
	this->CLKState = false;
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
    
    // scroll
	switch ( scrollState ) {
    case HIGH_STATE:
        if(     CLKState != this->CLK.read() ){ // si clk apretado entra
            DTState = this->DT.read();
            CLKState = this->CLK.read();
            scrollState = EDGE_STATE;
        }

        break;

    case LOW_STATE: 
    case EDGE_STATE: 
        if( CLKState == this->CLK.read() && DTState == this->DT.read() ) // checked debounce
        {
            if( CLKState != DTState )
            {
                this -> scrollDown = false;
                this -> scrollUp = true;  
            }
            else
            {
                this -> scrollDown = true;                
                this -> scrollUp = false;
            }
        }          
        scrollState = HIGH_STATE;
        break;
    }
}

void Scroll::disablePressed(){
	SWPressed = false;
}

bool Scroll::Pressed(){
	return SWPressed;
}

void Scroll::disableUp(){
	scrollUp = false;
}

bool Scroll::Up(){
	return scrollUp;
}

void Scroll::disableDown(){
	scrollDown = false;
}

bool Scroll::Down(){
	return scrollDown;
}

// Scroll::~Scroll() {
// 	// TODO Auto-generated destructor stub
// }
