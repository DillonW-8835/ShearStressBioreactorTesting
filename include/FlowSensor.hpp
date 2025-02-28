//

#include "sensirion-lf.h"
#include "sensirion-lf.cpp"


void flowSensorSetup(SensirionLF flowSensor) {
    uint16_t reset = flowSensor.init();
    if (reset != 0) {
        Serial.print("Error initializing the flow sensor: ");
        Serial.println(reset);
        return;
    }
    Serial.println("Flow sensor initialized.");
}

String readFlowSensor(SensirionLF flowSensor) {
    int ret = flowSensor.readSample();
    String flowData = "";
    if (ret == 0) {
        //Print flow to terminal
        Serial.print("Flow: ");
        Serial.print(flowSensor.getFlow(), 2);
        Serial.print(" ml/min");

        //Put flow data to webserver
        flowData += "Flow: ";
        flowData += String(flowSensor.getFlow(), 2) + " ml/min";

        //Print temp to terminal
        Serial.print(" | Temp: ");
        Serial.print(flowSensor.getTemp(), 1);
        Serial.print(" deg C\n");

        //Put temp data to webserver
        flowData += " | Temp: ";
        flowData += String(flowSensor.getTemp(), 1) + " deg C";
    } else {
        Serial.print("Error in flowsensor.readSample(): ");
        Serial.println(ret);
        flowData = "Error in flowsensor.readSample(): " + String(ret);
    }
    return flowData;
}