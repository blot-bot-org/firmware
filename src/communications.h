#include <WiFi.h>
#include <definitions.h>
#include <interface.h>

//! 
//! Comments for functions can be found in the .cpp file.
//!

namespace LocalNetwork {
    void wifi_connect();
}

namespace TcpServer {
    WiFiServer generate_server(int port);

    void gen_header_bytes(unsigned char *byte_array, size_t size, unsigned int total_ins_completed);
    void gen_pause_feedback_bytes(unsigned char *byte_array, size_t size, bool is_paused, unsigned int total_ins_completed);
    void gen_feedback_bytes(unsigned char *byte_array, size_t size, unsigned int buffer_index);
}

void decompose_unsigned_integer(unsigned char *array_idx, unsigned int integer);
void decompose_unsigned_short(unsigned char *array_idx, unsigned short s);
