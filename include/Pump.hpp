/************************************************************************
Gibson Moseley - Pump.hpp
All functions to setup and control the pump
*************************************************************************/

#ifndef PUMP_HPP
#define PUMP_HPP

#include <ModbusMaster.h>

ModbusMaster node;
//bool pumpOn;

// Set up Pump controller
const int MODBUS_RX2 = 16;
const int MODBUS_TX2 = 17;
// const int MODBUS_DE = 18;
// const int MODBUS_RE = 18;
const int MODBUS_ENABLE = 18; // automatically set to high when writing, low otherwise to receive
const int PUMP_ADDRESS = 0xEF; // Modbus address of pump controller

// Pump speeds in ml/min above which the precision of the pump decreases by a factor of 2
#define MIN_FLOW_RATE 8
#define MAX_FLOW_RATE 400

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
  Serial.print(millis());
    Serial.println(": MODBUS_ENABLE set to HIGH (Transmission Mode)");
  //digitalWrite(MODBUS_DE, 1);
}

void postTransmission()
{
  digitalWrite(MODBUS_ENABLE, 0);
  Serial.print(millis());
    Serial.println(": MODBUS_ENABLE set to LOW (Reception Mode)");
//   digitalWrite(MODBUS_DE, 0);
}

void pumpSetup() {
    // Setup RS485 communication
    pinMode(MODBUS_ENABLE, OUTPUT);
    // pinMode(MODBUS_DE, OUTPUT);
    digitalWrite(MODBUS_ENABLE, 0);
    // digitalWrite(MODBUS_DE, 0);

    // Initialize ModbusMaster with proper pins for TX, RX, and DE/RE
    Serial2.begin(9600, SERIAL_8N1, MODBUS_RX2, MODBUS_TX2);
    node.begin(PUMP_ADDRESS, Serial2);
    node.preTransmission(preTransmission);
    node.postTransmission(postTransmission);
}

/**
 * @brief Checks the status of the pump and optionally prints the status to the serial monitor.
 * 
 * This function reads the pump's status using a Modbus node and determines whether the pump
 * is on, off, or in an unknown state. The status is returned as a string and can also be 
 * broadcasted to all connected WebSocket clients. If the `printSerial` parameter is true, 
 * the status is printed to the serial monitor.
 * 
 * @param printSerial A boolean flag indicating whether to print the pump status to the serial monitor.
 *                    - `true`: Print the status to the serial monitor.
 *                    - `false`: Do not print the status.
 * 
 * @return A string representing the pump's status:
 *         - "Pump status: On" if the pump is on.
 *         - "Pump status: Off" if the pump is off.
 *         - "Pump status: Unknown" if the pump's state could not be determined.
 * 
 * @note The function uses a Modbus node to read the pump's state and assumes that the 
 *       response buffer contains the state information. It also sends the status to all 
 *       connected WebSocket clients.
 * 
 * @warning If the Modbus node fails to read the pump's state, an error message is printed 
 *          to the serial monitor, and the status is set to "Unknown".
 */

 bool checkPumpStatus(bool printSerial) {
    int retries = 3;

    while (retries > 0) {
        int result = node.readCoils(0x1001, 1); // Read the pump state
        if (result == 0) {
            pumpOn = node.getResponseBuffer(0);
            if (printSerial) {
                Serial.printf("Pump status: %s\n", pumpOn ? "On" : "Off");
            }
            return pumpOn;
        }
        retries--;
        delay(100); // Wait before retrying
    }

    Serial.println("Error: Unable to read pump state!");
    return false; // Default to off if the operation fails
}

// String checkPumpStatus(bool printSerial) {
//     delay(100);
//     String pumpStatus = "pumpStatus; ";
//     if (node.readCoils(0x1001, 1) == 0) {
//         uint16_t state = node.getResponseBuffer(0);
//         if(state == 1) {
//             pumpStatus += "Pump status: On";
//             if(printSerial == 1) {
//                 Serial.printf("Pump status: On\n");
//             }
//         } else if (state == 0) {
//             pumpStatus += "Pump status: Off";
//             if(printSerial == 1) {
//                 Serial.printf("Pump status: Off\n");
//             }
//         }
//         pumpOn = state;
//     }
//     else {
//         pumpStatus += "Pump status: Unknown";
//         Serial.println("Error: Unable to read pump state!");
//     }
//     ws.textAll(pumpStatus);
//     return pumpStatus;
//     //return pumpOn;
// }

bool setPump(bool option) {
    if (pumpOn == option) {
        return pumpOn; // No need to change state
    }

    int retries = 3;
    int result = -1;

    while (retries > 0) {
        result = node.writeSingleCoil(0x1001, option ? 0xFF : 0x00);
        if (result == 0) {
            pumpOn = option; // Update state only on success
            return pumpOn;
        }
        retries--;
        delay(100); // Wait before retrying
    }

    Serial.printf("Unable to switch pump state! Error code: %d\n", result);
    return pumpOn; // Return the previous state if the operation fails
}

bool setPumpSpeed(uint16_t high, uint16_t low) {
    int retries = 3;
    int result = -1; // Initialize result to an invalid value

    while (retries > 0) {
        node.setTransmitBuffer(0, low);
        node.setTransmitBuffer(1, high);
        result = node.writeMultipleRegisters(0x3001, 2); // Write 2 registers
                                                     // May be 0x02 instead of 2
        if (result == 0) {
            Serial.printf("Pump speed set successfully! High: 0x%04X, Low: 0x%04X\n", high, low);
            return true; // Success
        }

        Serial.printf("Retrying to set pump speed... Error code: %d\n", result);
        retries--;
        delay(100); // Wait before retrying
    }

    Serial.printf("Error (%d) setting flow rate!\n", result);
    return false; // Failure
}

uint16_t calculateFlowRateRegister(int flow) {
    uint16_t low = 0;
    uint16_t high = 0;

    if (flow <= STEP_1) {
        low = STEP_0_CMD + ((flow - STEP_0) * RATE_0);
    } else if (flow <= STEP_2) {
        low = STEP_1_CMD + ((flow - STEP_1) * RATE_1);
    } else if (flow <= STEP_3) {
        low = STEP_2_CMD + ((flow - STEP_2) * RATE_2);
    } else if (flow <= STEP_4) {
        low = STEP_3_CMD + ((flow - STEP_3) * RATE_3);
    } else if (flow <= STEP_5) {
        low = STEP_4_CMD + ((flow - STEP_4) * RATE_4);
    } else {
        low = STEP_5_CMD + (int)((flow - STEP_5) * RATE_5);
        high = (flow % 2) * 0x8000; // Add half of a step for odd numbers
    }

    return (high << 16) | low; // Combine high and low into a single value
}

// Function to set pump speed based on desired flow rate
bool setPumpSpeedFromFlowRate(int flowRate) {
    // Constrain the flow rate to the valid range
    flowRate = constrain(flowRate, MIN_FLOW_RATE, MAX_FLOW_RATE);

    // Calculate the register values
    uint16_t flowRateRegister = calculateFlowRateRegister(flowRate);
    uint16_t high = flowRateRegister >> 16; // Extract high 16 bits
    uint16_t low = flowRateRegister & 0xFFFF; // Extract low 16 bits

    // Write the register values to the pump
    return setPumpSpeed(high, low);
}

bool setPumpSpeed(int flow) {
    flow = constrain(flow, 8, 400); // Constrain flow to valid range
    uint16_t flowRateRegister = calculateFlowRateRegister(flow);
    return setPumpSpeed(flowRateRegister >> 16, flowRateRegister & 0xFFFF);
}

struct PumpSpeed {
    int32_t setSpeed;
    int32_t realTimeSpeed;
};

PumpSpeed getPumpSpeed() {
    PumpSpeed speed = {-1, -1}; // Default values for failure
    int retries = 3;
    int result = -1; // Initialize result to an invalid value

    while (retries > 0) {
        result = node.readHoldingRegisters(0x3001, 6); // Read 6 registers
        if (result == 0) {
            speed.setSpeed = (node.getResponseBuffer(1) << 16) | node.getResponseBuffer(0);
            speed.realTimeSpeed = (node.getResponseBuffer(5) << 16) | node.getResponseBuffer(4);
            Serial.printf("Set speed: %d\n", speed.setSpeed);
            Serial.printf("Real-time speed: %d\n", speed.realTimeSpeed);
            return speed;
        }
        retries--;
        delay(100); // Wait before retrying
    }

    Serial.println("Error: Unable to read pump speed!");
    return speed;
}

/*
 * Returns whether the pump is on.
 */
bool isPumpOn() {
    return pumpOn;
}

#endif