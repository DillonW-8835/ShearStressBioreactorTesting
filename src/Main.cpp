/*****************************************************************************************************
* NewMain.cpp
* 
* Where all commands/routines are executed
/*****************************************************************************************************/

//All library includes
#include <Arduino.h>
#include <SPI.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Update.h>
#include <Wire.h>
#include <SoftwareSerial.h>

//all file includes
#include "WebHosting.hpp"
#include "FlowSensor.hpp"
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "StepperMotor.hpp"

#define TX 17
#define RX 16
#define ControlPin 18
#define PUMP_ADDRESS  0xEF

//Set up Flow Sensor
SensirionLF flowSensor(SLF3X_SCALE_FACTOR_FLOW, SLF3X_SCALE_FACTOR_TEMP, SLF3X_I2C_ADDRESS);

SoftwareSerial mySerial(RX, TX);

// int incomingByte = 0;

uint16_t calculateCRC(uint8_t *data, uint8_t length) {
  uint16_t crc = 0xFFFF;
  for (uint8_t i = 0; i < length; i++) {
      crc ^= data[i];
      for (uint8_t j = 0; j < 8; j++) {
          if (crc & 0x0001) {
              crc >>= 1;
              crc ^= 0xA001;
          } else {
              crc >>= 1;
          }
      }
  }
  return crc;
}

void sendCommand(uint8_t* command, size_t length) {
  digitalWrite(ControlPin, HIGH); // Enable transmit mode
  delay(100);
  
  mySerial.write(command, length); // Send command
  mySerial.flush();  // Ensure all data is sent

  digitalWrite(ControlPin, LOW); // Enable receive mode
}

void readResponse() {
  delay(100);  // Wait for response

  if (mySerial.available()) {
      Serial.print("Response: ");
      uint8_t response[8]; // Expected response size
      int index = 0;
      
      while (mySerial.available() && index < 8) {
          response[index] = mySerial.read();
          Serial.print(response[index], HEX);
          Serial.print(" ");
          index++;
      }
      Serial.println();

      // Verify response
      if (index == 8 && response[1] == 0x06) { 
          Serial.println("RS485 Communication Enabled Successfully!");
      } else {
          Serial.println("Failed to Enable RS485 Communication.");
      }
  } else {
      Serial.println("No response from the device.");
  }
}

// void scanModbusAddresses() {
//   for (uint8_t addr = 1; addr <= 247; addr++) {
//       Serial.print("Testing address: ");
//       Serial.println(addr, HEX);
      
//       uint8_t command[] = { 
//           addr,   // Test this address
//           0x03,   // Function: Read Holding Registers
//           0x00, 0x00,  // Register 0x0000
//           0x00, 0x01,  // Read 1 register
//           0x00, 0x00   // Placeholder for CRC
//       };

//       // Compute CRC
//       uint16_t crc = calculateCRC(command, 6);
//       command[6] = crc & 0xFF;
//       command[7] = (crc >> 8) & 0xFF;

//       sendCommand(command, sizeof(command));
//       delay(100);  // Wait for response

//       if (mySerial.available()) {
//           Serial.print("Device found at address: ");
//           Serial.println(addr, HEX);
//           return;
//       }
//   }
//   Serial.println("No devices found.");
// }
//Start Running
void setup() {
  //Start Serial Communication
  // Serial.begin(115200);

  // Setup RS485 communication
  // pinMode(RE, OUTPUT);
  // pinMode(DE, OUTPUT);
  pinMode(ControlPin, OUTPUT);
  pinMode(TX, OUTPUT);
  pinMode(RX, INPUT);

  digitalWrite(ControlPin, LOW);
  // digitalWrite(RE, LOW);
  // digitalWrite(DE, 0);

  // Initialize ModbusMaster with proper pins for TX, RX, and DE/RE
  // Serial2.begin(115200, SERIAL_8N1, RX, TX);
  // mySerial.begin(115200);
  // node.begin(PUMP_ADDRESS, Serial2);

  // node.preTransmission(preTransmission);
  // node.postTransmission(postTransmission);
  
  //set up web server
  // initWebSetup();

  //begin communication
  // Wire.begin();

  // pumpOn = checkStatus();
  // Serial.printf("Pump is: %d\n", pumpOn);
  
  //Set up Flow Sensor and Stepper Motor
  // flowSensorSetup(flowSensor); //Function in FlowSensor.hpp
  // delay(100);
  // stepperSetup(stepper); //Function in StepperMotor.hpp
  // Serial.begin(115200);
  // mySerial.begin(9600);
}

void loop() {

  // scanModbusAddresses();
  // while(1);

//  // RS485 enable command: Slave 0x01, Function 0x06, Register 0x0001, Value 0x0001
 uint8_t command[] = { 
  PUMP_ADDRESS,
  0x05,
  0x10, 0x04,
  0xFF
  }; 
    
 // Compute CRC
 uint16_t crc = calculateCRC(command, 6);
 command[6] = crc & 0xFF;         // Low byte of CRC
 command[7] = (crc >> 8) & 0xFF;  // High byte of CRC

 sendCommand(command, sizeof(command));
 readResponse();

 delay(100);  // Wait for response

  // ws.cleanupClients();

  // if (mySerial.available() > 0) {
  //   mySerial.read();
  // }

  // if (mySerial.isListening()) { 
  //   Serial.println("portOne is listening!");
// }

  // if (mySerial.available()) {
  //   Serial.print ("Message RX: ");
  //   Serial.println (mySerial.readStringUntil ('\n'));
  // }

  // if (mySerial.available() > 0) {
  //   // read the incoming byte:
  //   incomingByte = mySerial.read();

  //   // say what you got:
  //   mySerial.print("I received: ");
  //   mySerial.println(incomingByte, DEC);
  // }

  // String flowData = readFlowSensor(flowSensor); //Function in FlowSensor.hpp
  // ws.textAll(flowData); //Send data to be handled by webscoket
  // delay(250);

  // stepper.moveRelativeInMillimeters(-MOVE_DISTANCE);
  // while (!stepper.motionComplete()) {
  //     //Do Nothing
  // }
  // Serial.print("Moved Stepper Motor\n");
  delay(5000);
}