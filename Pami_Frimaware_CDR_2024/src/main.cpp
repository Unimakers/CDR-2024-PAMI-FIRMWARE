#include <Arduino.h>
#include <PamiDef.h>
#include <AccelStepper.h>
#include <MergeSteppers.h>
#include <ESP32Servo.h>
#include <MergeServos.h>

// Steppers gauche et droite
AccelStepper stepperLeft(AccelStepper::DRIVER, STEP1, DIR1), stepperRight(AccelStepper::DRIVER, STEP2, DIR2);

// Wrapper des 2 steppers
MergeSteppers RobotSteppers(stepperLeft, stepperRight, EN_DRIVER1, EN_DRIVER2);

// Servos gauche et droite
Servo servoLeft, servoRight;

// Wrapper des 2 servos
MergeServos RobotServos(servoLeft, servoRight, SERVO1, SERVO2);

bool testAlternanceLoop = false;

void setup() {
    Serial.begin(115200);
    delay(20);
    Serial.println("Liaison série OK");
    RobotSteppers.set_max_acceleration(2000.0);
    RobotSteppers.set_speed(4000.0);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(20);
    Serial.println("Steppers et Servos OK");
}

void loop() {
    if (RobotSteppers.target_reached()) {
        RobotSteppers.turn(90);
    }

    RobotSteppers.run();
}
