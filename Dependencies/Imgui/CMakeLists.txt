cmake_minimum_required(VERSION 3.19)
 project(imgui) 

# Archivos principales de ImGui 
set(IMGUI_SOURCES 
src/imgui.cpp
src/imgui_draw.cpp
src/imgui_tables.cpp
src/imgui_widgets.cpp
src/imgui_stdlib.cpp
 )

 # Backends requeridos 
set(IMGUI_BACKENDS
src/imgui_impl_sdl2.cpp
src/imgui_impl_glfw.cpp
src/imgui_impl_opengl2.cpp 
) 

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS})

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")

# Buscar dependencias necesarias
find_package(OpenGL REQUIRED) 
#set(SDL2_DIR "C:/Users/Pedro/Desktop/IMGUI/vcpkg/vcpkg/installed/x64-windows/share/sdl2")
set(SDL2_INCLUDE_DIR "src/include/SDL2")
set(GLFW_INCLUDE_DIR "src/include")

# Incluir directorios 
target_include_directories(imgui PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/src  
${SDL2_INCLUDE_DIR}
${GLFW_INCLUDE_DIR} ) 

#add_executable(ImGuiDemo imgui/main.cpp)

# Vincular librerías necesarias 
target_link_libraries(imgui PUBLIC
OpenGL::GL 
)
