#include "Arduino.h"
#include "PamiDef.h"
#include "AccelStepper.h"
#include "MergeSteppers.h"
#include "Ultrasonic.h"
#include "ESP32Servo.h"

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

// Servo pour jardinière
Servo sg90;

// Capteur ultrason
Ultrasonic sonar(TRIGGER, ECHO);
bool obstacle = false;

// Team pour le match
int team;

void pollSonarDistance(void *pvParameters) {
	unsigned long previousMillis = 0, currentMillis;
	int readings[SONAR_ITERATIONS], sonar_index = 0, total = 0, average;

	for (;;) {
		currentMillis = millis();

		if (currentMillis - previousMillis >= SONAR_PING_INTERVAL) {
			previousMillis = currentMillis;

			noTone(BUZZER);

			total -= readings[sonar_index];
			readings[sonar_index] = sonar.read();
			total += readings[sonar_index];

			sonar_index = (sonar_index + 1) % SONAR_ITERATIONS;

			average = total / SONAR_ITERATIONS;

			if (average <= 20) {
				obstacle = true;
				tone(BUZZER, 440);
			} else {
				obstacle = false;
			}

			//DEBUG_PRINTLN(average);
		}
	}
}

void strategy(int zone, int jardiniere) {
	// zone = 1 = bleu
	// zone = 2 = jaune
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

void waitingTirette_readSwitch() {
	while (digitalRead(TIRETTE));
	tone(BUZZER, 261);
	delay(200);
	noTone(BUZZER);
	while (!digitalRead(TIRETTE));
	tone(BUZZER, 329);
	delay(200);
	noTone(BUZZER);

	switch (digitalRead(SW_TEAM)) {
		case 0:
			team = 2;
			DEBUG_PRINTLN("Team bleue");
			break;

		case 1:
			team = 1;
			DEBUG_PRINTLN("Team jaune");
			break;

		default:
			DEBUG_PRINTLN("Erreur sélection équipe à l'aide du switch");
			break;
	}
}

void setup() {
    Serial.begin(115200);
    delay(250);
    DEBUG_PRINT("Liaison série OK");
	pinMode(TIRETTE, INPUT_PULLUP);
	pinMode(SW_TEAM, INPUT_PULLUP);
	delay(250);
	DEBUG_PRINT(" | Tirette & SwitchTeam OK");
	ESP32PWM::allocateTimer(3);
	sg90.attach(SERVO2);
	RobotSteppers.set_max_acceleration(STEP_ACCEL);
    RobotSteppers.set_speed(STEP_SPEED);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(250);
    DEBUG_PRINT(" | Servos & Steppers OK");
	xTaskCreatePinnedToCore(pollSonarDistance, "sonarTask", 10000, NULL, 0, NULL, 0);
	delay(250);
	DEBUG_PRINT(" | Sonar Core2 OK");
	pinMode(BUZZER, OUTPUT);
	delay(250);
	DEBUG_PRINTLN(" | Buzzer OK");
	waitingTirette_readSwitch();
}

void loop() {
	static bool loopTest = true;

	//delay(91000); // début après 91s match

	if (loopTest) {
		strategy(team, 2);

		loopTest = false;
	}

	/*
	sg90.write(0);
	DEBUG_PRINTLN("0°");
	delay(1000);
	sg90.write(180);
	DEBUG_PRINTLN("180°");
	delay(1000);
	*/
}
