---@meta

---@class mb
mb = {}

---executes a script in a subdirectory
---@param dir string
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

---declare a Ninja rule
---@param name string
---@param spec table<string, any>
---@return string
function mb.rule(name, spec) end

---delcare a Ninja edge
---@param spec table<string, any>
---@return string[]
function mb.build(spec) end
