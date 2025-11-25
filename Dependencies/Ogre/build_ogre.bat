@echo off
setlocal

set BUILD_TYPE=%1
set ROOT_DIR=%2
set SOURCE_DIR=%ROOT_DIR%\src
set BUILD_DIR_DEBUG=%ROOT_DIR%\build\Debug
set BUILD_DIR_RELEASE=%ROOT_DIR%\build\Release
set EXECUTABLE_DIR_DEBUG=%ROOT_DIR%\..\..\build\x64\Debug
set EXECUTABLE_DIR_RELEASE=%ROOT_DIR%\..\..\build\x64\Release
set PLUGINS_DIR_DEBUG=%EXECUTABLE_DIR_DEBUG%\bin\Ogre\plugins
set PLUGINS_DIR_RELEASE=%EXECUTABLE_DIR_RELEASE%\bin\Ogre\plugins
set RESOURCES_DIR=%ROOT_DIR%\..\..\Assets


:: Verificar que el directorio fuente existe
if not exist "%SOURCE_DIR%" (
    echo ERROR: El directorio fuente "%SOURCE_DIR%" no existe.
    exit /b 1
)

:: Crear la carpeta de build si no existe
if not exist "%BUILD_DIR_DEBUG%" mkdir "%BUILD_DIR_DEBUG%"

:: Crear la carpeta de build si no existe
if not exist "%BUILD_DIR_RELEASE%" mkdir "%BUILD_DIR_RELEASE%"

:: Crear las carpetas de ejecutables
if not exist "%EXECUTABLE_DIR_DEBUG%" mkdir "%EXECUTABLE_DIR_DEBUG%"

:: Crear las carpetas de ejecutables
if not exist "%EXECUTABLE_DIR_RELEASE%" mkdir "%EXECUTABLE_DIR_RELEASE%"

:: Crear la carpeta de plugins si no existe
if not exist "%PLUGINS_DIR_DEBUG%" mkdir "%PLUGINS_DIR_DEBUG%"

:: Crear la carpeta de resources si no existe
if not exist "%RESOURCES_DIR%" mkdir "%RESOURCES_DIR%"

:: Crear la carpeta de plugins si no existe
if not exist "%PLUGINS_DIR_RELEASE%" mkdir "%PLUGINS_DIR_RELEASE%"



if "%BUILD_TYPE%" == "Release" ( 
    cmake "%SOURCE_DIR%" -B "%BUILD_DIR_RELEASE%" --preset=Release-x64

    if %errorlevel% neq 0 (
        echo ERROR: Fallo la configuracion con CMake.
        exit /b 1
    )

    echo Configuración con cmake completada

    cmake --build "%BUILD_DIR_RELEASE%" --config Release --target install

    if %errorlevel% neq 0 ( 
    echo ERROR: Fallo la compilacion en release.
    exit /b 1
    )

    echo Compilacion en release correcta.

    echo Moviendo DLLs a %EXECUTABLE_DIR_RELEASE% y %PLUGINS_DIR_RELEASE%
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\OgreMain.dll" "%EXECUTABLE_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\OgreRTShaderSystem.dll" "%EXECUTABLE_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\zlib.dll" "%EXECUTABLE_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\OgreOverlay.dll" "%EXECUTABLE_DIR_RELEASE%"

    ::xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\resources.cfg" "%RESOURCES_DIR%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\plugins.cfg" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\RenderSystem_GL.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\Plugin_OctreeZone.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\Plugin_OctreeSceneManager.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\Plugin_PCZSceneManager.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\Plugin_ParticleFX.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\Codec_STBI.dll" "%PLUGINS_DIR_RELEASE%"
    xcopy /Y /s "%BUILD_DIR_RELEASE%\bin\Release\SDL2.dll" "%EXECUTABLE_DIR_RELEASE%"

    echo Build en release completada. Ogre ha sido instalado en: %BUILD_DIR%
    
    exit /b 0
) else if "%BUILD_TYPE%" == "Debug" (
cmake "%SOURCE_DIR%" -B "%BUILD_DIR_DEBUG%" --preset=Debug-x64


if %errorlevel% neq 0 (
    echo ERROR: Fallo la configuracion con CMake.
    exit /b 1
)

echo Configuración con cmake completada

cmake --build "%BUILD_DIR_DEBUG%" --config Debug --target install

if %errorlevel% neq 0 ( 
 echo ERROR: Fallo la compilacion en debug.
 exit /b 1
)

echo Compilacion en debug correcta.


echo Moviendo DLLs a %EXECUTABLE_DIR_DEBUG% y %PLUGINS_DIR_DEBUG%
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\OgreMain_d.dll" "%EXECUTABLE_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\OgreRTShaderSystem_d.dll" "%EXECUTABLE_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\OgreOverlay_d.dll" "%EXECUTABLE_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\zlibd.dll" "%EXECUTABLE_DIR_DEBUG%"

::xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\resources.cfg" "%RESOURCES_DIR%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\plugins.cfg" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\RenderSystem_GL_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\Plugin_OctreeZone_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\Plugin_OctreeSceneManager_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\Plugin_PCZSceneManager_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\Plugin_ParticleFX_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\Codec_STBI_d.dll" "%PLUGINS_DIR_DEBUG%"
xcopy /Y /s "%BUILD_DIR_DEBUG%\bin\Debug\SDL2d.dll" "%EXECUTABLE_DIR_DEBUG%"

echo Build en debug completada. Ogre ha sido instalado en: %BUILD_DIR%

exit /b 0
) 
else (
echo Build type not exit. It has to be Debug or Release.  )