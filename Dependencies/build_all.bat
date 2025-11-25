@echo off
setlocal enabledelayedexpansion

REM Solicitar modo de compilación
set BUILD_TYPE=%1

:: Validar BUILD_TYPE (debe ser Debug, Release o All)
if /i "%BUILD_TYPE%"=="Debug" (
    call :CheckDepRoots
    call :CompileDependenciesParallel Debug
    exit /b 0
) 

if /i "%BUILD_TYPE%"=="Release" (
    call :CheckDepRoots
    call :CompileDependenciesParallel Release
    exit /b 0
)

if /i "%BUILD_TYPE%"=="All" (
    echo Compilando dependencias en modo Debug y Release...
    call :CheckDepRoots

    REM Compilar Debug en paralelo y esperar a que terminen todas
    call :CompileDependenciesParallel Debug
    echo Compilación en modo Debug completada.

    REM Compilar Release en paralelo y esperar a que terminen todas
    call :CompileDependenciesParallel Release
    echo Compilación en modo Release completada.

    exit /b 0
)

:: Si no coincide con ninguna opción válida
echo ERROR: build config doesn't match with Debug, Release or All.
exit /b 1

:: Verificar existencia de directorios
:CheckDepRoots
    set PHYSX_ROOT=%CD%\PhysX
    if not exist "%PHYSX_ROOT%" (
        echo ERROR: El directorio de PhysX "%PHYSX_ROOT%" no existe.
        exit /b 1
    )
    set FMOD_ROOT=%CD%\Fmod
    if not exist "%FMOD_ROOT%" (
        echo ERROR: El directorio de Fmod "%FMOD_ROOT%" no existe.
        exit /b 1
    )
    set LUA_ROOT=%CD%\Lua
    if not exist "%LUA_ROOT%" (
        echo ERROR: El directorio de Lua "%LUA_ROOT%" no existe.
        exit /b 1
    )
    set OGRE_ROOT=%CD%\Ogre
    if not exist "%OGRE_ROOT%" (
        echo ERROR: El directorio de Ogre "%OGRE_ROOT%" no existe.
        exit /b 1
    )
    set IMGUI_ROOT=%CD%\Imgui
    if not exist "%IMGUI_ROOT%" (
        echo ERROR: El directorio de Imgui "%IMGUI_ROOT%" no existe.
        exit /b 1
    )
    exit /b 0

:: Llamada a .bat de cada dependencia en paralelo y redirigir salida estándar a los archivos de log
:CompileDependenciesParallel
    set BUILD_TYPE=%1
    echo Iniciando compilación de dependencias en modo "%BUILD_TYPE%"...

    REM Crear directorio de logs si no existe
    if not exist "%CD%\BuildLogs\%BUILD_TYPE%" (
        mkdir "%CD%\BuildLogs\%BUILD_TYPE%"
    )

    REM Ejecutar cada proceso en paralelo con redirección de salida
    echo Compilando PhysX...
    call "%PHYSX_ROOT%\build_physx.bat" %BUILD_TYPE% "%PHYSX_ROOT%" > "%CD%\BuildLogs\%BUILD_TYPE%\build_physx.log" 2>&1
    echo Compilando Ogre...
    call "%OGRE_ROOT%\build_ogre.bat" %BUILD_TYPE% "%OGRE_ROOT%" > "%CD%\BuildLogs\%BUILD_TYPE%\build_ogre.log" 2>&1
    echo Compilando FMod...
    call "%FMOD_ROOT%\build_fmod.bat" %BUILD_TYPE% "%FMOD_ROOT%" > "%CD%\BuildLogs\%BUILD_TYPE%\build_fmod.log" 2>&1
    echo Compilando Lua...
    call "%LUA_ROOT%\build_lua.bat" %BUILD_TYPE% "%LUA_ROOT%" > "%CD%\BuildLogs\%BUILD_TYPE%\build_lua.log" 2>&1
    echo Compilando ImGui...
    call "%IMGUI_ROOT%\build_imgui.bat" %BUILD_TYPE% "%IMGUI_ROOT%" > "%CD%\BuildLogs\%BUILD_TYPE%\build_imgui.log" 2>&1

    exit /b 0
    