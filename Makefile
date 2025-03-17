all: upload_monitor

build:
	platformio run --environment esp32dev

upload:
	platformio run --target upload --environment esp32dev

monitor:
	platformio run --target monitor --environment esp32dev

upload_monitor:
	platformio run --target upload --target monitor --environment esp32dev
