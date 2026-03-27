AR = ar
CC = clang
CPP = clang++
ARDUINO_LIBRARIES ?= $(HOME)/Arduino
#INCLUDES = -I. -I../src -I$(ARDUINO_LIBRARIES)/libraries/kitchensink-mbedtls/src
INCLUDES = -I. -I../src -I$(ARDUINO_LIBRARIES)/libraries/kitchensink-tf-psa-crypto/src
C_FLAGS = -g -Wall $(INCLUDES)
CPP_FLAGS = -g -std=gnu++14 -Wall -fsanitize=address -DKS_PASSWORD_SUFFIX=\"_kitchenSink\" $(INCLUDES)
LD_FLAGS = -fsanitize=address

BUILDDIR = build

#C_FILES = $(wildcard $(ARDUINO_LIBRARIES)/libraries/kitchensink-mbedtls/src/*.c)
C_FILES = $(wildcard $(ARDUINO_LIBRARIES)/libraries/kitchensink-tf-psa-crypto/src/*.c)
CPP_FILES = $(wildcard */*.cpp) $(wildcard ../src/types/*.cpp) $(wildcard ../src/crypto/*.cpp) \
            $(wildcard ../src/data/*.cpp) \
            $(wildcard ../src/event/*.cpp) \
            $(wildcard ../src/serialize/*.cpp) \
            $(wildcard ../src/log/*.cpp) \
            $(wildcard ../src/hardware/*.cpp) \
            $(wildcard ../src/timer/*.cpp)
O_FILES = $(patsubst %.c,$(BUILDDIR)/%.o,$(C_FILES)) \
          $(patsubst %.cpp,$(BUILDDIR)/%.o,$(CPP_FILES))
TARGET = tests
GTEST_DIR = /usr/src/gtest
GTEST_SRCS = $(GTEST_DIR)/src/*.cc

all: $(TARGET)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

$(BUILDDIR)/gtest-all.o: $(GTEST_SRCS)
	@mkdir -p $(BUILDDIR)
	$(CPP) $(CPP_FLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

$(BUILDDIR)/gtest_main.o: $(GTEST_SRCS)
	@mkdir -p $(BUILDDIR)
	$(CPP) $(CPP_FLAGS) -I$(GTEST_DIR) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

gtest.a: $(BUILDDIR)/gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a: $(BUILDDIR)/gtest-all.o $(BUILDDIR)/gtest_main.o
	$(AR) $(ARFLAGS) $@ $^

$(TARGET): $(O_FILES) gtest_main.a
	$(CPP) -o $(TARGET) $(LD_FLAGS) $(O_FILES) gtest_main.a -lpthread

clean:
	rm -f $(TARGET) gtest.a gtest_main.a
	rm -rf $(BUILDDIR)
