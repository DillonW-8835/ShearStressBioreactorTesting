/************************************************************************
Gibson Moseley - FlowSensor.hpp
*************************************************************************/

#include "sensirion-lf.h"
//#include "sensirion-lf.cpp"

#define SLF3X_SCALE_FACTOR_FLOW 32.0  // Proper flow rate scale factor for SLF3X sensor
#define SLF3X_SCALE_FACTOR_TEMP 200.0 // Proper temperature scale factor for SLF3X sensor
#define SLF3X_I2C_ADDRESS 0x08        // I2C address of SLF3X sensor


float flowRate;
float flowData;

SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

  // The & allows the flowSensor object to be passed by reference as opposed to passed by value
void flowSensorSetup(SensirionLF& flowSensor) {
    uint16_t reset = flowSensor.init();
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    }
    Serial.println("Flow sensor initialized.");
}

float readFlowSensor(SensirionLF& flowSensor, float& flowData) {
   // // Array to hold flow and temperature data
    int ret = flowSensor.readSample();
    if (ret == 0) {
//         //Print flow to terminal
//         Serial.print("Flow: ");
//         Serial.print(flowSensor.getFlow(), 2);
//         Serial.print(" ml/min");

//         // Store flow data
       flowData = flowSensor.getFlow();

//         //Print temp to terminal
//         Serial.print(" | Temp: ");
//         Serial.print(flowSensor.getTemp(), 1);
//         Serial.print(" deg C\n");

//         // Store temperature data
//         //flowData[1] = flowSensor.getTemp();
    } else {
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
       // flowData[0] = flowData[1] = -1.0; // Indicate error with -1.0 values
       if (ret ==2) {
           Serial.println("CRC error");
       }
       flowData = -1.0;
    }
    return flowData;
}

float calculateAverage(float* readings, int numReadings) {
    float sum = 0;
    for (int i = 0; i < numReadings; i++) {
        sum += readings[i];
    }
    return sum / numReadings;
}