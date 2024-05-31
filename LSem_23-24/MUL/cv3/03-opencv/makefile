include ../env.mk

CXXFLAGS += -D__STDC_CONSTANT_MACROS
CXXFLAGS += $(shell pkg-config --cflags opencv4)
LDLIBS += $(shell pkg-config --libs opencv4)

BIN = opencv

.PHONY: all
all: $(BIN)

.PHONY: clean
clean:
	$(RM) $(BIN) *.o
