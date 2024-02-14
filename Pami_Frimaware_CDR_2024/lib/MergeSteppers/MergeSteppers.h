#pragma once
#include <AccelStepper.h>

#define RAYON 80
#define STEPPERMM 1

typedef enum {
    left_arc = 0,
    right_arc = 1
} Arc;

typedef struct {
    long left_distance;
    long right_distance;
} StepperDistance;

class MergeSteppers {
    protected:
        StepperDistance leftover_distance;
        AccelStepper &left, &right;
        StepperDistance distance_to_go();
        const int en1_pin = 0;
        const int en2_pin = 0;
        bool pending_stop = false;

    public:
        MergeSteppers(AccelStepper &L, AccelStepper &R);
        MergeSteppers(AccelStepper &L, AccelStepper &R, int enable_pin1, int enable_pin2);
        ~MergeSteppers();
        void set_speed(float speed);
        void set_max_acceleration(float acceleration);
        bool target_reached();
        void move_line(long distance);
        void move_arc(int side, int angle, int radius);
        void turn(int angle);
        void stop();
        bool get_pending_stop();
        void resume();
        void enable();
        void disable();
        void run();
};
