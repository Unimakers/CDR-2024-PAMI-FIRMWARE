#pragma once
#include "AccelStepper.h"

#define RAYON 47.5
#define HALF_RAYON (RAYON / 2)
#define STEP_PER_MM 14.8
#define STEP_SPEED 10000.0
#define STEP_ACCEL (STEP_SPEED / 2)

typedef struct {
    long left_distance;
    long right_distance;
} StepperDistance;

typedef struct {
	int x, y;
	float o;
} Coords;

class MergeSteppers {
    protected:
        StepperDistance leftover_distance;
        AccelStepper &left, &right;
		Coords currentCoords;
        const int en1_pin;
        const int en2_pin;
        bool pending_stop = false;

    public:
        MergeSteppers(AccelStepper &L, AccelStepper &R, int enable_pin1, int enable_pin2);
        ~MergeSteppers();
        void set_speed(float speed);
        void set_max_acceleration(float acceleration);
		void set_current_coords(int x, int y, float o);
        bool target_reached();
        void move_line(long distance);
		void goto_absolute(int x, int y);
		void move_to(long pos);
        void move_arc(int side, float radius, int angle);
        void turn(int angle);
		void turn_absolute(int x, int y);
        void stop();
        bool get_pending_stop();
        void resume();
        void enable();
        void disable();
        void run();
		StepperDistance distance_to_go();
};
