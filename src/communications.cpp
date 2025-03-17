/*
 * TCP Server implementation
 * @Mqlvin
 * 06/03/2025
*/

#include <definitions.h>
#include <communications.h>


namespace LocalNetwork {
    void wifi_connect() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(NW_SSID, NW_PASSWORD);

        Serial.print("Attempting to connect to WiFi...");
        unsigned char attempts = 0;
        while(WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            attempts += 1;

            // If attempted to connect for more than 30 seconds...
            if(attempts > 60) {
                Serial.println("\nError connecting to the network. Are you sure the SSID and password are correct?");
                esp32_exit();
            }

            delay(500);
        }
        Serial.print("\nWiFi connected established with local IP: ");
        Serial.println(WiFi.localIP());
    }
}


namespace TcpServer {
    WiFiServer generate_server(int port) {
        if(port <= 1000 || port >= 65536) {
            Serial.println("Error starting WiFiServer. Port " + String(port) + " is out of range 1001-65535.");
            esp32_exit();
        }

        WiFiServer server(port);
        return server;
    }

    void gen_header_bytes(unsigned char *byte_array, size_t size) {
        const unsigned char bytes_needed = 17;

        if(size < bytes_needed) {
            Serial.println("Error generating header bytes. The buffer for populating the header is too small.");
            esp32_exit();
        }

        byte_array[0] = 0x01; // valid request
        decompose_unsigned_short(&byte_array[1], PROTOCOL_VERSION); // 1 -> 2 with protocol version
        decompose_unsigned_integer(&byte_array[3], INS_BUFFER_SIZE); // 3 -> 6 with instruction buffer size
        decompose_unsigned_integer(&byte_array[7], MAX_MOTOR_SPEED); // 7 -> 10 with max motor speed
        decompose_unsigned_integer(&byte_array[11], MIN_PULSE_WIDTH); // 11 -> 13 with min pulse width
    }

    void gen_feedback_bytes(unsigned char *byte_array, size_t size, unsigned int buffer_index) {
        const unsigned char bytes_needed = 5;

        if(size < bytes_needed) {
            Serial.println("Error generating feedback bytes. The buffer for populating the feedback is too small.");
            esp32_exit();
        }

        byte_array[0] = 0x02;
        decompose_unsigned_integer(&byte_array[1], buffer_index);
    }
}


// takes an index, fills the next 4 bytes with the decomposed integer bytes
void decompose_unsigned_integer(unsigned char *array_idx, unsigned int integer) {
    array_idx[0] = static_cast<uint8_t>((integer >> 24) & 0xFF);
    array_idx[1] = static_cast<uint8_t>((integer >> 16) & 0xFF);
    array_idx[2] = static_cast<uint8_t>((integer >> 8) & 0xFF);
    array_idx[3] = static_cast<uint8_t>((integer) & 0xFF);
}

void decompose_unsigned_short(unsigned char *array_idx, unsigned short s) {
    array_idx[0] = static_cast<uint8_t>((s >> 8) & 0xFF);
    array_idx[1] = static_cast<uint8_t>((s) & 0xFF);
}
