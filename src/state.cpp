#include <cstring>
#include <state.h>
#include <definitions.h>

namespace BotState {
    
    ///
    /// Creates a new state object.
    /// Default values are defined in this function.
    /// 
    /// # Returns:
    /// - A new state object, with default values
    ///
    State create() {
        State state;

        state.active_client = WiFiClient();
        state.last_known_connected = false;
        state.first_buf_received = false;

        reset_state(&state);
        state.overall_instructions_completed = 0;

        return state;
    }

    ///
    /// Resets a state object.
    /// Default reset values are defined in this function.
    /// 
    /// # Parameters:
    /// - `state`: The current state object
    ///
    /// # Returns:
    /// - A new state object, with default values
    ///
    void reset_state(State *state) {
        memset(state->ins_buffer, 0x00, INS_BUFFER_SIZE); // clear instruction buffer
    
        state->awaiting_instructions = true; // await instructions
        state->buffer_idx = 0;
        state->ins_buffer_len = 0;
        state->paused = false;
        state->first_buf_received = false;
    }

}
