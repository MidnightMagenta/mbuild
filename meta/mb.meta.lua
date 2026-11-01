---@meta

---@class mb
mb = {}

---executes a script in a subdirectory
---@param dir string
---@return table? targets  -- by convention, a script may return a table
function mb.subdir(dir) end

---converts a current script relative path to a file
---or a directory into a project root relative path
---@param file string
---@return string
function mb.file(file) end

---delcare and define a variable emitted verbatim to Ninja
---@param name string
---@param val string
function mb.var(name, val) end

---declare a Ninja pool
---@param name string
---@param depth integer
function mb.pool(name, depth) end

---declare a Ninja default
---@param name string[]
function mb.default(name) end

---@class mb.RuleSpec
---@field command string The command to run (required by Ninja)
---@field description? string Shown instead of the command while building
---@field depfile? string Path to a Makefile-style depfile
---@field deps? "gcc"|"msvc" Dependency format
---@field msvc_deps_prefix? string
---@field generator? boolean Rule regenerates the build file itself
---@field restat? boolean Re-stat outputs after running to skip downstream work
---@field rspfile? string
---@field rspfile_content? string
---@field pool? string

---declare a Ninja rule
---@param name string
---@param spec mb.RuleSpec
---@return string
function mb.rule(name, spec) end

---@class mb.BuildSpec
---@field rule string Name of a rule previously declared with mb.rule
---@field out string|string[] Explicit outputs
---@field implicit_out? string|string[]
---@field inputs? string|string[]
---@field implicit? string|string[] Inputs that trigger rebuilds but aren't passed to the command
---@field order_only? string|string[]
---@field vars? table<string, string> Per-edge variable bindings
---@field pool? string

---delcare a Ninja edge
---@param spec mb.BuildSpec
---@return string[]
function mb.build(spec) end

---get the build directory
---@return string
function mb.builddir() end

---get path relative to build directory
---@param d string
---@return string
function mb.rel(d) end
