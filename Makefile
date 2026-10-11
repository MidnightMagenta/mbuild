CC := gcc
AR := ar

CFLAGS := -O0 -g -Iinclude -Iinclude/lib/lua
ARFLAGS := rcs

LUA_OBJS := lib/lua/lapi.o \
			lib/lua/lauxlib.o \
			lib/lua/lbaselib.o \
			lib/lua/lcode.o \
			lib/lua/lcorolib.o \
			lib/lua/lctype.o \
			lib/lua/ldblib.o \
			lib/lua/ldebug.o \
			lib/lua/ldo.o \
			lib/lua/ldump.o \
			lib/lua/lfunc.o \
			lib/lua/lgc.o \
			lib/lua/linit.o \
			lib/lua/liolib.o \
			lib/lua/llex.o \
			lib/lua/lmathlib.o \
			lib/lua/lmem.o \
			lib/lua/loadlib.o \
			lib/lua/lobject.o \
			lib/lua/lopcodes.o \
			lib/lua/loslib.o \
			lib/lua/lparser.o \
			lib/lua/lstate.o \
			lib/lua/lstring.o \
			lib/lua/lstrlib.o \
			lib/lua/ltable.o \
			lib/lua/ltablib.o \
			lib/lua/ltm.o \
			lib/lua/lua.o \
			lib/lua/luac.o \
			lib/lua/lundump.o \
			lib/lua/lutf8lib.o \
			lib/lua/lvm.o \
			lib/lua/lzio.o

LIB_OBJS := lib/path.o $(LUA_OBJS)
OBJS := src/main.o src/logging.o src/context.o

LIB_TARGET := lib/libmbuild.a
TARGET := mbuild

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS) $(LIB_TARGET)
	@echo "CC $@"
	@$(CC) -o $@ $^

$(LIB_TARGET): $(LIB_OBJS)
	@echo "AR $@"
	@$(AR) $(ARFLAGS) $@ $^

%.o: %.c
	@echo "CC $@"
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	find . -type f -name '*.[oad]' -delete
	rm -f mbuild compile_commands.json
