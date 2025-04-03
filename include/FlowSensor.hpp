/************************************************************************
Gibson Moseley - FlowSensor.hpp

Holds all commands necessry to use the flow sensor, and declerations of the flow sensor
*************************************************************************/

#ifndef FLOWSENSOR_HPP
#define FLOWSENSOR_HPP

//inclusion of necessary libraries
#include "sensirion-lf.h"
#include "sensirion-lf.cpp"

//set up low flow sensor with appropriate varibiles
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

//sets up the flowsensor to be used
void flowSensorSetup(SensirionLF flowSensor) {
    //run the intilizing command and save result
    uint16_t reset = flowSensor.init();

    //if result is zero then error happened otherwise it was reset/setup
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    } else {
        Serial.println("Flow sensor initialized.");
    }
}

//reads the setup flowsensor provided and send that data to the website
String readFlowSensor(SensirionLF flowSensor, bool printTerminal) {
    int ret = flowSensor.readSample();
    String flowData = "";

    if (ret == 0) {
        if (printTerminal == 1) {
            //Print flow and temp  to terminal
            Serial.print("Flow: ");
            Serial.print(flowSensor.getFlow(), 2);
            Serial.print(" ml/min");

            //Print temp to terminal
            Serial.print(" | Temp: ");
            Serial.print(flowSensor.getTemp(), 1);
            Serial.print(" deg C\n");
        }

        //Put flow data into string varibile
        flowData += "Flow: ";
        flowData += String(flowSensor.getFlow(), 2) + " ml/min";

        //Put temp data into string varibile
        flowData += " | Temp: ";
        flowData += String(flowSensor.getTemp(), 1) + " deg C";
    } else {
        //if unable to read set string varibile as error message
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        Serial.print("\n");
        flowData = "Error in flowsensor.readSample(): " + String(ret);
    }

    //send string varibile to webserver/website through webscoket 
    //ws.textAll(flowData);
    return flowData;
}

#endif