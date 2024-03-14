#include <MergeSteppers.h>

MergeSteppers::MergeSteppers(AccelStepper &L, AccelStepper &R, int enable_pin1, int enable_pin2):left(L), right(R), en1_pin(enable_pin1), en2_pin(enable_pin2) {
    pinMode(en1_pin, OUTPUT);
    pinMode(en2_pin, OUTPUT);
    digitalWrite(en1_pin, HIGH);
    digitalWrite(en2_pin, HIGH);

    leftover_distance.left_distance = 0;
    leftover_distance.right_distance = 0;
}

MergeSteppers::~MergeSteppers() {

}

void MergeSteppers::set_speed(float speed) {
    left.setMaxSpeed(speed);
    right.setMaxSpeed(speed);
}

void MergeSteppers::set_max_acceleration(float acceleration) {
    left.setAcceleration(acceleration);
    right.setAcceleration(acceleration);
}

StepperDistance MergeSteppers::distance_to_go() {
    StepperDistance tmp;

    tmp.left_distance = left.distanceToGo();
    tmp.right_distance = right.distanceToGo();

    return tmp;
} 

bool MergeSteppers::target_reached() {
    StepperDistance tmp = distance_to_go();

    return (tmp.left_distance == 0 && tmp.right_distance == 0);
}

void MergeSteppers::move_line(long distance) {
    left.move(distance);
    right.move(-distance);
}

void MergeSteppers::move_arc(int side, int angle, int radius) {
    float big_arc = radius + HALF_RAYON;
    float small_arc = radius - HALF_RAYON;

    float MM_do_big_arc = (PI * angle * big_arc) / 180.0;
    float MM_do_small_arc = (PI * angle * small_arc) / 180.0;

    long Steps_do_big_arc = MM_do_big_arc * STEP_PER_MM;
    long Steps_do_small_arc = MM_do_small_arc * STEP_PER_MM;

    switch (side) {
        case left_arc:
            left.move(Steps_do_big_arc);
            right.move(Steps_do_small_arc);
            break;
        case right_arc:
            left.move(Steps_do_small_arc);
            right.move(Steps_do_big_arc);
            break;
        default:
            Serial.println("Erreur ! Avez-vous spécifié un type de mouvement d'arc ?");
            break;
    }
}

void MergeSteppers::turn(int angle) {
    float Steps_to_do = (PI * angle * RAYON * STEP_PER_MM) / 180;

    left.move(Steps_to_do);
    right.move(Steps_to_do);
}

void MergeSteppers::stop() {
    if (target_reached()) {
        pending_stop = false;

        leftover_distance.left_distance = 0;
        leftover_distance.right_distance = 0;

        return;
    }

    if (pending_stop) {
        return;
    } else {
        pending_stop = true;

        leftover_distance.left_distance = left.distanceToGo();
        leftover_distance.right_distance = right.distanceToGo();

        left.stop();
        right.stop();

        leftover_distance.left_distance -= left.distanceToGo();
        leftover_distance.right_distance -= right.distanceToGo();
    }
}

bool MergeSteppers::get_pending_stop() {
    return pending_stop;
}

void MergeSteppers::resume() {
    pending_stop = false;

    left.move(leftover_distance.left_distance + left.distanceToGo());
    right.move(leftover_distance.right_distance + right.distanceToGo());
}

void MergeSteppers::enable() {
    digitalWrite(en1_pin, LOW);
    digitalWrite(en2_pin, LOW);
}

void MergeSteppers::disable() {
    digitalWrite(en1_pin, HIGH);
    digitalWrite(en2_pin, HIGH);

    left.move(0);
    right.move(0);
}

void MergeSteppers::run() {
    left.run();
    right.run();
}
