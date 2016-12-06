#ifndef UserRotate_h
#define UserRotate_h

#include <ClickEncoder.h>

class UserRotate{

	public: 
		UserRotate(int aWire, int bWire, char* name);
		int fetchCurrentAngle(); 
		int fetchCounterState();
		void timerIsr(); 
		int updateValue();
		void setResolutionPerClick(int16_t val); 
	private:
		ClickEncoder *_encoder; 
		int _currentAngle;
		int _wireA;
		int _wireB;  
		char _name [50]; 
		int16_t _counter; 
		int16_t _last; 
		int16_t _value; 
		int16_t _resolutionPerClick; 
}; 


#endif