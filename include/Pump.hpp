/************************************************************************
Gibson Moseley - Pump.hpp
All functions to setup and control the pump
*************************************************************************/

#ifndef PUMP_HPP
#define PUMP_HPP
#include <ModbusMaster.h>

ModbusMaster node;

// Set up Pump controller
const int MODBUS_RX2 = 16;
const int MODBUS_TX2 = 17;
const int MODBUS_ENABLE = 18; // automatically set to high when writing, low otherwise to receive
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller

// Pump speeds in ml/min above which the precision of the pump decreases by a factor of 2
const int STEP_0 = 8;
const int STEP_1 = 16;
const int STEP_2 = 32;
const int STEP_3 = 64;
const int STEP_4 = 128;
const int STEP_5 = 256;

// Register values corresponding to the above speeds
const uint16_t STEP_0_CMD = 0x4100;
const uint16_t STEP_1_CMD = 0x4180;
const uint16_t STEP_2_CMD = 0x4200;
const uint16_t STEP_3_CMD = 0x4280;
const uint16_t STEP_4_CMD = 0x4300;
const uint16_t STEP_5_CMD = 0x4380;
const uint16_t STEP_6_CMD = 0x43C8;

// Number of register value increments to change pump speed by 1 ml/min
const int RATE_0 = 16;      // 1 / 0.0625 ml/min
const int RATE_1 = 8;       // 1 / 0.125
const int RATE_2 = 4;       // 1 / 0.25
const int RATE_3 = 2;       // 1 / 0.5
const int RATE_4 = 1;       // 1 / 1
const double RATE_5 = 0.5;  // 1 / 2

bool pumpOn;

void preTransmission()
{
  digitalWrite(MODBUS_ENABLE, 1);
}

void postTransmission()
{
  digitalWrite(MODBUS_ENABLE, 0);
}

void pumpSetup() {
    // Setup RS485 communication
    pinMode(MODBUS_ENABLE, OUTPUT);
    digitalWrite(MODBUS_ENABLE, 0);

    // Initialize ModbusMaster with proper pins for TX, RX, and DE/RE
    Serial2.begin(9600, SERIAL_8N1, MODBUS_RX2, MODBUS_TX2);
    node.begin(PUMP_ADDRESS, Serial2);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}

String checkStatus() {
    String pumpStatus = "Pump Status: ";
    if (node.readCoils(0x1001, 1) == 0) {
        uint16_t state = node.getResponseBuffer(0);
        if(state == 1) {
            pumpStatus += "Pump status: On";
            Serial.printf("Pump status: On\n");
        } else if (state == 0) {
            pumpStatus += "Pump status: Off";
            Serial.printf("Pump status: Off\n");
        }
        pumpOn = state;
    }
    else {
        pumpStatus += "Pump status: Unknown";
        Serial.println("Error: Unable to read pump state!");
    }
    return pumpStatus;
    //return pumpOn;
}

bool setPump(bool option) {
    if (pumpOn != option) {
        pumpOn = option;

        uint16_t result = node.writeSingleCoil(0x1001, pumpOn ? 0xFF : 0x00);
        if (result != 0) {
            Serial.printf("Unable to switch pump state! Error code: %d\n", result);
            pumpOn = !option;
        }
    }
    return pumpOn;
}

bool setSpeed(uint16_t high, uint16_t low, bool start/* = false*/) {
    node.setTransmitBuffer(0, low);
    node.setTransmitBuffer(1, high);
    int result = node.writeMultipleRegisters(0x3001, 0x02);

    if (start) {
        setPump(true);
    }

    if (result != 0) {
        Serial.printf("Error (%d) setting flow rate!\n", result);
        return false;
    }
    else {
        return true;
    }
}

bool setSpeed(int flow, bool force) {
    // The pump will ignore speed commands when running
    if (pumpOn) {
        if (force) {
            setPump(false);
        }
        else {
            Serial.println("Error: Attempt to set pump speed while running.");
            return false;
        }
    }

    // Constrain values to within the pump range (although the pump controller does this as well for 0.1-420ish ml/min)
    if (flow < 8) {
        flow = 8;
    }
    else if (flow > 400) {
        flow = 400;
    }

    uint16_t low = 0;
    uint16_t high = 0;

    // TODO: currently only integer flow rates are possible, but the high bytes can be used to 
    // achieve decimal values, following the calculations done for flow rates over 256 ml/min

    // Start with the known register value at the edge of the precision level, then add the needed steps
    if (flow <= STEP_1) {
        low = STEP_0_CMD + ((flow - STEP_0) * RATE_0);
    }
    else if (flow > STEP_1 && flow <= STEP_2) {
        low = STEP_1_CMD + ((flow - STEP_1) * RATE_1);
    }
    else if (flow > STEP_2 && flow <= STEP_3) {
        low = STEP_2_CMD + ((flow - STEP_2) * RATE_2);
    }
    else if (flow > STEP_3 && flow <= STEP_4) {
        low = STEP_3_CMD + ((flow - STEP_3) * RATE_3);
    }
    else if (flow > STEP_4 && flow <= STEP_5) {
        low = STEP_4_CMD + ((flow - STEP_4) * RATE_4);
    }
    else if (flow > STEP_5) {
        low = STEP_5_CMD + (int) ((flow - STEP_5) * RATE_5);
        high = (flow % 2) * (0x8000); // add half of a step to achieve odd numbers
    }

    return setSpeed(high, low, force);
}

// void getSpeed() {
    // Read the set speed
    // uint16_t result = node.readHoldingRegisters(0x3001, 2); // Read 2 registers starting at 0x3001
    // if (result == 0) {
    //     uint16_t setSpeedLow = node.getResponseBuffer(0);
    //     uint16_t setSpeedHigh = node.getResponseBuffer(1);
    //     Serial.printf("Set speed: %X %X\n", setSpeedHigh, setSpeedLow);
    // } else {
    //     Serial.printf("Error (%d) reading set speed!\n", result);
    // }

    // // Read the real-time speed
    // result = node.readHoldingRegisters(0x3005, 2); // Read 2 registers starting at 0x3005
    // if (result == 0) {
    //     uint16_t realTimeSpeedLow = node.getResponseBuffer(0);
    //     uint16_t realTimeSpeedHigh = node.getResponseBuffer(1);
    //     Serial.printf("Real-time speed: %X %X\n", realTimeSpeedHigh, realTimeSpeedLow);
    // } else {
    //     Serial.printf("Error (%d) reading real-time speed!\n", result);
    // }
// }

int32_t getSpeed() {
    String pumpSpeed = "Pump Speed: ";
    uint16_t result = node.readWriteMultipleRegisters(0x3001, 6); // read all holding registers
    int32_t lowBytes = -1;
    // Check if the command returned no error
    if (result == 0) {
        // Print the speeds stored in the holding registers
        lowBytes = node.getResponseBuffer(0);
        Serial.printf("Set speed: %X %X\n", node.getResponseBuffer(1), lowBytes);
        Serial.printf("Real-time speed: %X %X\n", node.getResponseBuffer(5), node.getResponseBuffer(4));
    }
    
    return lowBytes;
}

/*
 * Returns whether the pump is on.
 */
bool isPumpOn() {
    return pumpOn;
}

#endif