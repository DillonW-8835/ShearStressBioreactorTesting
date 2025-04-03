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

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"
#include "Routine.hpp"
#include "MicrosdCard.hpp"

//Start Running
void setup() {
  //Start Serial Communication
  Serial.begin(115200);
  Wire.begin();
  //Setup modbus for pump communication

  pumpSetup(); //Function in Pump.hpp
  flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
    // setupMicroSDcard();

  initWebSetup();

  //setPump(0); //Turn pump off
  //Set up web server

  //Begin wire communication
  
  //Set up Flow Sensor, Stepper Motor, & MicroSD

  // setupMicroSDcard();
  // stepperSetup(stepper); //Function in StepperMotor.hpp

  setRoutine("Test", 0.01, 0.001, 0.3, 5);
}

void loop() {
  //delay(50);
  //ws.cleanupClients(); //Clean up clients to make sure they are not using too much memory
 // String flowData = readFlowSensor(flowSensor, 1); //Function in FlowSensor.hpp
 // ws.textAll(flowData); //Send flow data to website
  //ws.cleanupClients();
 delay(200);
}