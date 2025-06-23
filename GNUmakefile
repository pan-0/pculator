.SUFFIXES:
CC       ::= gcc
BUILD    ::= debug
BUILDDIR ::= build/$(BUILD)
OBJDIR   ::= $(BUILDDIR)

debug.cflags  ::= -ggdb -fno-omit-frame-pointer
debug.ldflags ::=

release.cflags ::= \
	-g0 \
	-O3 \
	-flto=auto \
	-fcf-protection=none \
	-fno-plt \
	-fno-ident \
	-fno-asynchronous-unwind-tables \
	-fdata-sections \
	-ffunction-sections \
	-mmanual-endbr
release.ldflags ::= \
	-s \
	-Wl,--gc-sections \
	-Wl,-O1 \
	-Wl,--build-id=none \
	-Wl,--as-needed \
	-Wl,--strip-all \
	-Wl,--strip-debug

CFLAGS ::= \
	-D_FILE_OFFSET_BITS=64 \
	$($(BUILD).cflags) \
	-Wall \
	-Wextra \
	-std=gnu11 \
	-Wno-parentheses \
	$(shell sdl2-config --cflags)

LDFLAGS ::= $($(BUILD).ldflags) -lm -lpcap $(shell sdl2-config --libs)

SRC ::= $(shell find PCulator/ -type f -name *.c) $(wildcard libudis86/*.c)
OBJ ::= $(SRC:%.c=$(OBJDIR)/%.o)

all: $(OBJDIR)/pculator
.PHONY: all

$(OBJDIR):
	mkdir -p $@ && cd $@ && dirname $(SRC) | sort -u | xargs mkdir -p

$(OBJDIR)/pculator: $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

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