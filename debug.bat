@echo off
chcp 65001 >nul
echo ================================
echo    Building DEBUG Version (MinGW)
echo ================================

cd /d "D:\Projects\ChuddoEngine"

:: Проверяем, запущен ли GameEngine.exe и завершаем его
tasklist /FI "IMAGENAME eq GameEngine.exe" 2>NUL | find /I /N "GameEngine.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo GameEngine is running, terminating...
    taskkill /F /IM "GameEngine.exe" >nul 2>&1
    timeout /t 1 /nobreak >nul
)

:: Конфигурируем для Debug с MinGW (кеш сохраняется для скорости)
echo Configuring CMake for Debug (MinGW)...
cmake -B build -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" -DCMAKE_DISABLE_FIND_PACKAGE_PkgConfig=ON -DCMAKE_SKIP_RPATH=ON -DCMAKE_DISABLE_PRECOMPILE_HEADERS=ON -DCMAKE_SKIP_INSTALL_RULES=ON

:: Компилируем проект
echo Building project...
cmake --build build

:: Автоматически очищаем лишние файлы
echo.
echo Cleaning extra files...
if exist "build\bin\Debug" (
    pushd "build\bin\Debug"
    del libglfw3.a 2>nul
    del libglm.a 2>nul
    del libgcc_s_seh-1.dll 2>nul
    del libstdc++-6.dll 2>nul
    del libwinpthread-1.dll 2>nul
    popd
    echo Extra files removed
)

echo.
echo ================================
echo    DEBUG Build Complete!
echo ================================

:: Правильный путь для MinGW
set "EXE_PATH=build\bin\Debug\GameEngine.exe"

if exist "%EXE_PATH%" (
    echo Executable: %EXE_PATH%
    for %%I in ("%EXE_PATH%") do echo File size: %%~zI bytes
    echo.
    set /p "run=Do you want to run GameEngine now? (y/n): "
    if /i "%run%"=="y" (
        echo Starting GameEngine...
        start "" "%EXE_PATH%"
    )
) else (
    echo ERROR: GameEngine.exe not found at: %EXE_PATH%
    echo Current directory: %CD%
    echo Searching for exe files...
    dir "build\bin\Debug" /b 2>nul
)

