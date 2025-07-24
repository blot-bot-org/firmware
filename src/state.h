#include <WiFi.h>
#include <definitions.h>

//! 
//! Comments for functions can be found in the .cpp file.
//!

namespace BotState {
    ///
    /// A state-machine type object, existing for the lifetime of the program.
    /// When a drawing is started, most variables are reset to a default state.
    ///
    /// # Fields:
    /// - `active_client`: The WiFiClient, or NULL when there is no connection
    /// - `last_known_connected`: Whether the client was connected in the last loop iteration
    /// - `ins_buffer`: The instruction buffer
    /// - `buffer_idx`: A pointer to the current instruction, in the instruction buffer
    /// - `overall_instructions_completed`: The number of total motor movements processed (dont think this is used anymore)
    /// - `awaiting_instructions`: True when the motor has requested instructions, and is waiting for more
    /// - `paused`: True if the client has requested a pause - when true, the current instruction will still finish stepping
    ///
    typedef struct {
        WiFiClient active_client; // NULL when no client
        bool last_known_connected;

        char ins_buffer[INS_BUFFER_SIZE];
        unsigned int ins_buffer_len;
        unsigned int buffer_idx; // the instruction index of the buffer
        unsigned int overall_instructions_completed; // the num of pairs of motor movements processed
        bool awaiting_instructions; // initially true, so no motor code ran
        bool first_buf_received;
    
        bool paused; // when true, motors will finish stepping current ins, then stop. independent of awaiting_instructions 
    } State;

    State create();
    void reset_state(State *state);
}
