BUILD_DIR = build
CC = avr-gcc
OBJCP = avr-objcopy
CFLAGS = -O3 -Wall -Wextra -Wpedantic -DF_CPU=20000000ULL -Wno-unused-function -Wno-unused-variable -mmcu=attiny1624
PORT = /dev/ttyUSB0
C_SOURCES = src/main.c src/uart.c src/pit.c src/fusb302.c src/usb_pd.c src/printf_uart.c src/i2c.c
C_INCLUDES = -Iinclude
CFLAGS += $(C_INCLUDES)

OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

all: clean build.hex

run: all
	avrdude -F -V -c atmelice_updi -pt1616 -U flash:w:build.hex

clean:
	rm -f build.hex
	rm -f build.elf
clean-all: clean
	rm -f ${BUILD_DIR}/*

$(BUILD_DIR)/%.o: %.c
	${CC} -c $(CFLAGS) $< -o $@

build.hex: $(OBJECTS)
	${CC} ${CFLAGS} -o $(BUILD_DIR)/build.elf $(OBJECTS)
	${OBJCP} -O ihex -R .eeprom $(BUILD_DIR)/build.elf $@
