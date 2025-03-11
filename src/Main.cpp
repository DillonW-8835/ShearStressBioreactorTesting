/*****************************************************************************************************
Main.cpp
Where we call all functions and run all code
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <SPI.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Update.h>
#include <Wire.h>
#include "sensirion-lf.h"

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"

float currentTime = 0.0;
const int numReadings = 25;
float flowReadings[numReadings];
int currentIndex = 0;
float totalFlow = 0.0;
float pumpSpeed = 0.0;
float loopTime = 0;
float prevTime = 0;
float sumFlow = 0.0;
int timeDelay = 200;
float averageFlow = 0.0;
//long int count = 0;
//int count = 200; // Base the number of counts based on the delay time between readings for the sensor

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);

  //Setup modbus for pump communication
  pumpSetup(); //Function in Pump.hpp
  
  //Set up web server
  // initWebSetup();

  //Begin wire communication
  Wire.begin();
  
  //Set up Flow Sensor and Stepper Motor
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
  //stepperSetup(stepper); //Function in StepperMotor.hpp

  // checkStatus();
  setSpeed(200, true);
  //setPump(true);
  //getSpeed();

  for (int i = 0; i < numReadings; i++) {
    flowReadings[i] = 0.0;
  }

}

void loop() {
  int ret = SLF3X.readSample();
  if (ret == 0) {
  currentTime = millis()/1000.0;;
  //flowRate = readFlowSensor(flowSensor, flowData); //Function in FlowSensor.hpp
  flowRate = SLF3X.getFlow();

 // float flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp

  totalFlow -= flowReadings[currentIndex];
  flowReadings[currentIndex] = flowRate;
  totalFlow += flowRate;
  currentIndex = (currentIndex + 1) % numReadings;
  averageFlow = totalFlow / numReadings;  

  //float sum = 0.0;
  //const int count = 20;

  // count = count + 1;
  // sumFlow = sumFlow + flowRate;
  // averageFlow = sumFlow / count;
  // for (int i = 0; i < count; i++) {
  //   flow = SLF3X.getFlow();
  //   sum = sum + flow;
  //   //averageFlow = sum / count;
  //   delay(50); // small delay that simulates the delay the flow sensor takes to read data
  // }


  //pumpSpeed = getSpeed();

  Serial.print(currentTime);
  Serial.print(", ");
  Serial.print(flowRate);
  Serial.print(", ");
  Serial.print(averageFlow);
 // Serial.print(",");

//  loopTime = currentTime - prevTime;
 // prevTime = currentTime;
 // loopTime = loopTime - currentTime;
 // Serial.print(loopTime);
  Serial.println(" ");

} else {
  Serial.print("Error in SLF3X.readSample(): ");
  Serial.println(ret);
}

  //getSpeed();

  // ws.cleanupClients();
  // float flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  // // ws.textAll(flowData); //Send data to be handled by webscoket
  //String pumpStatus = checkStatus();
  // ws.textAll(pumpStatus); //Send data to be handled by webscoket
  delay(20);
}