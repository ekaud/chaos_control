#include "Arduino.h"
#include "RotateCalc.h"

RotateCalc::RotateCalc(int axisCoords[3][3], int angle [3]){
	int firstArray [3] = {axisCoords[0][0], axisCoords[1][0], axisCoords[2][0]};
	int secondArray [3] = {axisCoords[0][1], axisCoords[1][1], axisCoords[2][1]};

	_rotatorOne.setAxisCoordinates(firstArray); 
	_rotatorOne.setAngleOfRotation(angle[0]);
	_rotatorTwo.setAxisCoordinates(secondArray);
	_rotatorTwo.setAngleOfRotation(angle[1]);
}

void RotateCalc::fetchRotationResult(float inputCoord [3], float result[3]){
	float tempResult1[3];
	_rotatorOne.getRotationResult(inputCoord, tempResult1);
	_rotatorTwo.getRotationResult(tempResult1, result);
	/*float theta = 0;
  	float phi = 3.1458/5;
  	float znew = result[0]*sin(theta) + result[2]*cos(theta);
  	float xnew = result[0]*cos(theta) - result[2]*sin(theta);
  	float ynew = result[1]*sin(phi) - znew*cos(phi);
  	result[0] = xnew;
  	result[1] = ynew; 
  	result[0] = result[0]/result[2];
  	result[1] = result[1]/result[2]; */
}

void RotateCalc::updateRotatorAngle(int number, int angle){
	if (number == 0){
		_rotatorOne.setAngleOfRotation(angle); 
	} else if (number == 1){
		_rotatorTwo.setAngleOfRotation(angle);
	}

}

void RotateCalc::fetchRotatorAngles(float angles [2]){
	angles[0] = _rotatorOne.fetchAngleOfRotation() * 57.2958; 
	angles[1] = _rotatorTwo.fetchAngleOfRotation() * 57.2958; 
}