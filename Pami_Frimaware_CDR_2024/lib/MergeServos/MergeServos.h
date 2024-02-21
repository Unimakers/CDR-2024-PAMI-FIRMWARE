#pragma once
#include <ESP32Servo.h>

class MergeServos {
    protected:
        Servo &left, &right;
        const int attach1_pin = 0;
        const int attach2_pin = 0;

    public:
        MergeServos(Servo &L, Servo &R, int pin_attach1, int pin_attach2);
        ~MergeServos();
        void write(int angle);
};
