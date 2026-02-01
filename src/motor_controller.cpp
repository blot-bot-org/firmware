#include <motor_controller.h>

/// 
/// Left and right motors move linearly, with one motor always maxed out.
/// Hence, if the left motor needs 1000 steps, and the right 200
/// the left motor will move at max speed (say 500 steps/s)
/// and the right motor will move proportionally (say 100 steps/s).
///

/// 
/// This function returns the left motor speed, given the left and right
/// motor steps, as defined above.
///
/// # Parameters:
/// - `left_steps`: The amount of steps the left motor must perform
/// - `right_steps`: The amount of steps the right motor must perform
///
/// # Returns:
/// - The steps/s of the left motor
/// 
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

/// 
/// This function returns the right motor speed, given the left and right
/// motor steps, as defined above.
///
/// # Parameters:
/// - `left_steps`: The amount of steps the left motor must perform
/// - `right_steps`: The amount of steps the right motor must perform
///
/// # Returns:
/// - The steps/s of the right motor
/// 
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

/// 
/// # Returns:
/// - `true` if motors have finished their movement, else `false`
///
bool has_movement(AccelStepper *lm, AccelStepper *rm) {
    return lm->distanceToGo() != 0 || rm->distanceToGo() != 0;
}

/// 
/// # Returns:
/// - An initialised AccelStepper object, with pins defined by `definitions.h`
///
AccelStepper init_stepper_motor(uint8_t enable_pin, uint8_t pulse_pin, uint8_t direction_pin) {
    // interface type is 1, https://www.airspayce.com/mikem/arduino/AccelStepper/classAccelStepper.html#a73bdecf1273d98d8c5fbcb764cabeea5
    AccelStepper object(1, pulse_pin, direction_pin);
    object.setEnablePin(enable_pin);
    object.setPinsInverted(false, false, true);

    object.setMaxSpeed(MAX_MOTOR_SPEED);
    object.setMinPulseWidth(MIN_PULSE_WIDTH);
    object.enableOutputs(); // sets pins to OUTPUT
    
    return object;
}

