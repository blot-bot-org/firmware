#include <motor_controller.h>

float get_left_motor_speed(short left_steps, short right_steps) {
    if(left_steps == right_steps) {
        return MAX_MOTOR_SPEED;
    }

    if(left_steps > right_steps) {
        return MAX_MOTOR_SPEED;
    }

    if(left_steps < right_steps) {
        return ((float) MAX_MOTOR_SPEED / abs(right_steps)) * (abs(left_steps));
    }

    return 0; // impossible
}
float get_right_motor_speed(short left_steps, short right_steps) {
    if(left_steps == right_steps) {
        return MAX_MOTOR_SPEED;
    }

    if(right_steps > left_steps) {
        return MAX_MOTOR_SPEED;
    }

    if(right_steps < left_steps) {
        return ((float) MAX_MOTOR_SPEED / abs(left_steps)) * (abs(right_steps));
    }

    return 0; // impossible
}

bool has_movement(AccelStepper *lm, AccelStepper *rm) {
    return lm->distanceToGo() != 0 || rm->distanceToGo() != 0;
}

AccelStepper init_stepper_motor(uint8_t pulse_pin, uint8_t direction_pin) {
    // interface type is 1, https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a73bdecf1273d98d8c5fbcb764cabeea5
    AccelStepper object(1, pulse_pin, direction_pin);

    object.setMaxSpeed(MAX_MOTOR_SPEED);
    object.setMinPulseWidth(MIN_PULSE_WIDTH);
    object.enableOutputs(); // sets pins to OUTPUT
    
    return object;
}
