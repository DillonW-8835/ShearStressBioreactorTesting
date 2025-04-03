/************************************************************************
Gibson Moseley - Routine.hpp

Allows for the running of routines for the system
*************************************************************************/

#ifndef ROUTINE_HPP
#define ROUTINE_HPP

//library for timing
#include <chrono>

//all necessary files for includes
#include "Pump.hpp"
#include "BioreactorVaribiles.hpp"
#include "FlowSensor.hpp"
#include "MicrosdCard.hpp"

//naming conventions
using namespace std;
using namespace std::chrono;

//this converts the amount of time running in seconds to a String with Hr:Min:Sec
String convertTimeToString(int timeSeconds) {
    int timeLeft = timeSeconds;
    int totalHr = 0;
    int totalMin = 0;
    int totalSec = 0;

    Serial.print(timeLeft);
    Serial.print("\n");
    String clockConversion = "";

    if(timeLeft/3600 >=1) {
        totalHr = timeLeft / 3600;
        timeLeft = timeLeft - (totalHr * 3600);
    }
    if (timeLeft/60 >= 1) {
        totalMin = timeLeft / 60;
        timeLeft = timeLeft - (totalMin * 60);
    }
  
    totalSec = timeLeft;

    // int totalHr = timeSeconds / 3600;
    // int totalMin = (timeSeconds % 3600) / 60;
    // int totalSec = timeSeconds % 60;

   // String clockConversion = "";

    clockConversion = "Routine Time: " + String(totalHr) + ":" + String(totalMin) + ":" + String(totalSec);

    String clockWebsite = "runningTime; " + clockConversion;
    Serial.print(clockConversion);
    ws.textAll(clockWebsite);
    //Serial.print(clockConversion);
    Serial.print("\n");

    return clockConversion;
    
}

void setRoutine(String routineName, double timeRun, double timeBreak, double shearStress, int repetition) {
    int desiredFlowRate = flowRateBasedOnShearStressCalc(shearStress);
    static int currentFlowRate = -1; // Store the current flow rate

    auto timeRunHr = std::chrono::duration<double, std::chrono::seconds::period>(timeRun * 3600);
    auto timeBreakHr = std::chrono::duration<double, std::chrono::seconds::period>(timeBreak * 3600);
   
    for (int i = 1; i <= repetition; i++) {
        Serial.printf("Starting Routine Iteration: %d\n", i);

        // if (pumpOn) {
        //     if (!setPump(0)) {
        //         Serial.println("Error: Failed to stop the pump!");
        //         return;
        //     }
        // }

        if (!pumpOn) {
            int retries = 3;
            while (retries > 0 && !setPump(1)) {
                setPump(1);
                Serial.println("Error: Failed to start the pump! Retrying...");
                retries--;
                delay(100); // Wait before retrying
            }
            if (!pumpOn) {
                Serial.println("Error: Unable to start the pump after retries!");
                return; // Exit if the pump cannot be started
            }
        }

        auto start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeRunHr) {
           
            if (currentFlowRate != desiredFlowRate) {
                if (!setPumpSpeedFromFlowRate(desiredFlowRate)) {
                    Serial.println("Error: Failed to set pump speed!");
                    return;
                }
                currentFlowRate = desiredFlowRate;
            }

            if (!pumpOn) {
                int retries = 3;
                while (retries > 0 && !setPump(1)) {
                    Serial.println("Error: Failed to start the pump! Retrying...");
                    retries--;
                    delay(100); // Wait before retrying
                }
                if (!pumpOn) {
                    Serial.println("Error: Unable to start the pump after retries!");
                    return; // Exit if the pump cannot be started
                }
            }
           
            String time = convertTimeToString(duration_cast<seconds>(high_resolution_clock::now() - start).count());
            String flow = readFlowSensor(flowSensor, 0);
            Serial.printf("Flow rate: %s\n", flow.c_str());

            ws.cleanupClients(); // Clean up clients to avoid memory issues

            static unsigned long lastPumpCheck = 0;
            unsigned long currentMillis = millis();
            if (currentMillis - lastPumpCheck >= 1000) {
                lastPumpCheck = currentMillis;
                if (!checkPumpStatus(true)) {
                    Serial.println("Error: Unable to determine pump status!");
                    // if (!setPump(1)) {
                    //     Serial.println("Error: Failed to restart the pump!");
                    //     return;
                    // }
                }
            }

            delay(200);
        }

        if (!setPump(0)) {
            Serial.println("Error: Failed to stop the pump!");
            return;
        }

        start = high_resolution_clock::now();
        while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeBreakHr) {
            String time = convertTimeToString(duration_cast<seconds>(high_resolution_clock::now() - start).count());
            
            ws.cleanupClients(); // Clean up clients to avoid memory issues 
            
            delay(200);
        }
    }

    Serial.println("Routine Done");
}

//alows seeting for a basic routne that repeats x times in hr conversions, scaled in seconds and rounds.
//Example 3.6 seconds will be 4 seconds
// void setRoutine(String routineName, double timeRun, double timeBreak, double shearStress, int repetion) {
//     //calculate flowrate needed to achive required shearStress
//     //int flowRate = flowRateBasedOnShearStressCalc(shearStress);

//     //scale the time to run and break from hr to seconds
//     auto timeRunHr =  std::chrono::duration<double, std::chrono::seconds::period>(timeRun * 3600);
//     auto timeBreakHr = std::chrono::duration<double, std::chrono::seconds::period>(timeBreak * 3600);

//     //runn the routine as may times as speicifed
//     for(int i = 1; i < repetion + 1; i++) {
//        // checkPumpStatus(true);
//         Serial.printf("Starting Routine Iteration: %d\n", i);
//         //make sure flow rate is set and pump is off

//        // setPump(0);
//        if (pumpOn) {
//         if (!setPump(0)) {
//             Serial.println("Error: Failed to stop the pump!");
//             return; // Exit or handle the error
//         }
//     }
//         //setPumpSpeed(flowRate, 1);
//         static int currentFlowRate = -1; // Store the current flow rate
//         int desiredFlowRate = flowRateBasedOnShearStressCalc(shearStress);

//         if (currentFlowRate != desiredFlowRate) {
//             if (!setPumpSpeed(desiredFlowRate)) {
//                 Serial.println("Error: Failed to set pump speed!");
//                 return; // Exit or handle the error
//             }
//             currentFlowRate = desiredFlowRate; // Update the current flow rate
//         }
//         //setPump(1);

//         if (pumpOn) {}
//             if (!setPump(1)) {
//                 Serial.println("Error: Failed to start the pump!");
//                 return; // Exit or handle the error
//             }
//         }
//         // if (pumpOn == 1 & i == 0) {
//         //     setPump(0);
//         //     setPumpSpeed(flowRate, 1);
//         // }
//         // else if (pumpOn == 0 & i == 0) {
//         //     setPumpSpeed(flowRate, 1);
//         // }

//         //print what repition we are on and start pump
//         // Serial.printf("Starting Routine Iteration: %d, ", i);
//         // setPump(1);

//         //run the pump for specified amount of time and print how long at the end
//         //auto startTotal = high_resolution_clock::now();
//         auto start = high_resolution_clock::now();
//         while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeRunHr) {
//             auto currentTotal = duration_cast<seconds>(high_resolution_clock::now() - start);
//             int timeInt = currentTotal.count();
//             String time = convertTimeToString(timeInt);
//             String flow = readFlowSensor(flowSensor, 0);
//            // checkPumpStatus(true);

//            static unsigned long lastPumpCheck = 0;
//            unsigned long currentMillis = millis();
           
//            if (currentMillis - lastPumpCheck >= 1000) { // Check pump status once per second
//                lastPumpCheck = currentMillis;
//                if (!checkPumpStatus(true)) {
//                    Serial.println("Error: Unable to determine pump status!");
//                }
//            }
//         //auto finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
//         //int time = finalTime.count();
//         //Serial.printf("Time ran was %d seconds, ", time);
//             delay(100);
//         }
//         //for break turn pump off
//         //setPump(0);

//         if (!setPump(0)) {
//             Serial.println("Error: Failed to stop the pump!");
//             return; // Exit or handle the error
//         }

//         //dont run the pump for speicifed amount of time and print it
//         start = high_resolution_clock::now();
//         while (duration_cast<seconds>(high_resolution_clock::now() - start) < timeBreakHr) {
//             auto currentTotal = duration_cast<seconds>(high_resolution_clock::now() - start);
//             //int timeInt = currentTotal.count();
//             String time = convertTimeToString(currentTotal.count());
//             String flow = readFlowSensor(flowSensor, 0);

//             delay(200);
//             // if (currentTotal.count() % 1 == 0) {
//             //     checkPumpStatus(true);
//             // }
//             //checkPumpStatus(true);

//             //writeBioreactorInfo(routineName, time, flow);
//         }
//         //finalTime = duration_cast<seconds>(high_resolution_clock::now() - start);
//         //time = finalTime.count();
//         //Serial.printf("Time paused was %d seconds.\n", time);
//     }

//     //denote routine is over
//     Serial.print("Routine Done\n"); 
// }

#endif