@echo off
setlocal

if "%~1"=="" (
    echo Uso: %~nx0 ^<debug^|release^>
    exit /b 1
)

set "CONFIG=%~1"
if /I not "%CONFIG%"=="debug" if /I not "%CONFIG%"=="release" (
    echo Opcion invalida: %CONFIG%
    echo Uso: %~nx0 ^<debug^|release^>
    exit /b 1
)

cd Dependencies

:: Llamar a build_all.bat con la misma configuracion
call build_all.bat %CONFIG%

cd ..

echo Compilando H-Engine en modo %CONFIG%...
msbuild .\H-Engine.sln -p:Configuration=%CONFIG% -noLogo -verbosity:minimal

echo Compilacion de H-Engine (%CONFIG%) completada.
endlocal
