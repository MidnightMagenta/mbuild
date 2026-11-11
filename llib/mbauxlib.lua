local _, filepath = ...
mb.track(filepath)

---@class mba
local mba = {}

---@param a any[]
---@param b any[]
---@return any[]
function mba.merge(a, b)
	local out = table.move(a, 1, #a, 1, {})
	return table.move(b, 1, #b, #out + 1, out)
end

---@param t any[]
---@param out any[]?
---@return any[]
function mba.flatten(t, out)
	out = out or {}
	for _, v in ipairs(t) do
		if type(v) == "table" then
			mba.flatten(v, out)
		else
			out[#out + 1] = v
		end
	end
	return out
end

return mba
