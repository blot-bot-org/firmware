#include <cstring>
#include <state.h>
#include <definitions.h>

namespace BotState {

    State create() {
        State state;

        state.active_client = WiFiClient();
        state.last_known_connected = false;

        reset_state(&state);
        state.overall_instructions_completed = 0;

        return state;
    }

    void reset_state(State *state) {
        memset(state->ins_buffer, 0x00, INS_BUFFER_SIZE); // clear instruction buffer
    
        state->awaiting_instructions = true; // await instructions
        state->buffer_idx = 0;
        state->ins_buffer_len = 0;
        state->paused = false;
    }

}
