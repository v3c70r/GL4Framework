#!/bin/bash
CC="g++"
LIBS="-lGL -lGLU -lglfw3 -lXrandr -lX11 -lXcursor -lXxf86vm -lpthread -lXinerama -lXi -lGLEW -lassimp -lIL -lILU -lILUT -lpython2.7"
INC=""
FLAGS="-g -std=c++11 "

$CC $FLAGS -o test *.cpp imgui/*.cpp $INC $LIBS

