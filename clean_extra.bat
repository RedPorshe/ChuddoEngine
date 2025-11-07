@echo off
chcp 65001 >nul
echo ================================
echo    Clean Extra Files (Both Versions)
echo ================================

cd /d "D:\Projects\ChuddoEngine"

echo Cleaning Debug version...
if exist "build\bin\Debug" (
    pushd "build\bin\Debug"
    del libglfw3.a 2>nul
    del libglm.a 2>nul
    del libgcc_s_seh-1.dll 2>nul
    del libstdc++-6.dll 2>nul
    del libwinpthread-1.dll 2>nul
    popd
    echo Debug cleaned
) else (
    echo Debug folder not found
)

echo.
echo Cleaning Release version...
if exist "build\bin\Release" (
    pushd "build\bin\Release"
    del libglfw3.a 2>nul
    del libglm.a 2>nul
    del libgcc_s_seh-1.dll 2>nul
    del libstdc++-6.dll 2>nul
    del libwinpthread-1.dll 2>nul
    popd
    echo Release cleaned
) else (
    echo Release folder not found
)

echo.
echo Final file counts:
if exist "build\bin\Debug" (
    echo Debug: 
    dir "build\bin\Debug" /b | find /c /v ""
) else (
    echo Debug: folder not found
)

if exist "build\bin\Release" (
    echo Release:
    dir "build\bin\Release" /b | find /c /v ""
) else (
    echo Release: folder not found
)

pause