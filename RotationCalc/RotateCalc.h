#ifndef RotateCalc_h 
#define RotateCalc_h

#include "RotationUtil.h"

class RotateCalc{
	public:

		RotateCalc(int axisCoords[3][3],int angle [3]); 
		void fetchRotationResult(float inputCoord[3], float result[3]); 

		void fetchRotatorAngles(float angles [2]);
		void updateRotatorAngle(int number, int angle);
	private:
		RotationUtil _rotatorOne; 
		RotationUtil _rotatorTwo; 
		float _preComputedXResult; 
		float _preComputedYResult; 
		float _preComputedZResult; 
}; 

#endif