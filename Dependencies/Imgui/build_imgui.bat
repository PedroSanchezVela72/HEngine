@echo off
setlocal

:: Parámetros: BUILD_TYPE (por ejemplo, Debug o Release)
set BUILD_TYPE=%1
set ROOT_DIR=%2

:: Compilación de ImGui
set IMGUI_SOURCE_DIR=%ROOT_DIR%
echo :: DIRECTORIO DESTINO IMGUI -- "%ROOT_DIR%"
set BUILD_DIR=%ROOT_DIR%\build
set EXECUTABLE_DIR=%ROOT_DIR%\build\x64
set GENERATOR="Visual Studio 17 2022"

if not exist "%IMGUI_SOURCE_DIR%" (
    echo ERROR: No existe la carpeta de ImGui en "%IMGUI_SOURCE_DIR%".
    exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

cmake -G %GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% "%IMGUI_SOURCE_DIR%" || (
    echo ERROR: Fallo la configuracion con CMake para ImGui.
    exit /b 1
)

cmake --build . --config %BUILD_TYPE% || (
    echo ERROR: Fallo la compilacion con CMake para ImGui.
    exit /b 1
)

echo Moviendo DLLs a %EXECUTABLE_DIR%\%BUILD_TYPE%
if not exist "%EXECUTABLE_DIR%\%BUILD_TYPE%" mkdir "%EXECUTABLE_DIR%\%BUILD_TYPE%"

if exist "%BUILD_DIR%\dlls\" (
    copy /Y "%BUILD_DIR%\dlls\*.dll" "%EXECUTABLE_DIR%\%BUILD_TYPE%"
)

echo.
echo ===========================================================================
echo Build completado. ImGui ha sido instalado en: %EXECUTABLE_DIR%\%BUILD_TYPE%
echo ===========================================================================

:: Compilación de OpenGL
echo.
echo :: INICIANDO COMPILACION DE OPENGL...
set OPENGL_SOURCE_DIR=%ROOT_DIR%\opengl\glfw-3.4
set OPENGL_BUILD_DIR=%ROOT_DIR%\build\opengl

if not exist "%OPENGL_SOURCE_DIR%" (
    echo ERROR: No existe la carpeta de OpenGL en "%OPENGL_SOURCE_DIR%".
    exit /b 1
)

if not exist "%OPENGL_BUILD_DIR%" mkdir "%OPENGL_BUILD_DIR%"
cd /d "%OPENGL_BUILD_DIR%"

cmake -G %GENERATOR% -DCMAKE_BUILD_TYPE=%BUILD_TYPE% "%OPENGL_SOURCE_DIR%" || (
    echo ERROR: Fallo la configuracion con CMake para OpenGL.
    exit /b 1
)

cmake --build . --config %BUILD_TYPE% || (
    echo ERROR: Fallo la compilacion con CMake para OpenGL.
    exit /b 1
)

echo Moviendo DLLs a %EXECUTABLE_DIR%\%BUILD_TYPE%
if exist "%OPENGL_BUILD_DIR%\dlls\" (
    copy /Y "%OPENGL_BUILD_DIR%\dlls\*.dll" "%EXECUTABLE_DIR%\%BUILD_TYPE%"
)

echo.
echo ===========================================================================
echo Build completado. OpenGL ha sido instalado en: %EXECUTABLE_DIR%\%BUILD_TYPE%
echo ===========================================================================

exit /b 0