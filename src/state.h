#include <WiFi.h>
#include <definitions.h>

namespace BotState {
    typedef struct {
        WiFiClient active_client; // NULL when no client
        bool last_known_connected;

        char ins_buffer[INS_BUFFER_SIZE];
        unsigned int ins_buffer_len;
        unsigned int buffer_idx; // the instruction index of the buffer
        unsigned int overall_instructions_completed; // the num of pairs of motor movements processed
        bool awaiting_instructions; // initially true, so no motor code ran
    
        bool paused; // when true, motors will finish stepping current ins, then stop. independent of awaiting_instructions 
    } State;

    State create();
    void reset_state(State *state);
}
