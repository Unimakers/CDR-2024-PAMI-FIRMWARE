#include "Arduino.h"
#include "PamiDef.h"
#include "AccelStepper.h"
#include "MergeSteppers.h"
#include "ESP32Servo.h"
#include "Ultrasonic.h"

// Steppers gauche et droite
AccelStepper stepperLeft(AccelStepper::DRIVER, STEP2, DIR2), stepperRight(AccelStepper::DRIVER, STEP1, DIR1);

// Wrapper des 2 steppers
MergeSteppers RobotSteppers(stepperLeft, stepperRight, EN_DRIVER1, EN_DRIVER2);

// Servos gauche et droite
//Servo servoLeft, servoRight;

// Capteur ultrason et cache valeur
Ultrasonic sonar(SERVO1, SERVO2);
unsigned int sonarDistanceCached;

void getSonarDistance(void *pvParameters) {
	for (;;) {
		sonarDistanceCached = sonar.read();
	}
}

void strategy(int zone, int jardiniere) {
	// zone 1 = bleu
	// zone 2 = jaune
	switch (zone) {
		case 1:
			switch (jardiniere) {
				case 1:
					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(400 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				case 2:
					RobotSteppers.move_line(525 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1200 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				case 3:
					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				default:
					Serial.println("Erreur sélection jardinière [1, 2, 3]");

					break;
			}

			break;
		case 2:
			switch (jardiniere) {
				case 1:
					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(400 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				case 2:
					RobotSteppers.move_line(525 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1200 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				case 3:
					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (sonarDistanceCached <= 20) {
							continue;
						} else {
							RobotSteppers.run();
						}
					}

					break;
				default:
					Serial.println("Erreur sélection jardinière [1, 2, 3]");

					break;
			}

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
	xTaskCreatePinnedToCore(getSonarDistance, "sonarTask", 10000, NULL, 0, NULL, 0);
	delay(1000);
	Serial.println("Sonar Core2 OK");
}

void loop() {
	static bool loopTest = true;

	if (loopTest) {
		strategy(1, 2);

		loopTest = false;
	}
}
