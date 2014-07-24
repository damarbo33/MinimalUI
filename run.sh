#!/bin/sh

#apt-get install libogg0 libogg-dev libvorbis0 libvorbisenc2 libvorbis-dev

mkdir -p obj/unzip
mkdir -p obj/zlib
mkdir -p obj/Debug/src/zlib
mkdir -p obj/Debug/src/unzip
mkdir -p obj/Debug/src/roms
mkdir -p obj/Debug/src/sdl
mkdir -p obj/Debug/src/sql
mkdir -p bin/Debug
make Debug

