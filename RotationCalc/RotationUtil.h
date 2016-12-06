#ifndef RotationUtil_h
#define RotationUtil_h

#include "Arduino.h"
class RotationUtil
{
	public:
        RotationUtil();
		RotationUtil(int axisCoords[3], int angle);
        void setAngleOfRotation(int degree); 
        float fetchAngleOfRotation();
        void setAxisCoordinates(int coord [3]); 
        void getRotationResult(float inputCoord[3], float  result[3]); 
    
	private: 
        void preComputeValues();
        float convertDegToRad(int degree);

	    float _axisCoordinates [3];
        float _sineResult; 
        float _cosineResult; 
        float _angleOfRotation;

        float _sumsSquared; 
        float _squareOfSums; 
        float _lastProduct; 
};


#endif
