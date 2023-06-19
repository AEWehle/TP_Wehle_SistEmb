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
            print_display = true;
            buttonState = LOW_STATE;
        }
        else {
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
                // this -> scrollDown = false;
                this -> scrollUp = true;  
                print_display = true;
            }
            else
            {
                this -> scrollDown = true;                
                // this -> scrollUp = false;
                print_display = true;
            }
        }          
        scrollState = HIGH_STATE;
        break;
    }
}

void Scroll::disablePressed(){
	this -> SWPressed = false;
}

bool Scroll::Pressed(){
	return this -> SWPressed;
}

void Scroll::disableUp(){
	this -> scrollUp = false;
}
void Scroll::enableUp(){
	this -> scrollUp = true;
}

bool Scroll::Up(){
	return this -> scrollUp;
}

void Scroll::disableDown(){
	this -> scrollDown = false;
}
void Scroll::enableDown(){
	this -> scrollDown = true;
}

bool Scroll::Down(){
	return this -> scrollDown;
}

// Scroll::~Scroll() {
// 	// TODO Auto-generated destructor stub
// }
