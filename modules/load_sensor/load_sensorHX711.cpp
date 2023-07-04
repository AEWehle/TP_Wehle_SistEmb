/**
 *
 * HX711 library for Arduino
 * https://github.com/bogde/HX711
 *
 * MIT License
 * (c) 2018 Bogdan Necula
 *
**/

#include "load_sensorHX711.h"
#include "pc_serial_com.h"

uint8_t shiftIn(DigitalIn& dataPin, DigitalOut& clockPin, uint8_t bitOrder) {
    uint8_t value = 0;
    uint8_t i;
 
    for (i = 0; i < 8; ++i) {
        clockPin.write(ON);
        if (bitOrder == LSBFIRST)
            value |= (dataPin.read()!=OFF) << i;
        else
            value |= (dataPin.read()!=OFF) << (7 - i);
        clockPin.write(OFF);
    }
    return value;
}

HX711::HX711(PinName dout, PinName pd_sck, int gain) : PD_SCK(pd_sck), DOUT(dout){
    DOUT.mode(PullUp);
	set_gain(gain);
}

HX711::~HX711() {
}

bool HX711::is_ready() {
	return DOUT.read() == OFF;
}

void HX711::set_gain(uint8_t gain) {
	switch (gain) {
		case 128:		// channel A, gain factor 128
			GAIN = 1;
			break;
		case 64:		// channel A, gain factor 64
			GAIN = 3;
			break;
		case 32:		// channel B, gain factor 32
			GAIN = 2;
			break;
	}

}

long HX711::read() {

	// Wait for the chip to become ready.
	// wait_ready();

	// Define structures for reading data into.
	unsigned long value = 0;
	uint8_t data[3] = { 0 };
	uint8_t filler = 0x00;

	// Pulse the clock pin 24 times to read the data.
	data[2] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[1] = shiftIn(DOUT, PD_SCK, MSBFIRST);
	data[0] = shiftIn(DOUT, PD_SCK, MSBFIRST);

	// Set the channel and the gain factor for the next reading using the clock pin.
	for (unsigned int i = 0; i < GAIN; i++) {
		PD_SCK.write(ON);
		delay(1);
		PD_SCK.write(OFF);
		delay(1);
	}

	// Replicate the most significant bit to pad out a 32-bit signed integer
	if (data[2] & 0x80) {
		filler = 0xFF;
	} else {
		filler = 0x00;
	}

	// Construct a 32-bit signed integer
	value = ( static_cast<unsigned long>(filler) << 24
			| static_cast<unsigned long>(data[2]) << 16
			| static_cast<unsigned long>(data[1]) << 8
			| static_cast<unsigned long>(data[0]) );

	return static_cast<long>(value);
}

void HX711::wait_ready(unsigned long delay_ms) {
	// Wait for the chip to become ready.
	// This is a blocking implementation and will
	// halt the sketch until a load cell is connected.

	while (!is_ready()) {
		// Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
		// https://github.com/bogde/HX711/issues/73
		delay(delay_ms);
	}
}

bool HX711::wait_ready_retry(int retries, unsigned long delay_ms) {
	// Wait for the chip to become ready by
	// retrying for a specified amount of attempts.
	// https://github.com/bogde/HX711/issues/76
	int count = 0;
	while (count < retries) {
		if (is_ready()) {
			return true;
		}
		delay(delay_ms);
		count++;
	}
	return false;
}


long HX711::read_average(uint8_t times) {
	long sum = 0;
	for (uint8_t i = 0; i < times; i++) {
		sum += read();
		// Probably will do no harm on AVR but will feed the Watchdog Timer (WDT) on ESP.
		// https://github.com/bogde/HX711/issues/73
		delay(0);
	}
	return sum / times;
}

double HX711::get_value(uint8_t times) {
	return read_average(times) - OFFSET;
}

float HX711::get_units(uint8_t times) {
	return get_value(times) / SCALE;
}

void HX711::tare(uint8_t times) {
	double sum = read_average(times);
	set_offset(sum);
}

void HX711::set_scale(float scale) {
	SCALE = scale;
}

float HX711::get_scale() {
	return SCALE;
}

void HX711::set_offset(long offset) {
	OFFSET = offset;
}

long HX711::get_offset() {
	return OFFSET;
}

void HX711::power_down() {
	PD_SCK.write(OFF);
	PD_SCK.write(ON);
}

void HX711::power_up() {
	PD_SCK.write(OFF);
}