#include "Arduino.h"	// for strcpy!
#include "UserRotate.h"

void UserRotate::timerIsr(){
	_encoder->service();
}

UserRotate::UserRotate(int aWire, int bWire, char* nameOfEncoder){
	strcpy(_name, nameOfEncoder );
	_encoder = new ClickEncoder(aWire, bWire, A4);
	_counter = 0; 
	_wireA = aWire; 
	_wireB = bWire; 
	_last = -1; 
	_resolutionPerClick = 5; 
}

int UserRotate::updateValue(){
	_value += _encoder->getValue();
	if(_value != _last){
		Serial.print(_name);
		Serial.print(" : ");
		Serial.print("Last: ");
		Serial.print(_last);
		Serial.print("Value: ");
		Serial.println(_value);
		Serial.println("HELLLOOO");
		if ( _last == 0 && _value == 1){
			_counter -= 1; 
			 Serial.print("currentCount = ");
			 Serial.println(_counter);
			if (_counter < -1){    // set a minimum on the degree that we are 
				_counter = 0;         // using 
			}
		} else if (_last == 1 && _value == 2){
			_counter += 1; 
			Serial.print("currentCount = ");
			Serial.println(_counter);
		}
		_last = _value;
		return _counter * _resolutionPerClick;  
	}
	return -1; 
}


void UserRotate::setResolutionPerClick(int16_t val){
	_resolutionPerClick = val; 
}
