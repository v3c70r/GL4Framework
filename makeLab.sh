#!/bin/bash
CC="g++"
LIBS="-L/nfs/home/g/g_qing/usr/lib -L/nfs/home/g/g_qing/usr/lib64 -L/lib64 -ldl -lGL -lGLU -lglfw3 -lXrandr -lX11 -lXcursor -lXxf86vm -lpthread -lXinerama -lXi -lGLEW -lassimp -lpython2.7"
INC="-I/nfs/home/g/g_qing/usr/include/"
FLAGS="-g -std=c++11 "

$CC $FLAGS -o test *.cpp imgui/*.cpp $INC $LIBS

