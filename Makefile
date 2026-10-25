CC := gcc
AR := ar

OPT ?= 0
DEBUG ?= y
CFLAGS := -O$(OPT) -Iinclude -Ilua -Imlib/include
ARFLAGS := rcs
LDFLAGS := -lm

ifeq ($(DEBUG), y)
    CFLAGS += -g -fsanitize=address,undefined -fno-omit-frame-pointer
    LDFLAGS += -fsanitize=address,undefined
endif

OBJS := src/main.o src/logging.o src/context.o src/read_script.o

LUA_TARGET := lua/liblua.a 
MLIB_TARGET := mlib/libm.a
TARGET := mbuild

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(LUA_TARGET) $(MLIB_TARGET)
	@echo "CC $@"
	@$(CC) $(LDFLAGS) -o $@ $^

$(LUA_TARGET):
	@$(MAKE) -C lua MYCFLAGS="-DLUA_USE_POSIX" -j

$(MLIB_TARGET):
	@make -C mlib -j

%.o: %.c
	@echo "CC $@"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	find . -type f -name '*.[oad]' -delete
	rm -f mbuild compile_commands.json
