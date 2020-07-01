@echo off

echo BUILDING...

g++ -std=c++11 -Wall -O3 -march=native -o launch.exe ^
src/main.cpp ^
-I "C:\Programs\C++\Libraries\glfw.3.3.2.bin.WIN64\include" ^
-I "C:\Programs\C++\Libraries\glm.0.9.9.8" ^
-I "C:\Programs\C++\Libraries\glew.2.1.0\include" ^
-L "C:\Programs\C++\Libraries\glfw.3.3.2.bin.WIN64\lib-mingw-w64" ^
-L "C:\Programs\C++\Libraries\glew.2.1.0\lib\Release\x64" ^
-lglfw3 -lgdi32 -lopengl32 -lglew32

if %errorlevel% == 0 (
   echo LAUNCHING...
   copy >NUL "C:\Programs\C++\Libraries\glew.2.1.0\bin\Release\x64\glew32.dll"
   launch.exe
   echo CLOSING...
) else ( pause )
