#include <ESP_FlexyStepper.h>

//Pins for Stepper Motor
int HIGH_MOTOR_DIRPIN = 27;
int HIGH_MOTOR_STEPPIN = 26;
int HIGH_MOTOR_ENAPIN = 25;

//Set up Stepper Motor varibiles
ESP_FlexyStepper stepper;

const float STEPS_PER_REV = 200;
const float DISTANCE_PER_REV = 1;
const float MAX_SPEED = 1000;
const float ACCELERATION = 1000;
const float MOVE_DISTANCE = 5; 

void stepperSetup(ESP_FlexyStepper stepper) {
    //Setup Stepper Motor Paramaters
    stepper.connectToPins(HIGH_MOTOR_STEPPIN, HIGH_MOTOR_DIRPIN);

    stepper.setStepsPerMillimeter(STEPS_PER_REV / DISTANCE_PER_REV);
    stepper.setSpeedInStepsPerSecond(MAX_SPEED);
    stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION);

    //stepper.setCurrentPositionInMillimeters(0.0);
    stepper.startAsService(1);
}

void moveStepper(int moveDistnace) {
    stepper.moveRelativeInMillimeters(moveDistnace);
}