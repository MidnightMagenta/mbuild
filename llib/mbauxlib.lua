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

return mba
