#include <WiFi.h>
#include <definitions.h>
#include <interface.h>

namespace LocalNetwork {
    void wifi_connect();
}

namespace TcpServer {
    WiFiServer generate_server(int port);

    void gen_header_bytes(unsigned char *byte_array, size_t size);
    void gen_feedback_bytes(unsigned char *byte_array, size_t size, unsigned int buffer_index);
}

void decompose_unsigned_integer(unsigned char *array_idx, unsigned int integer);
void decompose_unsigned_short(unsigned char *array_idx, unsigned short s);
