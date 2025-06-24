.SUFFIXES:
CC       ::= gcc
BUILD    ::= debug
BUILDDIR ::= build/$(BUILD)
OBJDIR   ::= $(BUILDDIR)

debug.cflags  ::= -ggdb -fno-omit-frame-pointer
debug.ldflags ::=

release.cflags ::= \
	-g0 \
	-O2 \
	-flto=auto \
	-fcf-protection=none \
	-fno-plt \
	-fno-ident \
	-fno-asynchronous-unwind-tables \
	-fdata-sections \
	-ffunction-sections
release.ldflags ::= \
	-s \
	-Wl,--gc-sections \
	-Wl,-O1 \
	-Wl,--build-id=none \
	-Wl,--as-needed \
	-Wl,--strip-all \
	-Wl,--strip-debug

sanitize.cflags ::= \
	-ggdb \
	-O2 \
	-flto=auto \
	-fno-plt \
	-fno-ident \
	-fno-asynchronous-unwind-tables \
	-fdata-sections \
	-ffunction-sections \
	-fno-omit-frame-pointer \
	-fsanitize-recover=address \
	-fsanitize=address \
	-fsanitize=leak \
	-fsanitize=undefined \
	-fsanitize=pointer-compare \
	-fsanitize=pointer-subtract \
	-fsanitize=pointer-overflow \
	-fsanitize=float-divide-by-zero \
	-fsanitize=signed-integer-overflow
sanitize.ldflags ::= \
	-Wl,--gc-sections \
	-Wl,-O1 \
	-Wl,--build-id=none \
	-Wl,--as-needed

CFLAGS ::= \
	-D_FILE_OFFSET_BITS=64 \
	$($(BUILD).cflags) \
	-Wall \
	-Wextra \
	-std=gnu11 \
	-Wno-parentheses \
	$(shell sdl2-config --cflags)

LDFLAGS ::= $($(BUILD).ldflags)
LDLIBS  ::= -lm -lpcap $(shell sdl2-config --libs)

SRC ::= $(shell find PCulator/ -type f -name *.c) $(wildcard libudis86/*.c)
OBJ ::= $(SRC:%.c=$(OBJDIR)/%.o)

all: $(OBJDIR)/pculator
.PHONY: all

$(OBJDIR):
	mkdir -p $@ && cd $@ && dirname $(SRC) | sort -u | xargs mkdir -p

$(OBJDIR)/pculator: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

C = \
	$(info CC	$(notdir $@)) \
	@$(CC) $(CFLAGS) -c $< -o $@
include deps.mk

clean:
	@rm -rf $(BUILDDIR)
.PHONY: clean

cleanall:
	@rm -rf build/
.PHONY: cleanall