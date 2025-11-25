@echo off
setlocal enabledelayedexpansion

:: Modo de compilacion
set BUILD_TYPE=%1
set ROOT_DIR=%2


:: Configurar rutas basadas en tu estructura de carpetas

set SOURCE_DIR=%ROOT_DIR%

pushd %CD%\..
set EXECUTABLE_DIR=%CD%\build\x64\%BUILD_TYPE%
popd
set GENERATOR="Visual Studio 17 2022"  :: Cambia según tu versión de VS

:: Verificar que el directorio fuente existe
if not exist "%SOURCE_DIR%" (
    echo ERROR: El directorio fuente "%SOURCE_DIR%" no existe.
    exit /b 1
)

if not exist "%EXECUTABLE_DIR%" (
    mkdir "%EXECUTABLE_DIR%"
)

echo Moviendo DLLs a %EXECUTABLE_DIR%
copy /Y "%ROOT_DIR%\api\core\lib\64\fmod.dll" "%EXECUTABLE_DIR%"
copy /Y "%ROOT_DIR%\api\core\lib\64\fmodL.dll" "%EXECUTABLE_DIR%"
copy /Y "%ROOT_DIR%\api\core\lib\64\vcruntime140_app.dll" "%EXECUTABLE_DIR%"

echo.
echo Build completado. Fmod ha sido instalado en: %SOURCE_DIR%

exit /b 0