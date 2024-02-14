#include <Arduino.h>
#include <PamiDef.h>
#include <AccelStepper.h>
#include <MergeSteppers.h>
#include <ESP32Servo.h>

// Steppers gauche et droite
AccelStepper testStepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper testStepper2(AccelStepper::DRIVER, STEP2, DIR2);

// Wrapper des 2 steppers
MergeSteppers Robot(testStepper1, testStepper2, EN_DRIVER1, EN_DRIVER2);

// Servo
Servo testServo1;
Servo testServo2;

int alternance = 0;

void setup() {
    Serial.begin(115200);
    delay(20);
    Serial.println("Liaison série OK");
    Robot.set_max_acceleration(2000.0);
    Robot.set_speed(1000.0);
    Robot.enable();
    //Robot.disable();
    delay(20);
    Serial.println("Steppers OK");
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    testServo1.setPeriodHertz(50);
    testServo1.attach(SERVO1);
    testServo2.setPeriodHertz(50);
    testServo2.attach(SERVO2);
    delay(20);
    Serial.println("Servos OK");
}

void loop() {
    if (Robot.target_reached()) {
        if (alternance == 0) {
            alternance = 1;

            Robot.move_line(1000.0);

            testServo1.write(0);
            testServo2.write(180);
        } else {
            alternance = 0;

            Robot.move_line(-1000.0);

            testServo1.write(180);
            testServo2.write(0);
        }
    }

    Robot.run();
}
