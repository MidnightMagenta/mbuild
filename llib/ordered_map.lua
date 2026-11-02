---@class OrderedMapState<K, V>
---@field keys K[]
---@field data table<K, V>

---@class OrderedMap<K, V>
---@field [K] V
---@operator len: integer
local OrderedMap = {}

---@type table<OrderedMap, OrderedMapState>
local private = setmetatable({}, { __mode = "k" })

---@generic K, V
---@param initial? [K, V][]
---@return OrderedMap<K, V>
function OrderedMap.new(initial)
	local self = setmetatable({}, OrderedMap)
	private[self] = { keys = {}, data = {} }
	if initial then
		for _, entry in ipairs(initial) do
			self[entry[1]] = entry[2]
		end
	end
	return self
end

---@generic K, V
---@param self OrderedMap<K, V>
---@param k K
---@return V?
function OrderedMap.__index(self, k)
	return private[self].data[k]
end

---@generic K, V
---@param self OrderedMap<K, V>
---@param k K
---@param v V?
function OrderedMap.__newindex(self, k, v)
	local s = private[self]
	local exists = s.data[k] ~= nil

	if v == nil then
		if exists then
			for i, key in ipairs(s.keys) do
				if key == k then
					table.remove(s.keys, i)
					break
				end
			end
			s.data[k] = nil
		end
	else
		if not exists then
			s.keys[#s.keys + 1] = k
		end
		s.data[k] = v
	end
end

---@generic K, V
---@param self OrderedMap<K, V>
---@return fun(): K?, V?
---@return OrderedMap<K, V>
---@return nil
function OrderedMap.__pairs(self)
	local s = private[self]
	local i = 0
	return function()
		i = i + 1
		local k = s.keys[i]
		if k ~= nil then
			return k, s.data[k]
		end
	end, self, nil
end

---@generic K, V
---@param self OrderedMap<K, V>
---@return integer
function OrderedMap.__len(self)
	return #private[self].keys
end

---@generic K, V
---@param self OrderedMap<K, V>
---@return K[]
function OrderedMap.keys(self)
	local copy = {}
	for i, k in ipairs(private[self].keys) do
		copy[i] = k
	end
	return copy
end

return OrderedMap
