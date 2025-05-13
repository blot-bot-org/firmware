# Blot Bot Firmware

This is the firmware for the Blot Bot drawing machine. It is written in C++ due to the libraries used, however most of the code I wrote followed C code constructs.


## Installation / Prerequisites 📥
You must install the PlatformIO core. The [install page](https://platformio.org/install) suggests installation via VSCode. Alternatively, you could install it on your system, via [the documentation](https://docs.platformio.org/en/latest/core/installation/index.html).<br>
Next, clone and enter this project's repository, and run `make build`. If all goes well, you will see (somewhere) the message: *'Successfully created esp32 image.'*


## Configuration ⚙️
You can edit Blot-Bot's parameters in the [definitions.h](https://github.com/blot-bot-org/firmware/blob/master/src/definitions.h) file.

You must set the following parameters:
- **L_PULSE**, **L_DIRECTION**, **L_ENABLE** - the pulse, direction, and enable GPIO pin for the left motor.
- **R_PULSE**, **R_DIRECTION**, **R_ENABLE** - the pulse, direction, and enable GPIO pin for the right motor.<br>
([ESP32 GPIO specifications](https://www.upesy.com/blogs/tutorials/esp32-pinout-reference-gpio-pins-ultimate-guide))
- **NW_SSID** - the SSID of the network the ESP32 will connect to.
- **NW_PASSWORD** - the password of the network the ESP32 will connect to.

You can optionally edit:
- **SOCKET_PORT** - the port the socket server will be made on. *(default 8180)*
- **MAX_MOTOR_SPEED** - the maximum steps/second the motors will move. *(default 500)*
- **MIN_PULSE_WIDTH** - the minimum step pulse width, in μs. (should be configured accordingly to your stepper motors) *(default 300)*
- **INS_BUFFER_SIZE** - can be adjusted depending on the RAM of your specific ESP32 model. *(default 32768)*

*Note: From experience, anything under 8192 bytes can cause network instability. Anything over 32768 bytes can take more than a second to send, which results in ink-blots on the page, depending on the pen.*

## Flashing ⚡
To flash the firmware to your ESP32, connect it to your computer and run `make upload`
To monitor its serial ouput, run `make monitor`

To upload and then monitor, you can run `make upload_monitor`


## That's it 👍
Having flashed the firmware, and monitored to make sure it joins the network successfully, the firmware is good to go! It now awaits a client to tell it what to draw.


## Licensing ⚖️
This firmware is licensed under the GPL v3 license, as per [AccelStepper's licensing](https://github.com/waspinator/AccelStepper/blob/master/LICENSE).
