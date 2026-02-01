#include <definitions.h>
#include <AccelStepper.h>
#include <cmath>

//! 
//! Comments for functions can be found in the .cpp file.
//!

float get_left_motor_speed(short left_steps, short right_steps);
float get_right_motor_speed(short left_steps, short right_steps);
bool has_movement(AccelStepper *lm, AccelStepper *rm);

AccelStepper init_stepper_motor(uint8_t enable_pin, uint8_t pulse_pin, uint8_t direction_pin);
