@echo off
chcp 65001 >nul
echo ================================
echo    Complete Clean
echo ================================

cd /d "D:\Projects\ChudEng"

echo Removing all build files...
if exist "build" (
    rmdir /s /q "build"
    echo Build directory removed
)

if exist "CMakeCache.txt" (
    del "CMakeCache.txt"
    echo CMakeCache.txt removed
)

if exist "CMakeFiles" (
    rmdir /s /q "CMakeFiles"
    echo CMakeFiles directory removed
)

echo.
echo Clean complete! Now run debug.bat or release.bat
pause