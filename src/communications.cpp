#include <definitions.h>
#include <communications.h>


/// 
/// Functions to invoke network connection.
///
namespace LocalNetwork {

    ///
    /// Initialises a network connection, given the global SSID and PASSWORD
    /// from the definitions file.
    ///
    void wifi_connect() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(NW_SSID, NW_PASSWORD);

        // attempt to connect to wifi, with timeout
        Serial.print("[debug] Attempting to connect to WiFi...");
        unsigned char attempts = 0;
        while(WiFi.status() != WL_CONNECTED) {
            Serial.print(".");
            attempts += 1;

            // if attempted to connect for more than 30 (60*0.5) seconds...
            if(attempts > 60) {
                Serial.println("\n[err] Error connecting to the network. Are you sure the SSID and password are correct?");
                esp32_exit();
            }

            delay(500);
        }

        Serial.print("\n[debug] WiFi connected established with local IP: ");
        Serial.println(WiFi.localIP());
    }
}

/// 
/// Functions related to the TcpServer and generation of packet bytes.
///
namespace TcpServer {

    /// 
    /// Starts a WifiServer on a given port, with port safety checks.
    /// 
    /// # Parameters:
    /// - `port`: The port to start hte server on
    ///
    /// # Returns:
    /// - The server initialised by the function
    ///
    WiFiServer generate_server(int port) {
        if(port <= 1000 || port >= 65536) {
            Serial.println("[err] Error starting WiFiServer. Port " + String(port) + " is out of range 1001-65535.");
            esp32_exit();
        }

        WiFiServer server(port);
        return server;
    }

    ///
    /// Generates the greeting response packet for the client.
    /// This includes: firmware protocol, instruction buffer size, max motor speed, minimum pulse width
    /// 
    /// # Parameters:
    /// - `byte_array`: The byte array to populate
    /// - `size`: The size of the byte array, must be greater than 18 (atm)
    /// - `total_ins_completed`: The initial instruction index, currently always 0
    ///
    void gen_header_bytes(unsigned char *byte_array, size_t size, unsigned int total_ins_completed) {
        const unsigned char bytes_needed = 18;

        if(size < bytes_needed) {
            Serial.println("[err] Error generating header bytes. The buffer for populating the header is too small.");
            esp32_exit();
        }

        byte_array[0] = 0x01; // valid request
        decompose_unsigned_short(&byte_array[1], PROTOCOL_VERSION); // protocol version
        decompose_unsigned_integer(&byte_array[3], total_ins_completed); // existing instruction index
        decompose_unsigned_integer(&byte_array[7], INS_BUFFER_SIZE); // instruction buffer size
        decompose_unsigned_integer(&byte_array[11], MAX_MOTOR_SPEED); // max motor speed
        decompose_unsigned_integer(&byte_array[15], MIN_PULSE_WIDTH); // min pulse width
    }

    ///
    /// Generates the pause response packet for the client.
    /// This includes: whether the firmware is paused (0x01) or not (0x00), total instructions completed
    /// 
    /// # Parameters:
    /// - `byte_array`: The byte array to populate
    /// - `size`: The size of the byte array, must be greater than 5 (atm)
    /// - `is_paused`: Whether the firmware is now paused
    /// - `total_ins_completed`: The number of instructions completed
    ///
    void gen_pause_feedback_bytes(unsigned char *byte_array, size_t size, bool is_paused, unsigned int total_ins_completed) {
        const unsigned char bytes_needed = 5;

        if(size < bytes_needed) {
            Serial.println("[err] Error generating feedback bytes. The buffer for populating the feedback is too small.");
            esp32_exit();
        }

        byte_array[0] = 0x04;
        byte_array[1] = is_paused ? 0x01 : 0x00;
        decompose_unsigned_integer(&byte_array[2], total_ins_completed);
    }

    ///
    /// Generates the feedback packet for the client.
    /// This includes: total instructions completed
    /// 
    /// # Parameters:
    /// - `byte_array`: The byte array to populate
    /// - `size`: The size of the byte array, must be greater than 5 (atm)
    /// - `buffer_index`: The number of instructions completed
    ///
    void gen_feedback_bytes(unsigned char *byte_array, size_t size, unsigned int buffer_index) {
        const unsigned char bytes_needed = 5;

        if(size < bytes_needed) {
            Serial.println("[err] Error generating feedback bytes. The buffer for populating the feedback is too small.");
            esp32_exit();
        }

        byte_array[0] = 0x02;
        decompose_unsigned_integer(&byte_array[1], buffer_index);
    }
}


/// 
/// Decomposes an unsigned integer into an array, at a given index. (big-endian)
///
/// # Parameters:
/// - `array_idx`: Pointer to the initial byte to set
/// - `integer`: The unsigned integer to decompose
///
void decompose_unsigned_integer(unsigned char *array_idx, unsigned int integer) {
    array_idx[0] = static_cast<uint8_t>((integer >> 24) & 0xFF);
    array_idx[1] = static_cast<uint8_t>((integer >> 16) & 0xFF);
    array_idx[2] = static_cast<uint8_t>((integer >> 8) & 0xFF);
    array_idx[3] = static_cast<uint8_t>((integer) & 0xFF);
}

/// 
/// Decomposes an unsigned short into an array, at a given index. (big-endian)
///
/// # Parameters:
/// - `array_idx`: Pointer to the initial byte to set
/// - `s`: The unsigned short to decompose
///
void decompose_unsigned_short(unsigned char *array_idx, unsigned short s) {
    array_idx[0] = static_cast<uint8_t>((s >> 8) & 0xFF);
    array_idx[1] = static_cast<uint8_t>((s) & 0xFF);
}
