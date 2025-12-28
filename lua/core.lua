local core = {}

-- local function dump_stack()
-- 	print("---- STACK DUMP ----")
-- 	for i = 1, 20 do
-- 		local info = debug.getinfo(i, "Sln")
-- 		if not info then
-- 			break
-- 		end
-- 		print(
-- 			string.format(
-- 				"%2d | source=%s | what=%s | name=%s | line=%s",
-- 				i,
-- 				tostring(info.source),
-- 				tostring(info.what),
-- 				tostring(info.name),
-- 				tostring(info.currentline)
-- 			)
-- 		)
-- 	end
-- 	print("--------------------")
-- end

function core.current_file()
	local info = debug.getinfo(2, "S")
	if info and info.source and info.source:sub(1, 1) == "@" then
		return info.source:sub(2)
	end
	return nil
end

function core.current_dir(level)
	level = (level or 1) + 1
	local info = debug.getinfo(level, "S")
	if not info or not info.source then
		return nil
	end
	local src = info.source
	if src:sub(1, 1) ~= "@" then
		return nil
	end
	return src:sub(2):match("(.*/)")
end

function core.file(path)
	local dir = core.current_dir(2)
	if not dir then
		return nil
	end
	if dir:sub(-1) ~= "/" then
		dir = dir .. "/"
	end
	return fs.normalize(dir .. path)
end

function core.files(paths)
	local dir = core.current_dir(2)
	if not dir then
		return nil
	end
	if dir:sub(-1) ~= "/" then
		dir = dir .. "/"
	end

	local res = {}
	for i, v in ipairs(paths) do
		res[i] = fs.normalize(dir .. v)
	end

	return res
end

function core.source(path, vars)
	vars = (vars or {})
	local dir = core.current_dir(2)
	if not dir then
		return nil
	end
	if dir:sub(-1) ~= "/" then
		dir = dir .. "/"
	end

	local src = fs.normalize(dir .. path)
	local obj = fs.normalize(dir .. path .. ".o")
	local rule = mbuild.get_rule(fs.extension(src))
	if not rule then
		error("No rule to build " .. src)
	end
	mbuild.build(rule, { src }, { obj }, vars)

	return obj
end

function core.sources(srcs)
	local dir = core.current_dir(2)
	if not dir then
		return nil
	end
	if dir:sub(-1) ~= "/" then
		dir = dir .. "/"
	end

	local objs = {}

	for i, v in ipairs(srcs) do
		local path
		local vars = {}

		if type(v) == "string" then
			path = v
		elseif type(v) == "table" then
			path = v[1]
			vars = v[2]
		else
			error("sources(): entries must be string or {string, table}", 2)
		end

		local src = fs.normalize(dir .. path)
		local obj = fs.normalize(dir .. path .. ".o")

		local rule = mbuild.get_rule(fs.extension(src))
		if not rule then
			error("No rule to build " .. src, 2)
		end

		mbuild.build(rule, { src }, { obj }, vars)
		objs[i] = obj
	end

	return objs
end

return core
