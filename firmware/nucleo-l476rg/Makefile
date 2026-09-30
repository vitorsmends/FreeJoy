.DEFAULT_GOAL := all
TINYUSB_DIR ?= $(CURDIR)/.deps/tinyusb
EXAMPLE = $(TINYUSB_DIR)/examples/device/nucleo_mock_joystick
BOARD = nucleo_l476rg_mock
.PHONY: all deps prepare flash test

deps:
	python3 prepare.py --deps --tinyusb "$(TINYUSB_DIR)"

prepare:
	python3 prepare.py --tinyusb "$(TINYUSB_DIR)"

all: prepare
	$(MAKE) -C "$(EXAMPLE)" BOARD=$(BOARD) TOOLCHAIN=gcc all
	mkdir -p build
	cp "$(EXAMPLE)/_build/$(BOARD)/nucleo_mock_joystick.bin" build/joystick.bin
	cp "$(EXAMPLE)/_build/$(BOARD)/nucleo_mock_joystick.elf" build/joystick.elf
	cp "$(EXAMPLE)/_build/$(BOARD)/nucleo_mock_joystick.hex" build/joystick.hex

flash: all
	openocd -f interface/stlink.cfg -f target/stm32l4x.cfg -c "program build/joystick.elf verify reset exit"

test: prepare
	mkdir -p build
	cc -std=c99 -Wall -Wextra -Werror -I../../application/Inc -Isrc ../../tests/nucleo_report_test.c -o build/report-test
	./build/report-test
	cc -std=c99 -Wall -Wextra -Werror -DCFG_TUSB_MCU=OPT_MCU_NONE -DTUP_DCD_ENDPOINT_MAX=8 -I../../application/Inc -Isrc -I"$(TINYUSB_DIR)/src" -I"$(TINYUSB_DIR)/hw" ../../tests/nucleo_descriptors_test.c src/usb_descriptors.c -o build/descriptors-test
	./build/descriptors-test
