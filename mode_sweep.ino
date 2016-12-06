//// ADC BEGIN
#include <ADC.h>

const int pwmPin = A8;
const int readPin = A9; 
const int readPin2 = A3; 
uint8_t pwmDuty = 200;
uint16_t counter = 0;

ADC *adc = new ADC(); 


void setup() {
  pinMode(pwmPin, OUTPUT);
  analogWriteFrequency(pwmPin, 375000);
  analogWrite(pwmPin, pwmDuty);
  analogWriteResolution(12);
  analogReference(0);
  Serial.begin(9600);
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
}

ADC::Sync_result readResult;

void compute_fourier(){
        analogWrite(pwmPin, 200);
}

void loop() {
  readResult = adc->readSynchronizedContinuous();
  counter += 400;
  if (counter > 4050){
    counter = 0;
  }
  analogWrite(A8,counter);
  delay(500);
  Serial.print("Running PWM at");
  Serial.println(counter,DEC);
  delay(50);
  readResult = adc->readSynchronizedContinuous();  
  compute_fourier();
  readResult.result_adc0 = (uint16_t)readResult.result_adc0;
  readResult.result_adc1 = (uint16_t)readResult.result_adc1; 
  float tempRead1 = (readResult.result_adc0*1.0/ adc->getMaxValue(ADC_0)) *4050.0;
  float tempRead2 = (readResult.result_adc1*1.0/adc->getMaxValue(ADC_1)) *4050.0;
  analogWrite(A22, (int)(tempRead1*1.0));
  analogWrite(A21, (int)(tempRead2*1.0)); 
  
}
