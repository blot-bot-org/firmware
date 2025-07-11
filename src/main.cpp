#include <cstring>
#include <definitions.h>
#include <communications.h>
#include <motor_controller.h>
#include <interface.h>
#include <state.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AccelStepper.h>
#include <ESP32Servo.h>


/* ----- globals ----- */
WiFiServer socket_server = TcpServer::generate_server(SOCKET_PORT);
BotState::State state;

AccelStepper left_motor = init_stepper_motor(L_PULSE, L_DIRECTION);
AccelStepper right_motor = init_stepper_motor(R_PULSE, R_DIRECTION);
Servo gondola_servo;
/* ------------------ */


/// 
/// Called when the ESP32 initialised.
///
void setup() {
    Serial.begin(BAUD_RATE);
    pinMode(LED, OUTPUT);
    gondola_servo.attach(33);
    gondola_servo.write(0); // pen up

    BotState::reset_state(&state);

    LocalNetwork::wifi_connect();
    socket_server.begin();

}


/// 
/// Loops on the ESP32.
///
void loop() {

    if (socket_server.hasClient()) {
        if(!state.last_known_connected) { // client trying to join, when we have no active client. therefore register it.
            state.active_client = socket_server.available();
            state.last_known_connected = true;
            Serial.println("[debug] The client has connected!");
        } else { // otherwise it's another client trying to join. we will just shut it down. ideally we should send an error message.
            WiFiClient tc = socket_server.available();
            tc.write((uint8_t) 0x00); // this should also write protocol: TODO!
            tc.stop();
        }
    }

    // if the socket was connected last iter, but isn't now, it just left. therefore deregister it.
    if (state.last_known_connected && !state.active_client.connected()) {
        Serial.println("[debug] The client has disconnected!");
        state.active_client.stop();

        gondola_servo.write(0); // pen up

        state.last_known_connected = false;
    }


    if(!(state.last_known_connected && state.active_client.connected())) { return; } // if there is no active socket client
    
    
    if(state.active_client.available() != 0) { // we have bytes available for reading
        char header_byte = state.active_client.read();
        if(header_byte == 0xFF) {
            Serial.println("[err] Error reading header byte from client. Byte was said to be available, but wasn't.");
            esp32_exit();
        }

        // we check the header byte to see what the client is intending
        switch(header_byte) {
            case 0x00: {
                Serial.println("[debug] A new client has initialised a drawing. Resetting the firmware state...");
                BotState::reset_state(&state);
                gondola_servo.write(0); // pen up

                // load any other useful initialising parameters here.
                unsigned char header_bytes[20];
                TcpServer::gen_header_bytes(header_bytes, sizeof(header_bytes), state.overall_instructions_completed);

                state.active_client.write(header_bytes, sizeof(header_bytes));
                Serial.println("[debug] Send header bytes!");

                state.active_client.write(0x03); // ask for the initial instructions
                
                return; // will keep iterating until instructions received
            }

            case 0x01: {
                if(state.active_client.available() > INS_BUFFER_SIZE) {
                    Serial.println("[err] Error receiving instructions. Client sent more bytes than buffer can handle.");
                    esp32_exit();
                }

                memset(state.ins_buffer, 0x00, INS_BUFFER_SIZE);
                size_t bytes_received = state.active_client.readBytes(state.ins_buffer, INS_BUFFER_SIZE);
                Serial.print("[debug] Read ");
                Serial.print(bytes_received);
                Serial.println(" bytes of instruction data.");
                state.awaiting_instructions = false;
                state.ins_buffer_len = bytes_received;

                // okay so sometimes the 0x01 prefix is there, sometimes it isn't.
                // so what ill do is check if the length of the instruction bytes is a multiple
                // of 5, if not, trim it to be so. this limits instructions to not be penup/pendown.
                
                state.buffer_idx = state.ins_buffer_len % 5;

                // gotta ignore the header byte idk why it's not being removed from the buffer
                // when im LITERALLY READING IT EARLIER but honestly idec spent too long on this lets just make it work

                return; // next iter will start drawing
            }

            case 0x02: {
                Serial.println("[debug] The client has indicated the drawing has finished. The client will be disconnected.");

                state.active_client.stop();
                state.last_known_connected = false;
                state.overall_instructions_completed = 0;
                gondola_servo.write(0); // pen up

                return;
            }

            case 0x04: {
                char pause_byte = state.active_client.read();
                if(pause_byte == -1) {
                    Serial.println("[err] Error reading socket data. The client sent a pause header but didn't specify the state.");
                    esp32_exit();
                }
                // only 0x00 unpauses, anything else pauses technically. ill classify this as a feature.
                bool pause_flag = pause_byte;

                Serial.print("[debug] The client has ");
                if(pause_flag) {
                    Serial.print("paused");
                } else {
                    Serial.print("unpaused");
                }
                Serial.println(" the drawing execution.");

                state.paused = pause_flag;


                unsigned char pause_feedback_bytes[6];
                TcpServer::gen_pause_feedback_bytes(pause_feedback_bytes, sizeof(pause_feedback_bytes), state.paused, state.overall_instructions_completed);
                state.active_client.write(pause_feedback_bytes, sizeof(pause_feedback_bytes));

                return;
            }

            case 0x05: {
                Serial.println("[debug] Client cancelled drawing. The client will be disconnected.");
                state.active_client.write(0x05); // ask for the initial instructions

                state.active_client.stop();
                state.last_known_connected = false;
                state.overall_instructions_completed = 0;
                gondola_servo.write(0); // pen up

                return;
            }

            default: {
                Serial.println("[err] Error receiving client data. The header byte was invalid.");
                esp32_exit();
            }
        }
    }


    // beyond this point the client had no message, meaning we're either waiting for instructions, or executing instructions.

    // if we are waiting for instructions, just keep waiting. i've added a little delay.
    if(state.awaiting_instructions) { delay(100); return; }

    // if the motors have to move, just move em and iterate.
    if(has_movement(&left_motor, &right_motor)) {
        left_motor.runSpeedToPosition();
        right_motor.runSpeedToPosition();

        return;
    }
    

    // now that we have completed an instruction, check if the client is still connected. if not, we pause instruction execution.
    if(!(state.last_known_connected && state.active_client.connected())) { return; } // if there is no active socket client
    
    // if we are here the motors had no movement, so we need to move on to the next instruction

    if(state.paused) {
        return; // if the bot is paused, don't execute anymore motor instructions
    }
    
    // get next idx of 0x0C (end of instruction) byte
    int next_eoi_idx = state.buffer_idx + 4; // jump 4 movement bytes ahead to where the 0x0C will be at or near
    if(next_eoi_idx >= state.ins_buffer_len) { // if there are no more bytes left, inform the client and await instructions.
        Serial.println("[debug] Ran out of instructions... awaiting more.");
        state.awaiting_instructions = true;
        state.active_client.write(0x03); // send out of instruction byte

        return;
    }

    while(next_eoi_idx < state.ins_buffer_len) {
        if(state.ins_buffer[next_eoi_idx] == 0x0C) break;
        next_eoi_idx += 1;
    }
    
    unsigned char command_length = next_eoi_idx - (state.buffer_idx);
    if(command_length < 4) {
        Serial.println("[err] Error reading instruction. The instruction was not at least 4 bytes, so the movement is unknown.");
        esp32_exit();
    }
    
    // parse 2 big endian bytes into unsigned short
    short left_motor_steps = (static_cast<short> (state.ins_buffer[state.buffer_idx]) << 8 | state.ins_buffer[state.buffer_idx + 1]);
    short right_motor_steps = (static_cast<short> (state.ins_buffer[state.buffer_idx + 2]) << 8 | state.ins_buffer[state.buffer_idx + 3]);
    if (command_length == 5 && state.ins_buffer[state.buffer_idx + 4] == 0x0A || state.ins_buffer[state.buffer_idx + 4] == 0x0B) {
        
        if(state.ins_buffer[state.buffer_idx + 4] == 0x0A) {
            gondola_servo.write(0);
            delay(300);
        } else {
            gondola_servo.write(90);
            delay(300);
        }

    }

    /*
    Serial.print("[debug] Running: ");
    Serial.print(left_motor_steps);
    Serial.print("<->");
    Serial.println(right_motor_steps);

    Serial.print("New command as int: ");
    Serial.println((int) command_length);
    */

    // tell motors to move
    left_motor.move(left_motor_steps);
    right_motor.move(right_motor_steps);
    // and set their speeds
    left_motor.setSpeed(get_left_motor_speed(left_motor_steps, right_motor_steps));
    right_motor.setSpeed(get_right_motor_speed(left_motor_steps, right_motor_steps));

    state.overall_instructions_completed++;

    uint8_t feedback_bytes[5];
    TcpServer::gen_feedback_bytes(feedback_bytes, sizeof(feedback_bytes), state.buffer_idx);
    // state.active_client.write(feedback_bytes, sizeof(feedback_bytes));

    state.buffer_idx = next_eoi_idx + 1; // +1 to move the idx to the next real instruction byte, otherwise its on the 0x0C
}
