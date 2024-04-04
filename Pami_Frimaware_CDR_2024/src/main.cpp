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

void strategy(int zone, int jardiniere) {
	// zone 1 = bleu
	// zone 2 = jaune
	switch (zone) {
		case 1:
			switch (jardiniere) {
				case 1:
					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(5920.0); // 400 mm

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					break;

				case 2:
					RobotSteppers.move_line(7400.0); // 500 mm

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					//RobotSteppers.move_arc(0, 150, 90);
					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					//RobotSteppers.set_max_acceleration(STEP_ACCEL);
					//RobotSteppers.set_speed(STEP_SPEED);

					RobotSteppers.move_line(11840.0);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					break;

				case 3:
					break;

				default:
					Serial.println("Erreur sélection jardinière [1, 2, 3]");
					break;
			}

			break;

		case 2:
			break;

		default:
			Serial.println("Erreur sélection de couleur équipe [1, 2]");
			break;
	}
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Liaison série OK");
    RobotSteppers.set_max_acceleration(STEP_ACCEL);
    RobotSteppers.set_speed(STEP_SPEED);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(1000);
    Serial.println("Steppers OK");
}

void loop() {
	static bool loopTest = true;

	if (loopTest) {
		//strategy(1, 2);
		RobotSteppers.set_current_coords(75, 1275, 0);
		RobotSteppers.turn_absolute(1000, 1600);

		while (!RobotSteppers.target_reached()) {
			RobotSteppers.run();
		}

		RobotSteppers.goto_absolute(1000, 1600);

		while (!RobotSteppers.target_reached()) {
			RobotSteppers.run();
		}

		loopTest = false;
	}
}
