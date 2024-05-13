#include "Arduino.h"
#include "PamiDef.h"
#include "AccelStepper.h"
#include "MergeSteppers.h"
#include "ESP32Servo.h"
#include "ESP32Wiimote.h"

//#define DEBUG // à commenter en prod
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

// Objet wiimote
ESP32Wiimote wiimote;

void pollWiimote(void *pvParameters) {
	unsigned long previousMillis = 0;

	for (;;) {
		delay(10); // la wiimote fonctionne à 100Hz
		wiimote.task();

		if (wiimote.available() > 0) {
			ButtonState button = wiimote.getButtonState();

			switch (button) {
				case BUTTON_ONE:
					RobotSteppers.set_speed(STEP_SPEED * sqrtf(2));
					RobotSteppers.set_max_acceleration(STEP_ACCEL * sqrtf(2));
					RobotSteppers.stop();
					break;
				case BUTTON_TWO:
					// --- pas sûr, peut-être à retirer
					RobotSteppers.set_speed(STEP_SPEED);
					RobotSteppers.set_max_acceleration(STEP_ACCEL);
					// ---
					RobotSteppers.run();
					break;
				case BUTTON_A:
					tone(BUZZER, 666, 1000UL);
					break;
				case BUTTON_B:
					sweepJardiniere();
					break;
				case BUTTON_UP:
					stepperLeft.setMaxSpeed(STEP_SPEED / 2);
					stepperRight.setMaxSpeed(STEP_SPEED);
					break;
				case BUTTON_DOWN:
					stepperLeft.setMaxSpeed(STEP_SPEED);
					stepperRight.setMaxSpeed(STEP_SPEED / 2);
					break;
				default:
					RobotSteppers.set_speed(STEP_SPEED);
					RobotSteppers.set_max_acceleration(STEP_ACCEL);
					noTone(BUZZER);
					break;
			}
		}
	}
}

void sweepJardiniere() {
	for (int i = 1; i < 91; i++) {
		sg90.write(i);
		delay(5);
	}

	for (int i = 89; i >= 0; i--) {
		sg90.write(i);
		delay(5);
	}

	DEBUG_PRINTLN("Jardinière balayée");
}

void setup() {
    Serial.begin(115200);
    delay(250);
    DEBUG_PRINTLN("Mode Coupe-Off, ça va chier des bulles");
	wiimote.init();
	wiimote.addFilter(ACTION_IGNORE, FILTER_ACCEL);
	delay(250);
	DEBUG_PRINT("Init Wiimote OK");
	ESP32PWM::allocateTimer(3);
	sg90.setPeriodHertz(50);
	sg90.attach(SERVO2);
	RobotSteppers.set_max_acceleration(STEP_ACCEL);
    RobotSteppers.set_speed(STEP_SPEED);
    RobotSteppers.enable();
    //RobotSteppers.disable();
    delay(250);
    DEBUG_PRINT(" | Servos & Steppers OK");
	xTaskCreatePinnedToCore(pollWiimote, "wiimoteTask", 10000, NULL, 0, NULL, 0);
	delay(250);
	DEBUG_PRINT(" | Wiimote Core2 OK");
	pinMode(BUZZER, OUTPUT);
	delay(250);
	DEBUG_PRINT(" | Buzzer OK");
	sg90.write(0);
}

void loop() {
	// ça glande par ici
}
