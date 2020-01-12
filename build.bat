@echo off

cl /Zi /EHsc /std:c++latest /DEBUG:FULL /Fedemo.exe main.cpp user32.lib gdi32.lib opengl32.lib winmm.lib
