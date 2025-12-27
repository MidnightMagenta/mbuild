---@overload fun(rule: string, ins: string[], outs: string[])
---@overload fun(rule: string, ins: string[], outs: string[], vars: string[])
---@param rule string
---@param ins string[]
---@param outs string[]
---@param vars string[]?
function build(rule, ins, outs, vars) end

---@param name string
---@param properties table<string, string>
function rule(name, properties) end
