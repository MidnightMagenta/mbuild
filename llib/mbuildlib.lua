local _, filepath = ...
mb.track(filepath)

---@class mbl
local mbl = {}

local function listify(x)
	if type(x) == "string" then
		return { x }
	end
	return x or {}
end

local function parse_version(s)
	local maj, min, pat = s:match("^(%d+)%.(%d+)%.(%d+)")
	if not maj then
		error("invalid version '" .. s .. "'", 3)
	end
	return { tonumber(maj), tonumber(min), tonumber(pat) }
end

---@param a string
---@param b string
---@return integer
function mbl.vercmp(a, b)
	local x, y = parse_version(a), parse_version(b)
	for i = 1, 3 do
		if x[i] ~= y[i] then
			return x[i] < y[i] and -1 or 1
		end
	end
	return 0
end

---@param paths string|string[]
---@return string[]
function mbl.files(paths)
	local opaths = {}
	for _, p in ipairs(listify(paths)) do
		opaths[#opaths + 1] = mb.file(p)
	end
	return opaths
end

---@class config
---@field data table<string, string>
local config = {}
config.__index = config

---@param data table<string, string>
---@return config
function config.new(data)
	return setmetatable({ data = data or {} }, config)
end

---@generic T
---@param key string
---@param default T?
---@return string|T
function config:get(key, default)
	default = default or "n"
	local v = self.data[key]
	if v == nil then
		return default
	end
	return v
end

---@param key string
---@return boolean
function config:has(key)
	return self.data[key] ~= nil
end

---@param path string
---@return config
function mbl.rdcfg(path)
	local f, err = io.open(path, "r")
	if not f then
		error("rdcfg: " .. err, 2)
	end

	local cfg = {}
	local lineno = 0
	for raw in f:lines() do
		lineno = lineno + 1
		local line = (raw:gsub("#.*$", ""))
		line = line:match("^%s*(.-)%s*$")

		if line ~= "" then
			local k, v = line:match("^([%w_%.%-]+)%s*=%s*(.*)$")
			if not k then
				f:close()
				error(("%s:%d: expected key=value"):format(path, lineno), 2)
			end
			cfg[k] = v
		end
	end
	f:close()

	mb.track(path)
	return config.new(cfg)
end

---@class mbl.CompileOpts
---@field rule string Name of a rule previously delcared with mb.rule
---@field suffix string Suffix added to the end of the output name
---@field implicit_out? string|string[]
---@field implicit? string|string[] Inputs that trigger rebuilds but aren't passed to the command
---@field order_only? string|string[]
---@field vars? table<string, string> Per-edge variable bindings
---@field pool? string

---compilation invoker for multiple sources
---@param rule string
---@param srcs string|string[]
---@param opts mbl.CompileOpts?
---@return string[]
function mbl.compile(rule, srcs, opts)
	opts = opts or {}
	opts.suffix = opts.suffix or ".o"
	local outs = {}

	local builddir = mb.builddir()
	for _, p in ipairs(listify(srcs)) do
		outs[#outs + 1] = mb.build({
			rule = rule,
			out = builddir .. "/" .. p .. opts.suffix,
			implicit_out = opts.implicit_out,
			inputs = p,
			implicit = opts.implicit,
			order_only = opts.order_only,
			vars = opts.vars,
			pool = opts.pool,
		})
	end

	return outs
end

---GCC style include flag generator
---@param dirs string|string[]
---@return string
function mbl.incl(dirs)
	local flags = {}
	for _, d in ipairs(listify(dirs)) do
		flags[#flags + 1] = "-I" .. mb.rel(d)
	end
	return table.concat(flags, " ")
end

---prefix a path with the build directory
---@param p string
---@return string
function mbl.bpath(p)
	return mb.builddir() .. "/" .. p
end

return mbl
