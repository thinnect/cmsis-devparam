UNITY_ROOT=$(HOME)/workspace/ThrowTheSwitch/Unity

CFLAGS = -std=c99
CFLAGS += -Wall

SRC_FILES  = $(UNITY_ROOT)/src/unity.c test.c
SRC_FILES += test_runners/TestProductionCode_Runner.c
SRC_FILES += ../src/lib/devp.c
SRC_FILES += ../src/lib/devp_comms.c
SRC_FILES += ../src/params/lifetime.c
SRC_FILES += ../src/params/uptime.c

SRC_FILES += zoo/thinnect.mist-comm/api/mist_comm_api.c
SRC_FILES += zoo/thinnect.mist-comm/cmsis/mist_comm_mutex.c

INC_DIRS  = -I. -I$(UNITY_ROOT)/src
INC_DIRS += -Izoo/thinnect.mist-comm/include
INC_DIRS += -Izoo/thinnect.mist-comm/include/compat
INC_DIRS += -Izoo/thinnect.mist-comm/cmsis
INC_DIRS += -Izoo/thinnect.lll/logging
INC_DIRS += -I../include

INC_DIRS += -Izoo/thinnect.cmsis-mock
SRC_FILES += zoo/thinnect.cmsis-mock/cmsis_os2.c

# endianness.h
INC_DIRS += -I$(HOME)/workspace/ls/zoo/jtbr.endianness

TARGET = test.out

all: clean default

default: $(SRC_FILES)
	gcc $(CFLAGS) $(INC_DIRS) $(SYMBOLS) $(SRC_FILES) -o $(TARGET)
	- ./$(TARGET)

test_runners/TestProductionCode_Runner.c: test.c
	mkdir -p test_runners
	ruby $(UNITY_ROOT)/auto/generate_test_runner.rb test.c test_runners/TestProductionCode_Runner.c

clean:
	rm -f $(TARGET)
