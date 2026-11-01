CC := gcc
AR := ar

OPT ?= 2
DEBUG ?= n
VERSION := $(shell cat VERSION)
CPPFLAGS := -DMB_VERSION='"$(VERSION)"'
CFLAGS := -O$(OPT) $(CPPFLAGS)
INCDIR := -Iinclude -Ilua -Imlib/include
ARFLAGS := rcs
LDFLAGS := -lm

ifeq ($(DEBUG), y)
    CFLAGS += -g -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address,undefined
endif

OBJS := src/main.o src/logging.o src/context.o src/read_script.o \
		src/ninja_types.o src/ninja.o src/version.o

LUA_TARGET := lua/liblua.a
MLIB_TARGET := mlib/libm.a
TARGET := mbuild

.PHONY: all clean FORCE

all: $(TARGET)

$(TARGET): $(OBJS) $(LUA_TARGET) $(MLIB_TARGET)
	@echo "CC $@"
	@$(CC) $(LDFLAGS) -o $@ $^

$(LUA_TARGET): FORCE
	@$(MAKE) -C lua MYCFLAGS="-O$(OPT) -DLUA_USE_POSIX"

$(MLIB_TARGET): FORCE
	@make -C mlib CFLAGS="$(CFLAGS)"

src/version.o: src/version.c VERSION
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(INCDIR) -MMD -MP -c $< -o $@

%.o: %.c
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(INCDIR) -MMD -MP -c $< -o $@

clean:
	find . -type f -name '*.[oad]' -delete
	rm -f mbuild compile_commands.json

FORCE:
 
-include $(OBJS:.o=.d)
