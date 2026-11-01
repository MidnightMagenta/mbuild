local mbl = {}

local function listify(x)
	if type(x) == "string" then
		return { x }
	end
	return x or {}
end

function mbl.files(paths)
	local opaths = {}
	for _, p in ipairs(listify(paths)) do
		opaths[#opaths + 1] = mb.file(p)
	end
	return opaths
end

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
	return cfg
end

return mbl
