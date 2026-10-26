CC := gcc
AR := ar

OPT ?= 0
DEBUG ?= y
CFLAGS := -O$(OPT)
INCDIR := -Iinclude -Ilua -Imlib/include
ARFLAGS := rcs
LDFLAGS := -lm

ifeq ($(DEBUG), y)
    CFLAGS += -g -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address,undefined
endif

OBJS := src/main.o src/logging.o src/context.o src/read_script.o \
		src/ninja_types.o src/ninja.o

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

%.o: %.c
	@echo "CC $@"
	@$(CC) $(CFLAGS) $(INCDIR) -MMD -MP -c $< -o $@

clean:
	find . -type f -name '*.[oad]' -delete
	rm -f mbuild compile_commands.json

FORCE:
