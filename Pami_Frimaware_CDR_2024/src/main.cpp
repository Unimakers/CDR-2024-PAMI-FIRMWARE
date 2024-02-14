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
    Robot.set_max_acceleration(2000.0);
    Robot.set_speed(1000.0);
    delay(20);
    Robot.enable();
    //Robot.disable();
    Serial.println("Steppers OK");
    delay(1000);
}

void loop() {
    if (Robot.target_reached()) {
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
    Robot.run();
}
