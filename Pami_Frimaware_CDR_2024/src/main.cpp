#include "Arduino.h"
#include "PamiDef.h"
#include "AccelStepper.h"
#include "MergeSteppers.h"
#include "ESP32Servo.h"
#include "Ultrasonic.h"

#define DEBUG // à commenter en prod
#ifdef DEBUG
	#define DEBUG_PRINTLN(x) Serial.println(x)
	#define DEBUG_PRINT(x) Serial.print(x)
#else
	#define DEBUG_PRINTLN(x)
	#define DEBUG_PRINT(x)
#endif

// Steppers gauche et droite
AccelStepper stepperLeft(AccelStepper::DRIVER, STEP2, DIR2), stepperRight(AccelStepper::DRIVER, STEP1, DIR1);

// Wrapper des 2 steppers
MergeSteppers RobotSteppers(stepperLeft, stepperRight, EN_DRIVER1, EN_DRIVER2);

// Servos gauche et droite
//Servo servoLeft, servoRight;

// Capteur ultrason
Ultrasonic sonar(SERVO1, SERVO2);
bool obstacle = false;

void pollSonarDistance(void *pvParameters) {
	unsigned long previousMillis = 0;
	int readings[SONAR_ITERATIONS], sonar_index = 0, total = 0;

	for (;;) {
		unsigned long currentMillis = millis();

		if (currentMillis - previousMillis >= SONAR_PING_INTERVAL) {
			previousMillis = currentMillis;

			total -= readings[sonar_index];
			readings[sonar_index] = sonar.read();
			total += readings[sonar_index];

			sonar_index = (sonar_index + 1) % SONAR_ITERATIONS;

			int average = total / SONAR_ITERATIONS;

			if (average <= 20) {
				obstacle = true;

				tone(BUZZER, 440);
				delay(SONAR_PING_INTERVAL);
				noTone(BUZZER);
			} else {
				obstacle = false;
			}

			DEBUG_PRINTLN(average);
		}
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
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				case 2:
					RobotSteppers.move_line(525 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1200 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				case 3:
					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				default:
					DEBUG_PRINTLN("Erreur sélection jardinière [1, 2, 3]");

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
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				case 2:
					RobotSteppers.move_line(525 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1200 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				case 3:
					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1325 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					break;
				default:
					DEBUG_PRINTLN("Erreur sélection jardinière [1, 2, 3]");

					break;
			}

			break;
		default:
			DEBUG_PRINTLN("Erreur sélection de couleur équipe [1, 2]");

			break;
	}
}

void waitingTirette() {
	while (digitalRead(TIRETTE));
	tone(BUZZER, 261);
	delay(200);
	noTone(BUZZER);
	while (!digitalRead(TIRETTE));
	tone(BUZZER, 329);
	delay(200);
	noTone(BUZZER);
}

void setup() {
    Serial.begin(115200);
    delay(250);
    DEBUG_PRINT("Liaison série OK");
	pinMode(TIRETTE, INPUT_PULLUP);
	delay(250);
	DEBUG_PRINT(" | Tirette OK");
    RobotSteppers.set_max_acceleration(STEP_ACCEL);
    RobotSteppers.set_speed(STEP_SPEED);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(250);
    DEBUG_PRINT(" | Steppers OK");
	xTaskCreatePinnedToCore(pollSonarDistance, "sonarTask", 10000, NULL, 0, NULL, 0);
	delay(250);
	DEBUG_PRINT(" | Sonar Core2 OK");
	pinMode(BUZZER, OUTPUT);
	delay(250);
	DEBUG_PRINTLN(" | Buzzer OK");

	waitingTirette();
}

void loop() {
	static bool loopTest = true;

	if (loopTest) {
		strategy(1, 2);

		loopTest = false;
	}
}
