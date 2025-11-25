@echo off
setlocal enabledelayedexpansion

:: Modo de compilación
set BUILD_TYPE=%1
:: Carpeta de Physx
set PHYSX_ROOT_H=%2


REM Define main paths
set PHYSX_ROOT=%PHYSX_ROOT_H%\src\physx
set COMPILER_PATH=%PHYSX_ROOT%\compiler
set INSTALL_PATH=%PHYSX_ROOT%\install\bin
set BUILD_INSTALL_PATH=%INSTALL_PATH%\%BUILD_TYPE%
set DEST_BUILD=%PHYSX_ROOT_H%\%BUILD_TYPE%
set DEST_LIBS=%DEST_BUILD%\libs
set DEST_BIN=%PHYSX_ROOT_H%\..\..\build\x64\%BUILD_TYPE%
set BIN_PATH=%PHYSX_ROOT%\bin
set EXTERNS_DLLS=%BIN_PATH%\%BUILD_TYPE%\externs

echo Running build generation script...
cd %PHYSX_ROOT%

:: Debuggin paths
@REM echo Existing paths: 
@REM echo PHYSX_ROOT: %PHYSX_ROOT%
@REM echo COMPILER_PATH: %COMPILER_PATH%
@REM echo INSTALL_PATH: %INSTALL_PATH%
@REM echo BUILD_INSTALL_PATH: %BUILD_INSTALL_PATH%
@REM echo DEST_BUILD: %DEST_BUILD%
@REM echo DEST_BIN: %DEST_BIN%
@REM echo DEST_LIBS: %DEST_LIBS%
@REM echo EXTERNS_DLLS: %EXTERNS_DLLS%
@REM echo BIN_PATH: %BIN_PATH%

if "%BUILD_TYPE%" == "Debug" (
    echo 2 | call generate_projects.bat
) else if "%BUILD_TYPE%" == "Release" (
    echo 3 | call generate_projects.bat
)

timeout /t 5 >nul

REM Buscar la carpeta de compilación dentro de compiler/
set BUILD_FOLDER=
for /d %%D in (%COMPILER_PATH%\*) do (
    set BUILD_FOLDER=%%D
)

if not defined BUILD_FOLDER (
    echo ERROR: No build folder found in %COMPILER_PATH%
    exit /b 1
)

echo Build folder detected: %BUILD_FOLDER%

if not exist "%BUILD_FOLDER%\CMakeCache.txt" (
    echo ERROR: CMakeCache.txt not found in %BUILD_FOLDER%.
    exit /b 1
)

echo Compiling and installing PhysX in %BUILD_TYPE% mode...
cd /d %BUILD_FOLDER%

MSBuild INSTALL.vcxproj /p:Configuration=%BUILD_TYPE%

REM Crear carpetas para libs y bin si no existen
if not exist "%DEST_LIBS%" mkdir "%DEST_LIBS%"
if not exist "%DEST_BIN%" mkdir "%DEST_BIN%"

REM Copiar archivos .lib y .dll según el modo de compilación
if exist "%BUILD_INSTALL_PATH%" (
    echo Copying .lib files to %DEST_LIBS%...
    xcopy /y "%BUILD_INSTALL_PATH%\*.lib" "%DEST_LIBS%\" >nul
    echo Copying .pdb files to %DEST_LIBS%...
    xcopy /y "%BUILD_INSTALL_PATH%\*.pdb" "%DEST_LIBS%\" >nul
    echo Copying .dll files to %DEST_BIN%...
    xcopy /y "%BUILD_INSTALL_PATH%\*.dll" "%DEST_BIN%\" >nul
) else (
    echo ERROR: %BUILD_INSTALL_PATH% does not exist.
    exit /b 1
)

REM Copiar archivos DLL externos
if exist "%EXTERNS_DLLS%" (
    echo Copying external .dll files to %DEST_BIN%...
    xcopy /y "%EXTERNS_DLLS%\*.dll" "%DEST_BIN%\" >nul
) else (
    echo Warning: %EXTERNS_DLLS% does not exist.
)

REM Eliminar la carpeta physx/install
if exist "%INSTALL_PATH%" (
    echo Deleting %INSTALL_PATH%...
    rmdir /s /q "%INSTALL_PATH%\.."
) else (
    echo Warning: %INSTALL_PATH% does not exist.
)
REM Eliminar la carpeta physx/install
if exist "%BIN_PATH%" (
    echo Deleting %BIN_PATH%...
    rmdir /s /q "%BIN_PATH%"
) else (
    echo Warning: %BIN_PATH% does not exist.
)


if exist "%BUILD_FOLDER%" (
    rmdir /s /q "%BUILD_FOLDER%"
)
echo Process completed successfully.
exit /b 0