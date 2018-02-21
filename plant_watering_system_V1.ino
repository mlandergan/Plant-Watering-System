/*
  Code Produced
  By Mark Edward Landergan
  5/4/2017
  Plant Watering System

  Version 1
  Purpose: read moisture sensor value by controlling VCC pin through a digitalwrite to prevent oxidation and apply water when necessary
*/
#include <TimerOne.h>
const int moistureSensorPin = A0; // signal wire from moisture sensor connected to pin A0
const int VccPin = 8; // connect VCC pin to digital pin 8
const int outletPin = 9; // connect the relay control wire to digital pin 9

int sensorValue = 0; // value read from the moisture Sensor
long AverageSensorValue = 0;
int finalSensorValue = 0;
int needsWater = 172;
int lowThreshold = 840;  // determine the low value of what needs to be water
int highThreshold = 890;
boolean getSensors = false; // variable to prevent oversampling
int hasBeenWatered = 0; // variable to prevent overWatering
int cycle = 0; // variable used to keep track of total time
int sampleRate = 500;
volatile unsigned long sec = 0;  // variable used for seconds, volatile because its being updated in the ISR
volatile unsigned long globalTime = 0;  // variable used to keep track of total elapsed time, volatile because its being updated in the ISR
int timeBetweenWater = 21600; // value the system will wait until reading the sensor again (in seconds) 6 hrs = 6*60*60 = 21600 seconds

void setup() {
  Serial.begin(9600); // pour a bowl of ceral
  pinMode(VccPin, OUTPUT);
  pinMode(outletPin, OUTPUT);

  Timer1.initialize(1000000); // set a timer of length 1000000 microseconds (or 1 sec - or 1Hz)
  Timer1.attachInterrupt( timerIsr ); // attach the service routine here
}

void printToSerial(int counter) {
  Serial.print(counter);
  Serial.print(",");
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.println(AverageSensorValue);
}

void turnOnPump() {
  Serial.println("Water time!");
  digitalWrite(outletPin, HIGH);
  delay(6000); // how long the pump will be run need to test this
  digitalWrite(outletPin, LOW);
}

void resetSystem() {
  sec = 0; // reset the clock
  getSensors = false;
  AverageSensorValue = 0;
  finalSensorValue = 0;
}

void timerIsr() {
  sec++; // increment sec
  globalTime++; // increment clock timer
}

void loop() {
  if (sec >= timeBetweenWater) { // wait a given period of time defined by timeBetweenWater
    Serial.println("Count reached");
    if (!(hasBeenWatered == 2)) { // only water the plant 2 times a day
      if (!getSensors) {
        for (int counter = 0; counter < sampleRate; counter++) {
          digitalWrite(VccPin, HIGH); // turn on the sensor
          sensorValue = analogRead(moistureSensorPin); // read the sig pin and store it to Sensor Value
          AverageSensorValue += sensorValue;
          printToSerial(counter);
          if (counter == (sampleRate - 1)) getSensors = true;
        }
        finalSensorValue = AverageSensorValue / (sampleRate - 1);
        Serial.print("Average Sensor Value");
        Serial.print(":");
        Serial.println(finalSensorValue);
      }

      digitalWrite(VccPin, LOW); // turn off the sensor when not sampling
      //  if (finalSensorValue > 168 || finalSensorValue < 175) { // needs watering
      if (finalSensorValue > lowThreshold && finalSensorValue < highThreshold) { // needs watering
        turnOnPump();
        hasBeenWatered++;
      }
    }
    resetSystem(); // reset variables so previous sensor values don't interfer with next reading
  }

  if (globalTime > 82800) { // it has been over 23 hours
    if (!(hasBeenWatered == 1)) { // it hasn't been watered at all in a 23 hour period
      turnOnPump();
    }
    resetSystem(); // reset variables
    hasBeenWatered = 0; // set hasBeenWatered back to 0 for the next day
    globalTime = 0; // set the global time back to 0 for the next day
  }

}



