@echo off
if not exist build mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
if %errorlevel% neq 0 goto :error
cmake --build . --config Debug
if %errorlevel% neq 0 goto :error
echo Build completed successfully!
goto :end

:error
echo Build failed!
pause

:end
