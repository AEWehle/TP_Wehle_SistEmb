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
	/*if(!this -> CLKState){
		if(this->CLK.read()){
			delay(2);
			CLKState = this->CLK.read();
		}
	}
	else{*/
    /*switch(scrollState){
    case HIGH_STATE:
		if(!CLKState){
            if(this->CLK.read()){
                delay(2);
                CLKState = this->CLK.read();
            }
	    }
	    else if(!this->CLK.read()){
			DTState = this->DT.read();
			delay(1);
			CLKState = !this->CLK.read();
			if(!CLKState && DTState == this->DT.read()){
				if(DTState)
					count++;
				else
					count--;
                scrollState = LOW_STATE;
            }
		}
        break;
    case LOW_STATE:
        if(CLKState){
            if(!this->CLK.read()){
                delay(2);
                CLKState = this->CLK.read();
            }
	    }
	    else if(this->CLK.read()){
			DTState = this->DT.read();
			delay(1);
			CLKState = this->CLK.read();
			if(CLKState && DTState == this->DT.read()){
				if(DTState)
					count++;
				else
					count--;
                scrollState = HIGH_STATE;
            }
		}
        break;
    default:
     scrollState = HIGH_STATE;
    }*/
	//}
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
    
    // scroll
    /*state=(state<<1) | this->CLK.read();
    char str[100];
	switch ( scrollState ) {
    case HIGH_STATE:*/
        /*if(     CLKState != this->CLK.read() ){ // si clk apretado entra
            DTState = this->DT.read();
            CLKState = this->CLK.read();
            scrollState = EDGE_STATE;
        }*/
       /* if (this->CLK.read() == LOW){
            this -> DTState = this->DT.read();
            scrollState = EDGE_STATE;
            sprintf(str, "HIGH a EDGE con DT: %d\n", DTState);
            pcSerialComStringWrite(str);
        }

        break;

    case LOW_STATE: 
        if (this->CLK.read() == HIGH){
            this -> DTState = this->DT.read();
            scrollState = EDGE_STATE;
            sprintf(str, "LOW a EDGE con DT: %d\n", DTState);
            pcSerialComStringWrite(str);
        }
        break;

    case EDGE_STATE: */
        /*if( CLKState == this->CLK.read() && DTState == this->DT.read() ) // checked debounce
        {
            if( CLKState != DTState )
            {
                // this -> scrollDown = false;
                this -> scrollUp = true;  
                print_display = true;
                count++;
            }
            else
            {
                this -> scrollDown = true;                
                // this -> scrollUp = false;
                print_display = true;
                count--;
            }
        }
        DTState = this->DT.read();
        CLKState = this->CLK.read();          
        scrollState = HIGH_STATE;*/
/*
        if (state==0x80){
            state=0x00;
            scrollState = LOW_STATE;
            if(this->DT.read() == this -> DTState){
                if(this->DT.read())
                    count++;
                else
                    count--;
                sprintf(str, "EDGE a LOW %s\n", DTState? "aumentando":"disminuyendo");
                pcSerialComStringWrite(str);
            }
        }
        if (state==0x7f){
            state=0xff;
            scrollState = HIGH_STATE;
            if(this->DT.read() == this -> DTState){
                if(!this->DT.read())
                    count++;
                else
                    count--;
                sprintf(str, "EDGE a HIGH %s\n", !DTState? "aumentando":"disminuyendo");
                pcSerialComStringWrite(str);    
            }
        }
        break;
    }*/
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
