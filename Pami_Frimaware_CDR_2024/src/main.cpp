#include "Arduino.h"
#include "PamiDef.h"
#include "AccelStepper.h"
#include "MergeSteppers.h"
#include "Ultrasonic.h"
#include "ESP32Servo.h"
#include "esp32-hal-timer.h"

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
bool obstacle = false, disableBuzzer = false;

// Team définie par le switch au départ
int team;

// Timer match PAMI
hw_timer_t *timerMatch = NULL;

void pollSonarDistance(void *pvParameters) {
	unsigned long previousMillis = 0;
	int readings[SONAR_ITERATIONS], sonar_index = 0, total = 0, average;

	for (;;) {
		if (millis() - previousMillis >= SONAR_PING_INTERVAL) {
			previousMillis = millis();

			noTone(BUZZER);

			total -= readings[sonar_index];
			readings[sonar_index] = sonar.read();
			total += readings[sonar_index];

			sonar_index = (sonar_index + 1) % SONAR_ITERATIONS;

			average = total / SONAR_ITERATIONS;

			if (average <= 31) {
				obstacle = true;

				if (!disableBuzzer) tone(BUZZER, 440);
			} else {
				obstacle = false;
			}

			DEBUG_PRINTLN(average);
		}
	}
}

void sweepJardiniere() {
	for (int i = 0; i <= 90; i++) {
		sg90.write(i);
		delay(4);
	}

	for (int i = 89; i > 0; i--) {
		sg90.write(i);
		delay(4);
	}
}

void makeItRun() {
	long tempDistance_L = 0, tempDistance_R = 0;

	while (stepperLeft.isRunning() || stepperRight.isRunning()) {
		RobotSteppers.run();

		if (obstacle) {
			DEBUG_PRINTLN("Obstacle en vue");

			tempDistance_L = stepperLeft.distanceToGo();
			tempDistance_R = stepperRight.distanceToGo();

			RobotSteppers.set_max_acceleration(STEP_ACCEL * 2);
			RobotSteppers.set_speed(STEP_SPEED * 2);

			RobotSteppers.move_line(0);

			tempDistance_L += stepperLeft.distanceToGo();
			tempDistance_R += stepperRight.distanceToGo();

			RobotSteppers.run();

			while (stepperLeft.isRunning() || stepperRight.isRunning()) {
				RobotSteppers.run();
			}

			while (obstacle) {
				DEBUG_PRINTLN("Obstacle toujours en vue");
			}

			RobotSteppers.set_max_acceleration(STEP_ACCEL);
			RobotSteppers.set_speed(STEP_SPEED);

			stepperLeft.move(tempDistance_L);
			stepperRight.move(tempDistance_R);
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
					RobotSteppers.move_line(200 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(900 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

					RobotSteppers.turn(45);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					sweepJardiniere();

					break;
				case 2:
					RobotSteppers.move_line(600 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1250 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

					sweepJardiniere();

					break;
				case 3:
					RobotSteppers.move_line(50 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(-35);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(2000 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

					break;
				default:
					DEBUG_PRINTLN("Erreur sélection jardinière [1, 2, 3]");

					break;
			}

			break;
		case 2:
			switch (jardiniere) {
				case 1:
					RobotSteppers.move_line(200 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(900 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

					RobotSteppers.turn(-45);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					sweepJardiniere();

					break;
				case 2:
					RobotSteppers.move_line(600 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1250 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

					sweepJardiniere();

					break;
				case 3:
					RobotSteppers.move_line(50 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					RobotSteppers.turn(35);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(2000 * STEP_PER_MM);

					/*
					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}
					*/

					makeItRun();

					disableBuzzer = true;

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

void IRAM_ATTR onTimer() {
	RobotSteppers.move_line(0);
	RobotSteppers.run();
	DEBUG_PRINTLN("Temps écoulé");
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
	sg90.setPeriodHertz(50);
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
	DEBUG_PRINT(" | Buzzer OK");
	sg90.write(0);
	timerMatch = timerBegin(0, 80, true);
	timerAttachInterrupt(timerMatch, &onTimer, true);
	timerAlarmWrite(timerMatch, TIMER_INTERVAL, false);
	DEBUG_PRINTLN(" | Timer match OK");
	waitingTirette_readSwitch();

	//delay(92000); // début après 92s de match
	timerAlarmEnable(timerMatch);
	strategy(team, 1);
}

void loop() {
	// ça glande par ici
}
