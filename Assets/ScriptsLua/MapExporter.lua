-- Función que actualiza la tabla 'entities' en Scene.lua.
-- Si la entidad ya existe, devuelve true; si no, la agrega y devuelve false.
local function updateEntities(sceneFilePath, newEntity)
    local file = io.open(sceneFilePath, "r")
    if not file then
        error("No se pudo abrir Scene.lua para lectura: " .. sceneFilePath)
    end
    local content = file:read("*a")
    file:close()

    -- Buscar la línea donde se define la tabla 'entities'
    local entitiesLine = content:match("entities%s*=%s*%b{}")
    if not entitiesLine then
        error("No se encontró la definición de 'entities' en " .. sceneFilePath)
    end

    -- Extraer los nombres existentes
    local list = {}
    local inside = entitiesLine:match("{(.*)}")
    if inside then
        for entity in inside:gmatch('"(.-)"') do
            table.insert(list, entity)
        end
    end

    -- Verificar si ya existe newEntity
    for _, e in ipairs(list) do
        if e == newEntity then
            return true  -- Ya existe
        end
    end

    -- Si no existe, agregarla y reconstruir la línea de entities
    table.insert(list, newEntity)
    local newEntitiesLine = 'entities = { "' .. table.concat(list, '", "') .. '" }'
    local newContent = content:gsub("entities%s*=%s*%b{}", newEntitiesLine)

    file = io.open(sceneFilePath, "w")
    if not file then
        error("No se pudo abrir Scene.lua para escritura: " .. sceneFilePath)
    end
    file:write(newContent)
    file:close()

    return false
end

-- Función principal para cargar el mapa y exportarlo a Scene.lua
function loadMap(mapFilePath, sceneFilePath)
    local mapData = dofile(mapFilePath)
    -- Si no se retornó nada, usamos la variable global 'main'
    if not mapData then
        mapData = { main = main }
    end

    if not mapData or not mapData.main then
        error("El archivo del mapa no tiene la estructura esperada. Se espera una tabla con la clave 'main'.")
    end

    -- Extraer el nombre del archivo sin la ruta y sin extensión (este será el nombre de la entidad padre)
    local mapName = mapFilePath:match("([^/\\]+)%.lua$") or "MapRoot"

    -- Si la entidad padre ya existe en la tabla 'entities', no se vuelve a exportar nada
    if updateEntities(sceneFilePath, mapName) then
        print("La entidad padre '" .. mapName .. "' ya existe en la tabla entities. No se exportan las entidades hijas.")
        return
    end

    -- Abrir Scene.lua en modo append para agregar la definición de entidades
    local scene = io.open(sceneFilePath, "a")
    if not scene then
        error("Could not open scene file for writing: " .. sceneFilePath)
    end

    -- Exportar la entidad padre
    scene:write("\n")
    scene:write(mapName .. " = {\n")
    scene:write("\tTransform = {position = {x = 0, y = 0, z = 0}},\n")
    scene:write("}\n\n")

    -- Recorrer cada colección del mapa
    for collectionName, objects in pairs(mapData.main) do
        local collectionEntity = collectionName .. "_Collection"
        -- Actualizar la tabla 'entities' para la entidad hija de la colección
        updateEntities(sceneFilePath, collectionEntity)
        scene:write(collectionEntity .. " = {\n")
        scene:write("\tParent = \"" .. mapName .. "\",\n")
        
        scene:write("\tTransform = {position = {x = 0, y = 0, z = 0}},\n")
        scene:write("}\n\n")

        -- Para cada objeto de la colección
        for objectName, objectData in pairs(objects) do
            -- Actualizar la tabla 'entities' para la entidad hija del objeto
            updateEntities(sceneFilePath, objectName)
            scene:write(objectName .. " = {\n")
            scene:write("\tParent = \"" .. collectionEntity .. "\",\n") 
            if collectionEntity == "Suelo_Collection" then
                scene:write("\tIsFloor = true,\n")
            end
            scene:write("\tTransform = {\n")
            if objectData.position then
                scene:write(string.format("\t\tposition = {x = %f, y = %f, z = %f},\n", objectData.position.x, objectData.position.z, -objectData.position.y))
            end
            if objectData.rotation then
                scene:write(string.format("\t\trotation = {x = %f, y = %f, z = %f},\n", objectData.rotation.x, objectData.rotation.z, objectData.rotation.y))
            end
            if objectData.scale then
                scene:write(string.format("\t\tscale = {x = %f, y = %f, z = %f},\n", objectData.scale.x, objectData.scale.z, objectData.scale.y))
            end
            scene:write("\t},\n")
            if objectData.model then
                scene:write(string.format("\tRenderMesh = {mesh = \"%s\"},\n", objectData.model))
            end
            scene:write("\tRigidBody = {\n")
            scene:write("\t\tmass = 0.0,\n")
            scene:write("\t\trbStatic = true,\n")
            scene:write("\t\trbKinematic = false,\n")
            scene:write("\t},\n")
            scene:write("\tBoxCollider = {\n")
            if objectData.dimensions then
                scene:write(string.format("\t\thalfExtents = {x = %f, y = %f, z = %f},\n", objectData.dimensions.x * 0.5,objectData.dimensions.z * 0.5,objectData.dimensions.y * 0.5 ))
                scene:write(string.format("\t\tpositionOffset = {x = 0, y = %f, z = 0},\n", objectData.dimensions.z * 0.5))
                scene:write(string.format("\t\ttrigger = false,\n"))
                scene:write(string.format("\t\tactive = true,\n"))
                scene:write(string.format("\t\tdraw = false,\n"))
               
            end
            scene:write("\t}\n")
            scene:write("}\n\n")
        end
    end

    scene:close()
end
