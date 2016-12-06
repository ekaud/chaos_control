#include "Arduino.h"
#include "RotationUtil.h"

const float pointOfR [3]  = {2000,2000,0};
const float deg_to_rad = 57.2958; 
const float preComputedSine [73] = {0.00000,
0.08716,0.17365,0.25882,0.34202,0.42262,0.50000,0.57358,0.64279,
0.70711,0.76604,0.81915,0.86603,0.90631,0.93969,0.96593,0.98481,
0.99619,1.00000,0.99619,0.98481,0.96593,0.93969,0.90631,0.86603,
0.81915,0.76604,0.70711,0.64279,0.57358,0.50000,0.42262,0.34202,
0.25882,0.17365,0.08716,0.00000,-0.08715,-0.17365,-0.25882,-0.34202,
-0.42262,-0.50000,-0.57358,-0.64279,-0.70711,-0.76604,-0.81915,-0.86602,
-0.90631,-0.93969,-0.96593,-0.98481,-0.99619,-1.00000,-0.99619,-0.98481,
-0.96593,-0.93969,-0.90631,-0.86603,-0.81915,-0.76605,-0.70711,-0.64279,
-0.57358,-0.50000,-0.42262,-0.34202,-0.25882,-0.17365,-0.08716,-0.00000};


const float preComputedCos [73] = {1.00000,
0.99619,0.98481,0.96593,0.93969,0.90631,0.86603,0.81915,0.76604,
0.70711,0.64279,0.57358,0.50000,0.42262,0.34202,0.25882,0.17365,
0.08716,0.00000,-0.08716,-0.17365,-0.25882,-0.34202,-0.42262,-0.50000,
-0.57358,-0.64279,-0.70711,-0.76604,-0.81915,-0.86602,-0.90631,-0.93969,
-0.96593,-0.98481,-0.99619,-1.00000,-0.99619,-0.98481,-0.96593,-0.93969,
-0.90631,-0.86603,-0.81915,-0.76605,-0.70711,-0.64279,-0.57358,-0.50000,
-0.42262,-0.34202,-0.25882,-0.17365,-0.08716,-0.00000,0.08715,0.17365,
0.25882,0.34202,0.42262,0.50000,0.57357,0.64279,0.70711,0.76604,
0.81915,0.86602,0.90631,0.93969,0.96593,0.98481,0.99619,1.00000};

RotationUtil::RotationUtil()
{
  _sineResult = 1; 
}

RotationUtil::RotationUtil(int axisCoords [3], int angle)
{
  setAxisCoordinates(axisCoords); 
  setAngleOfRotation(angle);
}

float RotationUtil::convertDegToRad(int degree)
{
	return (degree/deg_to_rad);
}

void RotationUtil::setAngleOfRotation(int degree)
{
  _angleOfRotation = convertDegToRad(degree); 
  _sineResult = preComputedSine[(int)(degree % 360)/5];  
  _cosineResult = preComputedCos[(int)(degree % 360)/5];
  preComputeValues();   // must recompute the hashed values since _lastProduct depends on _sineResult; 
}

float RotationUtil::fetchAngleOfRotation(){
  return _angleOfRotation;
}

void RotationUtil::setAxisCoordinates(int coord [3])
{
  for (int i = 0; i < 3; i ++){
    _axisCoordinates[i] = coord[i];
  }
  preComputeValues();   // obviously must recompute all the hashed values
}

void RotationUtil::preComputeValues(){
  _sumsSquared = ((_axisCoordinates[0] * _axisCoordinates[0]) + (_axisCoordinates[1] * _axisCoordinates[1]) + (_axisCoordinates[2] * _axisCoordinates[2]));
  _squareOfSums = sqrt(_sumsSquared); 
  _lastProduct = _squareOfSums * _sineResult;
}

void RotationUtil::getRotationResult(float inputCoord[3], float result[3] ){
  
  // OLD ROTATION CODE!
  /*
  float initialProduct = ((_axisCoordinates[0] * inputCoord[0]) + (_axisCoordinates[1] * inputCoord[1]) + 
                          (_axisCoordinates[2] * inputCoord[2])) * (1 - _cosineResult);
  
  for (int i = 0; i < 3; i ++){
    float tempResult = (_axisCoordinates[i] * initialProduct) + (_sumsSquared * inputCoord[i] * _cosineResult); 
    if (i == 0){
      tempResult += (((0 - _axisCoordinates[2]) * inputCoord[1]) + (_axisCoordinates[1] * inputCoord[2]))*_lastProduct; 
    } else if ( i == 1){
      tempResult += (((_axisCoordinates[2] * inputCoord[0])) - (_axisCoordinates[0] * inputCoord[2])) * _lastProduct;
    } else {
      tempResult += (((0 - _axisCoordinates[1]) * inputCoord[0]) + (_axisCoordinates[0] * inputCoord[1])) * _lastProduct;
    }
    result[i] = tempResult/_sumsSquared;
  }  */

  // NEW ROTATION CODE!
  float constProd = (0 - ( (_axisCoordinates[0] * inputCoord[0]) + 
                           (_axisCoordinates[1] * inputCoord[1]) + 
                           (_axisCoordinates[2] * inputCoord[2]))); 
  float endTerm = 1 - _cosineResult; 
  for (int i = 0; i < 3; i++){
    float tempResult = 0; 
    if (i == 0){
      tempResult = (pointOfR[0] * ((_axisCoordinates[1]*_axisCoordinates[1]) + (_axisCoordinates[2] * _axisCoordinates[2]))
                  - _axisCoordinates[0] * ( (pointOfR[1] * _axisCoordinates[1]) + (pointOfR[2] * _axisCoordinates[2]) +
                    constProd))* endTerm + (inputCoord[i] * _cosineResult); 

      tempResult += (((0 - pointOfR[2]) * _axisCoordinates[1]) +
                    (pointOfR[1] * _axisCoordinates[2]) - (_axisCoordinates[2] * inputCoord[1])  
                    + (_axisCoordinates[1] * inputCoord[2])) * _sineResult ; 
    } else if (i == 1){
      tempResult = (pointOfR[1] * ((_axisCoordinates[0]*_axisCoordinates[0]) + (_axisCoordinates[2] * _axisCoordinates[2]))
                  - _axisCoordinates[1] * ( (pointOfR[0] * _axisCoordinates[0]) + (pointOfR[2] * _axisCoordinates[2]) +
                    constProd))* endTerm + (inputCoord[i] * _cosineResult); 

      tempResult += ((pointOfR[2] * _axisCoordinates[0]) - (pointOfR[0] *_axisCoordinates[2]) 
                    + (_axisCoordinates[2]* inputCoord[0]) - (_axisCoordinates[0] *inputCoord[2])) *_sineResult; 
    } else {
      tempResult = (pointOfR[2] * ((_axisCoordinates[0]*_axisCoordinates[0]) +  (_axisCoordinates[1] * _axisCoordinates[1]))
                  - _axisCoordinates[2] * ( (pointOfR[0] * _axisCoordinates[0]) + (pointOfR[1] * _axisCoordinates[1]) + 
                    constProd))* endTerm + (inputCoord[i] * _cosineResult);

      tempResult += ( ((0 - pointOfR[1])) *_axisCoordinates[0] + (pointOfR[0]*_axisCoordinates[1]) +
                      ((0 - _axisCoordinates[1]) * inputCoord[0]) + (_axisCoordinates[0] * inputCoord[1])) * _sineResult; 
    }
    result[i] = tempResult; 
  }
}


