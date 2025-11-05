@echo off
chcp 65001 >nul
echo ================================
echo    Checking Build Output
echo ================================

cd /d "D:\Projects\ChudEng\build\bin\Debug"

echo Files in build\bin\Debug:
dir

echo.
echo File sizes:
for %%f in (*.*) do (
    echo %%~nf.%%~xf - %%~zf bytes
)

pause