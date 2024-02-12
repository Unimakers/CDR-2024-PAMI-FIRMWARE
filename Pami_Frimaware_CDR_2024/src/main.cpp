#include <Arduino.h>
#include <PamiDef.h>
#include <AccelStepper.h>
#include <MergeSteppers.h>

// Steppers gauche et droite
AccelStepper testStepper1(AccelStepper::DRIVER, STEP1, DIR1);
AccelStepper testStepper2(AccelStepper::DRIVER, STEP2, DIR2);

// Wrapper des 2 steppers
MergeSteppers Robot(testStepper1, testStepper2, EN_DRIVER1, EN_DRIVER2);

int alternance = 0;

void setup() {
    Serial.begin(115200);
    delay(20);
    Serial.println("Liaison série OK");
    testStepper1.setAcceleration(2000.0);
    testStepper1.setMaxSpeed(1000.0);
    testStepper2.setAcceleration(2000.0);
    testStepper2.setMaxSpeed(1000.0);
    delay(20);
    pinMode(EN_DRIVER1, OUTPUT);
    pinMode(EN_DRIVER2, OUTPUT);
    delay(20);
    digitalWrite(EN_DRIVER1, LOW);
    digitalWrite(EN_DRIVER2, LOW);
    Serial.println("Steppers OK");
    delay(1000);
}

bool target_reached(AccelStepper stepper) {
    return (stepper.currentPosition() == stepper.targetPosition());
}

void loop() {
    if (target_reached(testStepper1) && target_reached(testStepper2)) {
        if (alternance == 0) {
            alternance = 1;
            testStepper1.move(1000.0);
            testStepper2.move(-1000.0);
        } else {
            alternance = 0;
            testStepper1.move(-1000.0);
            testStepper2.move(1000.0);
        }
    }
    testStepper1.run();
    testStepper2.run();
}
