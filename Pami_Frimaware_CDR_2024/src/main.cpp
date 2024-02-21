#include <Arduino.h>
#include <PamiDef.h>
#include <AccelStepper.h>
#include <MergeSteppers.h>
#include <ESP32Servo.h>
#include <MergeServos.h>

// Steppers gauche et droite
AccelStepper testStepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper testStepper2(AccelStepper::DRIVER, STEP2, DIR2);

// Wrapper des 2 steppers
MergeSteppers RobotSteppers(testStepper1, testStepper2, EN_DRIVER1, EN_DRIVER2);

// Servos gauche et droite
Servo testServo1;
Servo testServo2;

// Wrapper des 2 servos
MergeServos RobotServos(testServo1, testServo2, SERVO1, SERVO2);

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
        if (!testAlternanceLoop) {
            testAlternanceLoop = true;

            RobotSteppers.move_line(10000.0);

            RobotServos.write(180);
        } else {
            testAlternanceLoop = false;

            RobotSteppers.move_line(-10000.0);

            RobotServos.write(0);
        }
    }

    RobotSteppers.run();
}
