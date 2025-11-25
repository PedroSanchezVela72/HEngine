@echo off
setlocal enabledelayedexpansion

:: Parámetros
set BLENDER_ROOT_H=%2
set RESOURCES_DIR=..\..\Resources
set EXPORT_SCRIPT=%BLENDER_ROOT_H%\exportToLua.py

:: Definir rutas principales
set BLENDER_DIR=%BLENDER_ROOT_H%\tools\BlenderPortable
set BLENDER_EXECUTABLE=%BLENDER_DIR%\blender.exe
set BLENDER_URL=https://ftp.nluug.nl/pub/graphics/blender/release/Blender4.0/blender-4.0.0-windows-x64.zip
set BLENDER_ZIP=%BLENDER_ROOT_H%\tools\blender.zip

:: Verificar e instalar Blender Portable si no existe
if not exist "%BLENDER_EXECUTABLE%" (
    echo Blender not found. Downloading Blender Portable...
    mkdir "%BLENDER_ROOT_H%\tools"

    powershell -Command "& {Invoke-WebRequest -Uri '%BLENDER_URL%' -OutFile '%BLENDER_ZIP%'}"

    if exist "%BLENDER_ZIP%" (
        echo Extracting Blender...
        powershell -Command "& {Expand-Archive -Path '%BLENDER_ZIP%' -DestinationPath '%BLENDER_ROOT_H%\tools' -Force}"
        move "%BLENDER_ROOT_H%\tools\blender-4.0.0-windows-x64" "%BLENDER_DIR%"
        del "%BLENDER_ZIP%"
        echo Blender installed successfully.
    ) else (
        echo ERROR: Blender download failed.
        exit /b 1
    )
) else (
    echo Blender already installed.
)

:: Validar que la carpeta de recursos existe
if not exist "%RESOURCES_DIR%" (
    echo ERROR: La carpeta de recursos no existe.
    exit /b 1
)

:: Procesar todos los archivos .blend en la carpeta de recursos
for %%f in ("%RESOURCES_DIR%\*.blend") do (
    echo Procesando %%f...
    "%BLENDER_EXECUTABLE%" --background "%%f" --python "%EXPORT_SCRIPT%"
    
    if %errorlevel% neq 0 (
        echo ERROR: Fallo al procesar %%f
    ) else (
        echo %%f convertido correctamente a .lua
    )
)

echo Proceso completado.
exit /b 0
