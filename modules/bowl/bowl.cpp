//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "bowl.h"
#include "load_sensorHX711.h"
#include "motor.h"

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
static int time_count_bowl = 0;

//=====[Declarations (prototypes) of private functions]========================

//=====[Implementations of public functions]===================================

//=====[Para calibrar la celda de carga]=======================================

void bowl_tare() {
//   Serial.begin(9600);
//   balanza.begin(DOUT, CLK);
//   Serial.print("Lectura del valor del ADC:t");
//   Serial.println(balanza.read());
    balanza.read();
//   Serial.println("No ponga ningún objeto sobre la balanza");
//   Serial.println("Destarando...");
    balanza.set_scale(); //La escala por defecto es 1
    balanza.tare(20);  //El peso actual es considerado Tara.
    // Serial.println("Coloque un peso conocido:");
}


// calibrar escala luego de tara
void bowl_calibrate() {
//   Serial.begin(9600);
//   balanza.begin(DOUT, CLK);
//   Serial.print("Lectura del valor del ADC:  ");
//   Serial.println(balanza.read());
     float test_load = 200; // para medir en gramos probamos en gramos, en este caso 200
     float scale = balanza.read_average(20)/test_load;
//   Serial.println("No ponga ningun  objeto sobre la balanza");
//   Serial.println("Destarando...");
//   Serial.println("...");
    balanza.set_scale(scale); // Establecemos la escala
}


void bowlInit()
{
    bowl_tare();
}

void bowl_charge(){
    motorActivation();
    chargingState = ON;
}

float get_food_load() {
    food_load = balanza.get_units(20);
    return food_load;
}

void bowlUpdate()
{
    if( get)
    if( (get_food_load() > food_load_required) && chargingState ){
        motorDeactivation();
        chargingState = OFF;
    }

    time_count_bowl++;
    if ( time_count_bowl >= MINUTE_BOWL ){
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

void set_food_load_required(int max_load){
    food_load_required = max_load;
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
