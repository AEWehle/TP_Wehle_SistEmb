//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "bowl.h"
#include "load_sensorHX711.h"
#include "motor.h"
#include "food_storage.h"
// #include "siren.h"

// sacar, solo prueba
#include "pc_serial_com.h"

//=====[Declaration of private defines]========================================

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

const PinName DOUT = A1;
const PinName CLK = A0;
HX711 balanza(DOUT, CLK);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool chargingState = OFF;

static bool foodIncreasedDetected      = OFF;
static bool foodDecreasedDetected      = OFF;

static float food_load_required = 100;  // lo establece el usuario
static float food_load = 0; // en gramos
static float last_minute_food_load = 0; // en gramos el peso de hace 1 minuto atras
static float init_food_load;

static int time_count_bowl = 0;
int initial_time_releasing_food;
const float SCALE = 7037.0/118; // para un peso de 118g

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

//=====[Para calibrar la celda de carga]=======================================

void bowl_tare() {
    balanza.set_offset(balanza.get_value(10)); //tare
}


// calibrar escala luego de tara
void bowl_calibrate() {
//   Serial.print("Lectura del valor del ADC:  ");
    // char str[100]="";
    // sprintf(str,"Lectura del valor del ADC: $d\r\n", balanza.read());
    // pcSerialComStringWrite(str);
    // float test_load = 200; // para medir en gramos probamos en gramos, en este caso 200
    // pcSerialComStringWrite("No ponga ningun objeto sobre la balanza\r\nDestarando..\r\n");
    // float scale = balanza.read_average(20)/test_load;
}


void bowlInit()
{   
    delay(500);
    bowl_tare();
    balanza.set_scale(SCALE);
    chargingState = OFF;
}

void bowl_charge( float food_to_add ){
    motorActivation();
    init_food_load = get_food_load();
    food_load_required = init_food_load + food_to_add;
    chargingState = ON;
    initial_time_releasing_food = time (NULL);
}

float get_food_load() {
    return food_load;
}

void bowlUpdate()
{
    food_load = balanza.get_units(10);
    if( chargingState ){
        if ( time(NULL) >= (MAX_TIME_RELEASING_FOOD_SECONDS + initial_time_releasing_food)){
            if (food_load < init_food_load + 10) {
                motorDeactivation();
                chargingState = OFF;
                setEmptyStorage();
            }
            else {
                init_food_load = food_load;
                initial_time_releasing_food = time(NULL);
            }
        }
        if (food_load > food_load_required) {        
            motorDeactivation();
            chargingState = OFF;
        }
    }

    time_count_bowl++;
    if ( time_count_bowl >= MINUTE_BOWL_SECONDS ){
        time_count_bowl = 0;

        if( last_minute_food_load > (food_load * (1 + TOLERANCIA))){
            foodDecreasedDetected = ON;
            foodIncreasedDetected = OFF;
            last_minute_food_load = food_load;
        }
        else if( last_minute_food_load < (food_load * (1 - TOLERANCIA))) {
            foodDecreasedDetected = OFF;
            foodIncreasedDetected = ON;
            last_minute_food_load = food_load;
        }
        else{
            foodDecreasedDetected = OFF;
            foodIncreasedDetected = OFF;
        }
    }
}

float  get_last_minute_food_load(){
    return last_minute_food_load;
}


bool foodIncreasedStateRead()
{
    return foodIncreasedDetected;
}


bool foodDecreasedStateRead()
{
    return foodDecreasedDetected;
}



//=====[Implementations of private functions]==================================
