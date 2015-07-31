#!/bin/bash
CC="g++-5"
LIBS="-lGL -lGLU -lglfw3 -lXrandr -lX11 -lXcursor -lXxf86vm -lpthread -lXinerama -lXi -lGLEW -lassimp -lIL -lILU -lILUT"
INC=""

$CC -g -std=c++11 -o test *.cpp imgui/*.cpp $INC $LIBS

