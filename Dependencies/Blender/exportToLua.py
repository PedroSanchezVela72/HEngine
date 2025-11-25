import bpy
import os
import math

def export_to_lua(filepath):
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write("main = {\n")

        # Recorrer todas las colecciones
        for collection in bpy.data.collections:
            f.write(f"    ['{collection.name}'] = {{\n")

            for obj in collection.objects:
                if obj.type == 'MESH':  # Solo exportar objetos de tipo MESH
                    model_name = obj.get("model", obj.data.name + ".mesh")  # Si tiene una propiedad "model", la usa

                    f.write(f"        ['{obj.name}'] = {{\n")
                    f.write(f"            type = '{obj.type}',\n")
                    f.write(f"            model = '{model_name}',\n")
                    f.write(f"            position = {{ x = {obj.location.x}, y = {obj.location.y}, z = {obj.location.z} }},\n")
                    
                     # Obtener rotación en grados desde Euler Angles
                    euler = obj.rotation_euler  # Acceder a los ángulos de Euler
                    rotation_x = math.degrees(euler.x)
                    rotation_y = math.degrees(euler.y)
                    rotation_z = math.degrees(euler.z)
                    
                    f.write(f"            rotation = {{ x = {rotation_x}, y = {rotation_y}, z = {rotation_z} }},\n")
                    f.write(f"            scale = {{ x = {obj.scale.x}, y = {obj.scale.y}, z = {obj.scale.z} }},\n")
                    f.write(f"            dimensions = {{ x = {obj.dimensions.x}, y = {obj.dimensions.y}, z = {obj.dimensions.z} }}\n")
                    f.write("        },\n")

            f.write("    },\n")

        f.write("}\n")

    print(f"Mapa exportado con éxito a {filepath}")

# Obtener la ruta del archivo .blend
ruta_blend = bpy.path.abspath("//")  # Esto devuelve la ruta completa del archivo .blend sin el nombre del archivo
directorio_padre = os.path.dirname(ruta_blend)
directorio_padre2 = os.path.dirname(directorio_padre)

ruta_blend = bpy.data.filepath 
# Extraer el nombre del archivo .blend sin la extensión
nombre_blend = os.path.splitext(os.path.basename(ruta_blend))[0]

# Guardar en el escritorio del usuario
ruta_salida = os.path.join(directorio_padre2, "Assets/ScriptsLua", f"{nombre_blend}.lua")
if not os.path.exists(os.path.dirname(ruta_salida)):
    os.makedirs(os.path.dirname(ruta_salida), exist_ok=True)

export_to_lua(ruta_salida)