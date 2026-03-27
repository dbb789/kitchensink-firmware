AR  = ar
CC  = clang
CPP = clang++

ARDUINO_LIBRARIES ?= $(HOME)/Arduino
INCLUDES = -I../src -I$(ARDUINO_LIBRARIES)/libraries/kitchensink-tf-psa-crypto/src

C_FLAGS   = -g -Wall $(INCLUDES)
CPP_FLAGS = -g -std=gnu++14 -Wall -DKS_PASSWORD_SUFFIX=\"\" $(INCLUDES)

BUILDDIR = build

C_FILES = $(wildcard $(ARDUINO_LIBRARIES)/libraries/kitchensink-tf-psa-crypto/src/*.c)
LIB_CPP_FILES = $(wildcard ../src/types/*.cpp) \
                $(wildcard ../src/crypto/*.cpp) \
                $(wildcard ../src/data/*.cpp) \
                $(wildcard ../src/event/*.cpp) \
                $(wildcard ../src/serialize/*.cpp) \
                $(wildcard ../src/log/*.cpp) \
                $(wildcard ../src/hardware/*.cpp) \
                $(wildcard ../src/timer/*.cpp)

LIB_O_FILES = $(patsubst %.c,$(BUILDDIR)/%.o,$(C_FILES)) \
              $(patsubst %.cpp,$(BUILDDIR)/%.o,$(LIB_CPP_FILES))

TARGETS = aesencrypt aesdecrypt

all: $(TARGETS)

$(BUILDDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(BUILDDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

libkitchensink.a: $(LIB_O_FILES)
	$(AR) $(ARFLAGS) $@ $^

aesencrypt.o: aesencrypt.cpp
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

aesdecrypt.o: aesdecrypt.cpp
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

fileinstream.o: fileinstream.cpp
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

fileoutstream.o: fileoutstream.cpp
	$(CPP) $(CPP_FLAGS) -o $@ -c $<

aesencrypt: aesencrypt.o fileoutstream.o libkitchensink.a
	$(CPP) -o $@ $^

aesdecrypt: aesdecrypt.o fileinstream.o fileoutstream.o libkitchensink.a
	$(CPP) -o $@ $^

clean:
	rm -f $(TARGETS) aesencrypt.o aesdecrypt.o fileinstream.o fileoutstream.o libkitchensink.a
	rm -rf $(BUILDDIR)
