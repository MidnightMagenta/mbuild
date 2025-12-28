---@class mbuild
local _mbuild = {}

---@overload fun(rule: string, ins: string[], outs: string[])
---@overload fun(rule: string, ins: string[], outs: string[], vars: string[])
---@param rule string
---@param ins string[]
---@param outs string[]
---@param vars string[]?
function _mbuild.build(rule, ins, outs, vars) end

---@param name string
---@param properties table
function _mbuild.rule(name, properties) end

---@param key string
---@param value string
function _mbuild.set(key, value) end

---@param key string
---@return string
function _mbuild.get(key) end

---@param path string
function _mbuild.build_dir(path) end

---@param extension string
---@return string
function _mbuild.get_rule(extension) end

---@type mbuild
mbuild = _mbuild
