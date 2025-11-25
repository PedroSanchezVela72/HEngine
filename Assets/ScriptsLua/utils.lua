
function serializeTable(t, indent)
  indent = indent or ""
  local s = "{\n"
  for k, v in pairs(t) do
      local key = tostring(k)
      local value
      if type(v) == "table" then
          value = serializeTable(v, indent .. "  ")
      elseif type(v) == "string" then
          value = "\"" .. v .. "\""
      elseif type(v) == "boolean" then
          value = tostring(v)
      else
          value = tostring(v)
      end
      s = s .. indent .. "  " .. key .. " = " .. value .. ",\n"
  end
  s = s .. indent .. "}"
  return s
end


function saveEntityToFile(name, scene)
  local entity = _G[name]
  local file = io.open(scene, "a")
  file:write(name .. " = " .. serializeTable(entity))
  file:write("\n\n")
  file:close()
end
