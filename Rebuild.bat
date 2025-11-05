@echo off
chcp 65001 >nul
echo ================================
echo    Quick Rebuild
echo ================================

cd /d "D:\Projects\ChudEng"

:: Проверяем, запущен ли GameEngine.exe и завершаем его
echo Checking for running GameEngine process...
tasklist /FI "IMAGENAME eq GameEngine.exe" 2>NUL | find /I /N "GameEngine.exe">NUL
if "%ERRORLEVEL%"=="0" (
    echo GameEngine is running, terminating...
    taskkill /F /IM "GameEngine.exe" >nul 2>&1
    timeout /t 1 /nobreak >nul
)

:: Определяем текущую конфигурацию
if exist "build\bin\Debug\GameEngine.exe" (
    set "CONFIG=Debug"
    set "EXE_PATH=build\bin\Debug\GameEngine.exe"
) else if exist "build\bin\Release\GameEngine.exe" (
    set "CONFIG=Release" 
    set "EXE_PATH=build\bin\Release\GameEngine.exe"
) else (
    set "CONFIG=Debug"
    set "EXE_PATH=build\bin\Debug\GameEngine.exe"
)

echo Quick rebuilding (%CONFIG%)...
cmake --build build --config %CONFIG%

echo.
echo ================================
echo    Rebuild Complete!
echo ================================
echo Executable: %EXE_PATH%

:: Предлагаем запустить
echo.
set /p "run=Do you want to run GameEngine now? (y/n): "
if /i "%run%"=="y" (
    echo Starting GameEngine...
    start "" "%EXE_PATH%"
)

pause