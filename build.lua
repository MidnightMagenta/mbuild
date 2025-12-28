mbuild.set("cflags", "-test -flags")
print("LUA's view of cflags: ", mbuild.get("cflags"))
mbuild.rule("cc", {
	cc = "gcc",
	command = "$cc -MD -MF $out.d $cflags -c $in -o $out",
	depfile = "$out.d",
	deps = "gcc",
	description = "CC $out",
	file_extensions = { ".c" },
})

local testfile = "path/to/come/file.c"
print(mbuild.get_rule(fs.extension(testfile)))

local c = require("core")

local files = c.sources({ "test.c", { "foo.c", { cflags = "-test -flags" } }, "bar.c", "baz.c" })
if files then
	for _, v in ipairs(files) do
		print(v)
	end
end

print(c.file("test.c"))

print(c.current_dir())

dofile("build/test.lua")

mbuild.build_dir("build")
