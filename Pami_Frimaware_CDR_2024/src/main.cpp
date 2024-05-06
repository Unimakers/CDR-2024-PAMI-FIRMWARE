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
bool obstacle = false, disableBuzzer = false;

// Team définie par le switch au départ
int team;

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

			if (average <= 30) {
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

void strategy(int zone, int jardiniere) {
	// zone = 1 = bleu
	// zone = 2 = jaune
	switch (zone) {
		case 1:
			switch (jardiniere) {
				case 1:
					RobotSteppers.move_line(200 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(900 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					disableBuzzer = true;

					RobotSteppers.turn(45);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					sweepJardiniere();

					break;
				case 2:
					RobotSteppers.move_line(600 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(-90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(1250 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					disableBuzzer = true;

					sweepJardiniere();

					break;
				case 3:
					RobotSteppers.move_line(50 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.turn(-35);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(2000 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

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
					RobotSteppers.move_line(50 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.turn(90);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(375 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						if (!obstacle) {
							RobotSteppers.run();
						}
					}

					RobotSteppers.turn(90);

					disableBuzzer = true;

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					RobotSteppers.move_line(60 * STEP_PER_MM);

					while (!RobotSteppers.target_reached()) {
						RobotSteppers.run();
					}

					sweepJardiniere();

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
	DEBUG_PRINTLN(" | Buzzer OK");
	sg90.write(0);
	waitingTirette_readSwitch();
}

void loop() {
	static bool loopTest = true;

	//delay(90500); // début après 90.5s de match

	if (loopTest) {
		strategy(team, 1);
		loopTest = false;
	}
}
