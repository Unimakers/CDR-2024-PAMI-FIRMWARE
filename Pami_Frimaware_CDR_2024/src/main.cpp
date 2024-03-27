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
//Servo servoLeft, servoRight;

// Wrapper des 2 servos
//MergeServos RobotServos(servoLeft, servoRight, SERVO1, SERVO2);

bool testAlternanceLoop = true; // test mouvements

void setup() {
    Serial.begin(115200);
    delay(20);
    Serial.println("Liaison série OK");
    RobotSteppers.set_max_acceleration(2500.0);
    RobotSteppers.set_speed(15000.0);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(20);
    Serial.println("Steppers OK");
}

void loop() {
    if (RobotSteppers.target_reached()) {
        if (testAlternanceLoop) {
            RobotSteppers.move_line(70000.0);
            //RobotSteppers.turn(90);

            testAlternanceLoop = false;
        } else {
            //RobotSteppers.turn(-90);
            RobotSteppers.move_line(-70000.0);

            testAlternanceLoop = true;
        }
    }

    RobotSteppers.run();
}
