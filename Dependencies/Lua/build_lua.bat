@echo off
setlocal  enabledelayedexpansion
:: Modo de compilacion
set BUILD_TYPE=%1

set ROOT_DIR=%2

set PROJECT_FILE=%ROOT_DIR%\src\Lua.sln
set SOURCE_DIR=%ROOT_DIR%\src
set OUTPUT_DIR_LIB=%ROOT_DIR%\%BUILD_TYPE%\libs
set SOURCE_LIB=%SOURCE_DIR%\lib

:: Verificar que el archivo de la solución existe
if not exist "%PROJECT_FILE%" (
    echo ERROR: No se encontró la solución de Visual Studio "%PROJECT_FILE%".
    exit /b 1
)

:: Crear carpetas de salida si no existen
if not exist "%OUTPUT_DIR_LIB%" mkdir "%OUTPUT_DIR_LIB%" 
if not exist "%OUTPUT_DIR_DLL%" mkdir "%OUTPUT_DIR_DLL%"

:: Compilar Debug
echo Compilando Lua en Debug...
msbuild "%PROJECT_FILE%" /p:Configuration=%BUILD_TYPE% /p:Platform=x64 /t:Build

:: Verificar si la compilación Debug falló
if %errorlevel% neq 0 (
    echo ERROR: Fallo la compilación en "%BUILD_TYPE%".
    exit /b 1
)

echo Compilación en "%BUILD_TYPE%" de lua completada.

:: Copiar archivos Debug
xcopy /y "%SOURCE_LIB%\x64\*.lib" "%OUTPUT_DIR_LIB%\" >nul

rmdir /s /q "%SOURCE_LIB%"

exit /b 0