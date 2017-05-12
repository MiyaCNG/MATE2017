CC = avr-gcc
CXX = avr-g++

CFLAGS = -c -g -Os -w -std=gnu++11 -fno-exceptions -ffunction-sections\
 -fdata-sections -fno-threadsafe-statics  -w -x c++ -E -CC,
 -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10800 \
 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR
 
CXXFLAGS = -c -g -Os -w -std=gnu11 -ffunction-sections -fdata-sections \
 -MMD -mmcu=atmega2560 -DF_CPU=16000000L -DARDUINO=10800 \
 -DARDUINO_AVR_MEGA2560 -DARDUINO_ARCH_AVR 

ARDUINO_DIR = -I~/.arduino15/packages/arduino/hardware/avr/1.6.6/cores/arduino

BUILD_PATH = ./build
CORE_BUILD_PATH = $(BUILD_PATH)/core

INCLUDES = $(ARDUINO_DIR)

control_board_atmega: control_board_atmega.o messenger.o debug.o

control_board_atmega.o: control_board_atmega.cpp
	$(CC) $(CFLAGS) control_board_atmega.cpp -o control_board_atmega.o

messenger.o: ../common/messenger.cpp:
	$(CC) $(CFLAGS) $(INCLUDES) ../common/messenger.cpp -o messenger.o

debug.o: ../common/debug.cpp:
	$(CC) $(CFLAGS) $(INCLUDES) ../common/debug.cpp -o debug.o

core.a: $(CORE_BUILD_PATH)/WInterrupts.c.o $(CORE_BUILD_PATH)/hooks.c.o\
$(CORE_BUILD_PATH)/wiring.c.o $(CORE_BUILD_PATH)/wiring_analog.c.o

$(CORE_BUILD_PATH)/%.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) "$(ARDUINO_DIR)/%.c" -o "$(CORE_BUILD_PATH)%.c.o"

$(CORE_BUILD_PATH)/%.cpp.o:
	avr-gcc $(CFLAGS) $(INCLUDES) "$(ARDUINO_DIR)/%.cpp" -o "$(CORE_BUILD_PATH)%.cpp.o"
