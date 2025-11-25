cmake_minimum_required(VERSION 3.10)
 project(imgui) 

# Archivos principales de ImGui 
set(IMGUI_SOURCES 
imgui/imgui.cpp
imgui/imgui_draw.cpp
imgui/imgui_tables.cpp
imgui/imgui_widgets.cpp
imgui/misc/cpp/imgui_stdlib.cpp
 )

 # Backends requeridos 
set(IMGUI_BACKENDS
imgui/backends/imgui_impl_sdl2.cpp
imgui/backends/imgui_impl_glfw.cpp
imgui/backends/imgui_impl_opengl2.cpp 
imgui/backends/imgui_impl_dx11.cpp 
) 

add_library(imgui STATIC ${IMGUI_SOURCES} ${IMGUI_BACKENDS})

set(CMAKE_TOOLCHAIN_FILE "C:/Users/Pedro/Desktop/IMGUI/vcpkg/vcpkg/scripts/buildsystems/vcpkg.cmake" CHACHE STRING "")

# Buscar dependencias necesarias
find_package(OpenGL REQUIRED) 
set(SDL2_DIR "C:/Users/Pedro/Desktop/IMGUI/vcpkg/vcpkg/installed/x64-windows/share/sdl2")
find_package(SDL2 REQUIRED) 

# Incluir directorios 
target_include_directories(imgui PUBLIC 
${CMAKE_CURRENT_SOURCE_DIR}/imgui 
${CMAKE_CURRENT_SOURCE_DIR}/imgui/backends 
${SDL2_INCLUDE_DIRS} ) 

#add_executable(ImGuiDemo imgui/main.cpp)

# Vincular librerías necesarias 
target_link_libraries(imgui PUBLIC
OpenGL::GL 
${SDL2_LIBRARIES}
d3d11 
)
