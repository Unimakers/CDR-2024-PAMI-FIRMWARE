#include <MergeServos.h>

MergeServos::MergeServos(Servo &L, Servo &R, int pin_attach1, int pin_attach2):left(L), right(R), attach1_pin(pin_attach1), attach2_pin(pin_attach2) {
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    left.setPeriodHertz(50);
    left.attach(attach1_pin);
    right.setPeriodHertz(50);
    right.attach(attach2_pin);
}

MergeServos::~MergeServos() {

}

void MergeServos::write(int angle) {
    left.write(angle);
    right.write(180 - angle);
}
