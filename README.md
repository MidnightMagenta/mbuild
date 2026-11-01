# mbuild

Simple tool for generating ninja scripts. 

Made to build the [nyx](https://github.com/MidnightMagenta/nyx) kernel.

## Brief

Mbuild is a simple Lua based Ninja generator, which assumes nothing, and removes (at least for me) several headaches associated with using GNU Make to build large, multi file projects.

## Status

This tool is in **VERY EARLY** alpha, the API is subject to change, and stability might be questionable. Expect bugs.

## Examples

Examples are available in the `examples` directory.

### Hello World compilation example

```
local mbl = require("mbuildlib")

mb.var("compiler", "gcc")
mb.rule("cc", {
	command = "$compiler -c $in -o $out",
	description = "CC $in -> $out",
})
mb.rule("ld", {
	command = "$compiler -o $out $in",
	description = "LD $in -> $out",
})

local sources = mbl.files({
	"hello_world.c",
})

local objs = mbl.compile("cc", sources)
local target = mb.build({
	rule = "ld",
	out = mbl.bpath("hello_world"),
	inputs = objs,
})

mb.default(target)
```

Assuming a `hello_world.c` exists, running these commands in your command line

```
mkdir build && mbuild -C build ..
ninja -C build
```

will compile a build/hello_world program.

## Building

This project has no external dependencies except the compiler and Make. (and obviously Ninja)

Building steps:

```
git clone --recurse-submodules https://github.com/MidnightMagenta/mbuild.git
cd mbuild
make
```

Optionally adding `DEBUG=y OPT=0` will compile an unoptimized build with debug symbols.

## Usage

`mbuild [-C dir] [-v N] <path-to-root-Mbuild>`

|flag|description|
|-|-|
|-C --cdir|Change current working directory. This is the directory to which the build.ninja file will be emitted|
|-V --verbosity| 1 to 4, verbosity of the output messages. 1 = errors only, 4 = everything|

## API documentation

Coming soon(TM) (for now meta/mb.meta.lua and llib/mbuildlib.lua contain annotated functions/function declarations)
