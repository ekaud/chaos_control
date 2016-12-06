#include <RotateCalc.h>
#include <RotationUtil.h>
#include "arduinoFFT.h"
#include <TimerOne.h>
#include <TimerThree.h>
#include <UserRotate.h>

// FFT constants
#define SCL_INDEX 0x00
#define SCL_TIME 0x01
#define SCL_FREQUENCY 0x02
const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2
double samplingFrequency = 20000;

double vReal[samples]; 
double vImag[samples];
arduinoFFT FFT = arduinoFFT(); /* Create FFT object */
uint16_t sampleCounter = 0; 
uint16_t sample20 = 0;
uint16_t pwmDuty = 10;
long time_old = 0;
long interval_for_calc = 1000; 
double bin_power_chaos_min = 1.3;
double bin_power_chaos_max = 1.7;
float sumx = 0;
float sumy = 0;

//// ADC BEGIN
#include <ADC.h>

const int readPin = A9; 
const int readPin2 = A3; 
const int pwmPin = A8; 
ADC *adc = new ADC(); 
elapsedMicros time; 
//// ADC END
int allCoordinates[3][3] = {{1, 0, 0},{0,1,0},{0, 0, 1}};
int angles [3]  = {70,0,0};
RotateCalc calculator = RotateCalc(allCoordinates, angles);
char name [] = "Rotator1";
char nameTwo [] = "Rotator2"; 
UserRotate rotateInput = UserRotate(25,24,name); 
UserRotate rotateInput2 = UserRotate(12,11, nameTwo); 

float phase = 0.0;
float twopi = 3.14159 * 2;
elapsedMicros usec = 0;

void handlerWrap(){
  rotateInput.timerIsr();
  rotateInput2.timerIsr();
}

void compute_fourier(){
  unsigned long currMillis = millis(); 
  if ( (currMillis - time_old) > interval_for_calc){
    time_old = currMillis;
     if (sample20 == samples){
        double binPower = fourier(vReal, vImag);
        sample20 = 0; 
        if (binPower < bin_power_chaos_min){
           pwmDuty += 100;
           if (pwmDuty > 4050){
            pwmDuty = 100;
            }
        }
       else if (binPower > bin_power_chaos_max){
          pwmDuty = 100;
       }
       analogWrite(pwmPin, pwmDuty); 
//       analogWrite(pwmPin, 1400); 
       Serial.print("pwm_pin is ");
       Serial.println(pwmPin);
    }
     
  }
}

void setup() {
  sampleCounter = 0; 
  sample20 = 0;
  Serial.begin(9600);
  pinMode(pwmPin, OUTPUT);
  analogWriteFrequency(pwmPin, 375000);
  analogWriteResolution(12);
  analogReference(0);
  analogWrite(pwmPin, pwmDuty);
  Timer1.initialize(1000);
  Timer1.attachInterrupt(handlerWrap);
  calculator.updateRotatorAngle(0, 0);
  /*Timer3.initialize(5000000); 
  Timer3.attachInterrupt(compute_fourier); */
  
  //// ADC STUFF 
  pinMode (LED_BUILTIN,OUTPUT); 
  pinMode(readPin,INPUT);
  pinMode(readPin2, INPUT); 

  adc->setAveraging(1);
  adc->setResolution(16); 
  adc->setConversionSpeed(ADC_HIGH_SPEED);
  adc->setSamplingSpeed(ADC_HIGH_SPEED);

  // ADC 1
  adc->setAveraging(1);
  adc->setResolution(16,ADC_1);
  adc->setConversionSpeed(ADC_HIGH_SPEED,ADC_1);
  adc->setSamplingSpeed(ADC_HIGH_SPEED,ADC_1);

 // do comparison 
 //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
 adc->startSynchronizedContinuous(readPin, readPin2); 
 delay(100);
 Serial.println("END SETUP"); 

 //// ADC END
 
 
}

ADC::Sync_result readResult; 



void loop() { 
  
  readResult = adc->readSynchronizedContinuous();  
  compute_fourier();
  readResult.result_adc0 = (uint16_t)readResult.result_adc0;
  readResult.result_adc1 = (uint16_t)readResult.result_adc1; 
 
    
  float tempRead1 = (readResult.result_adc0*1.0/ adc->getMaxValue(ADC_0)) *4050.0;
  float tempRead2 = (readResult.result_adc1*1.0/adc->getMaxValue(ADC_1)) *4050.0;

//  for (int i = 0; i < 8; i ++){
//    readResult = adc->readSynchronizedContinuous();
//    readResult.result_adc0 = (uint16_t)readResult.result_adc0;
//    readResult.result_adc1 = (uint16_t)readResult.result_adc1; 
//    tempRead1 += (readResult.result_adc0*1.0/ adc->getMaxValue(ADC_0)) *4050.0;
//    tempRead2  += (readResult.result_adc1*1.0/adc->getMaxValue(ADC_1)) *4050.0;
//    
//  }
//  tempRead1 = tempRead1/9;
//  tempRead2 = tempRead2/9;
  

   
  
  sampleCounter += 1; 

  if (sampleCounter % 20 == 0){
    if (sample20 < samples){
      vReal[sample20] = double (tempRead1); 
    //Serial.print(vReal[sampleCounter]); 
    vImag[sample20] = 0; 
    sample20 += 1;
    }
   }


  
  float valuesRead[3] = {tempRead1, tempRead2 , 0}; 
  float result [3]; 
  calculator.updateRotatorAngle(0, 0);
  calculator.fetchRotationResult(valuesRead,result); 
//  int currentCount = rotateInput.updateValue();
//  if (currentCount != -1){
//    calculator.updateRotatorAngle(0, currentCount); 
//  } 
//  currentCount = rotateInput2.updateValue();
//  if(currentCount != -1){
//    calculator.updateRotatorAngle(1, currentCount);
//  }

//  analogWrite(A22, (int)(result[0]));
//  analogWrite(A21, (int)(result[1]));
  analogWrite(A22, (int)(result[0]*1.8 - 2200));
  analogWrite(A21, (int)(result[1]*3.0 - 5000)); 
  
//  analogWrite(A22, 2000);
//  analogWrite(A21, 1000); 
}

double fourier(double vRealIn [samples], double vImagIn [samples]) {

//  Serial.println("Data:");
//  PrintVector(vRealIn, samples, SCL_TIME);
  FFT.Windowing(vRealIn, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  //Serial.println("Weighed data:");
  //PrintVector(vRealIn, samples, SCL_TIME);
  FFT.Compute(vRealIn, vImagIn, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vRealIn, vImagIn, samples); /* Compute magnitudes */
//  Serial.println("Computed magnitudes:");
  vRealIn[0] = 0;
  double maxValue = 0; 
  for (int i = 0; i < samples ; i++){
    if (vRealIn[i] > maxValue){
      maxValue  = vRealIn[i];
    }
  }
  // normalize the values
  double binSum = 0; 
  for (int i = 0; i < samples; i++){
    vRealIn[i] = vRealIn[i]/maxValue; 
    if (i < 8){
      binSum += vRealIn[i];
    }
  }
//  PrintVector(vRealIn, (samples>>1), SCL_FREQUENCY);
  Serial.print("The bin value is: "); 
  Serial.println(binSum); 
  Serial.print("The PWM value is: "); 
  Serial.println(pwmDuty); 
  //PrintVector(vRealIn, (samples >> 1), SCL_FREQUENCY);  
  double f_peak = FFT.MajorPeak(vRealIn, samples, samplingFrequency);
//  Serial.println("Peak Frequency = ");
//  Serial.println(f_peak, 6);
  //while(1); /* Run Once */
 // delay(1000); /* Repeat after delay */
  return binSum;
}

void PrintVector(double *vData, uint16_t bufferSize, uint8_t scaleType) 
{
  for (uint16_t i = 0; i < bufferSize; i++) 
  {
    double abscissa;
    /* Print abscissa value */
    switch (scaleType) 
    {
      case SCL_INDEX:
        abscissa = (i * 1.0);
  break;
      case SCL_TIME:
        abscissa = ((i * 1.0) / samplingFrequency);
  break;
      case SCL_FREQUENCY:
        abscissa = ((i * 1.0 * samplingFrequency) / samples);
  break;
    }
    Serial.print(abscissa, 6);
    Serial.print(",");
    Serial.print(vData[i], 4);
    Serial.print(",");
//    Serial.println();
  }
  Serial.println();
}
