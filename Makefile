# A simple makefile generator by KiSoft, 2010. mailto: kisoft@rambler.ru
# version: 0.3.12.5

# Project Variables start
CPP=g++
CC=gcc
LD=g++
LIB=ar
WINDRES=
# Project Variables end

# Target: Debug
FLAGS=

OBJS_DEBUG=obj/Debug/src/Colorutil.o obj/Debug/src/Constant.o obj/Debug/src/Database.o obj/Debug/src/Dirutil.o obj/Debug/src/Excepcion.o obj/Debug/src/Fileattr.o obj/Debug/src/Fileio.o obj/Debug/src/Icogestor.o obj/Debug/src/ImagenGestor.o obj/Debug/src/Iofrontend.o obj/Debug/src/Ioutil.o obj/Debug/src/Launcher.o obj/Debug/src/Menuobject.o obj/Debug/src/SHA1.o obj/Debug/src/Traza.o obj/Debug/src/joymapper.o obj/Debug/src/main.o obj/Debug/src/roms/estructuras.o obj/Debug/src/roms/gestorroms.o obj/Debug/src/sdl/SDL_draw.o obj/Debug/src/sql/sqlite3.o obj/Debug/src/unzip/ioapi.o obj/Debug/src/unzip/unzip.o obj/Debug/src/unzip/unziptool.o obj/Debug/src/zlib/adler32.o obj/Debug/src/zlib/compress.o obj/Debug/src/zlib/crc32.o obj/Debug/src/zlib/deflate.o obj/Debug/src/zlib/gzio.o obj/Debug/src/zlib/infback.o obj/Debug/src/zlib/inffast.o obj/Debug/src/zlib/inflate.o obj/Debug/src/zlib/inftrees.o obj/Debug/src/zlib/trees.o obj/Debug/src/zlib/uncompr.o obj/Debug/src/zlib/zutil.o

OBJS_CLEAN=obj/Debug/src/Colorutil.o obj/Debug/src/Constant.o obj/Debug/src/Database.o obj/Debug/src/Dirutil.o obj/Debug/src/Excepcion.o obj/Debug/src/Fileattr.o obj/Debug/src/Fileio.o obj/Debug/src/Icogestor.o obj/Debug/src/ImagenGestor.o obj/Debug/src/Iofrontend.o obj/Debug/src/Ioutil.o obj/Debug/src/Launcher.o obj/Debug/src/Menuobject.o obj/Debug/src/SHA1.o obj/Debug/src/Traza.o obj/Debug/src/joymapper.o obj/Debug/src/main.o obj/Debug/src/roms/estructuras.o obj/Debug/src/roms/gestorroms.o

Debug: bin/Debug/MinimalUI

bin/Debug/MinimalUI: $(OBJS_DEBUG)
	@echo Building executable bin/Debug/MinimalUI
	@g++   -o bin/Debug/MinimalUI $(OBJS_DEBUG)  -lSDL -lSDL_gfx -lSDL_ttf -lfreetype -lSDL_image -ljpeg -lpng -lz -lSDL_mixer -lmikmod  

obj/Debug/src/Colorutil.o: src/Colorutil.cpp
	@echo Compiling: "src/Colorutil.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Colorutil.cpp" -o obj/Debug/src/Colorutil.o

obj/Debug/src/Constant.o: src/Constant.cpp
	@echo Compiling: "src/Constant.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Constant.cpp" -o obj/Debug/src/Constant.o

obj/Debug/src/Database.o: src/Database.cpp
	@echo Compiling: "src/Database.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Database.cpp" -o obj/Debug/src/Database.o

obj/Debug/src/Dirutil.o: src/Dirutil.cpp
	@echo Compiling: "src/Dirutil.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Dirutil.cpp" -o obj/Debug/src/Dirutil.o

obj/Debug/src/Excepcion.o: src/Excepcion.cpp
	@echo Compiling: "src/Excepcion.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Excepcion.cpp" -o obj/Debug/src/Excepcion.o

obj/Debug/src/Fileattr.o: src/Fileattr.cpp
	@echo Compiling: "src/Fileattr.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Fileattr.cpp" -o obj/Debug/src/Fileattr.o

obj/Debug/src/Fileio.o: src/Fileio.cpp
	@echo Compiling: "src/Fileio.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Fileio.cpp" -o obj/Debug/src/Fileio.o

obj/Debug/src/Icogestor.o: src/Icogestor.cpp
	@echo Compiling: "src/Icogestor.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Icogestor.cpp" -o obj/Debug/src/Icogestor.o

obj/Debug/src/ImagenGestor.o: src/ImagenGestor.cpp
	@echo Compiling: "src/ImagenGestor.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/ImagenGestor.cpp" -o obj/Debug/src/ImagenGestor.o

obj/Debug/src/Iofrontend.o: src/Iofrontend.cpp
	@echo Compiling: "src/Iofrontend.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Iofrontend.cpp" -o obj/Debug/src/Iofrontend.o

obj/Debug/src/Ioutil.o: src/Ioutil.cpp
	@echo Compiling: "src/Ioutil.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Ioutil.cpp" -o obj/Debug/src/Ioutil.o

obj/Debug/src/Launcher.o: src/Launcher.cpp
	@echo Compiling: "src/Launcher.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Launcher.cpp" -o obj/Debug/src/Launcher.o

obj/Debug/src/Menuobject.o: src/Menuobject.cpp
	@echo Compiling: "src/Menuobject.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Menuobject.cpp" -o obj/Debug/src/Menuobject.o

obj/Debug/src/SHA1.o: src/SHA1.cpp
	@echo Compiling: "src/SHA1.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/SHA1.cpp" -o obj/Debug/src/SHA1.o

obj/Debug/src/Traza.o: src/Traza.cpp
	@echo Compiling: "src/Traza.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/Traza.cpp" -o obj/Debug/src/Traza.o

obj/Debug/src/joymapper.o: src/joymapper.cpp
	@echo Compiling: "src/joymapper.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/joymapper.cpp" -o obj/Debug/src/joymapper.o

obj/Debug/src/main.o: src/main.cpp
	@echo Compiling: "src/main.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/main.cpp" -o obj/Debug/src/main.o

obj/Debug/src/roms/estructuras.o: src/roms/estructuras.cpp
	@echo Compiling: "src/roms/estructuras.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/roms/estructuras.cpp" -o obj/Debug/src/roms/estructuras.o

obj/Debug/src/roms/gestorroms.o: src/roms/gestorroms.cpp
	@echo Compiling: "src/roms/gestorroms.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/roms/gestorroms.cpp" -o obj/Debug/src/roms/gestorroms.o

obj/Debug/src/sdl/SDL_draw.o: src/sdl/SDL_draw.c
	@echo Compiling: "src/sdl/SDL_draw.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/sdl/SDL_draw.c" -o obj/Debug/src/sdl/SDL_draw.o

obj/Debug/src/sql/sqlite3.o: src/sql/sqlite3.c
	@echo Compiling: "src/sql/sqlite3.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/sql/sqlite3.c" -o obj/Debug/src/sql/sqlite3.o

obj/Debug/src/unzip/ioapi.o: src/unzip/ioapi.c
	@echo Compiling: "src/unzip/ioapi.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/unzip/ioapi.c" -o obj/Debug/src/unzip/ioapi.o

obj/Debug/src/unzip/unzip.o: src/unzip/unzip.c
	@echo Compiling: "src/unzip/unzip.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/unzip/unzip.c" -o obj/Debug/src/unzip/unzip.o

obj/Debug/src/unzip/unziptool.o: src/unzip/unziptool.cpp
	@echo Compiling: "src/unzip/unziptool.cpp"
	@g++ -Wall  -DUNIX $(FLAGS)    -c "src/unzip/unziptool.cpp" -o obj/Debug/src/unzip/unziptool.o

obj/Debug/src/zlib/adler32.o: src/zlib/adler32.c
	@echo Compiling: "src/zlib/adler32.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/adler32.c" -o obj/Debug/src/zlib/adler32.o

obj/Debug/src/zlib/compress.o: src/zlib/compress.c
	@echo Compiling: "src/zlib/compress.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/compress.c" -o obj/Debug/src/zlib/compress.o

obj/Debug/src/zlib/crc32.o: src/zlib/crc32.c
	@echo Compiling: "src/zlib/crc32.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/crc32.c" -o obj/Debug/src/zlib/crc32.o

obj/Debug/src/zlib/deflate.o: src/zlib/deflate.c
	@echo Compiling: "src/zlib/deflate.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/deflate.c" -o obj/Debug/src/zlib/deflate.o

obj/Debug/src/zlib/gzio.o: src/zlib/gzio.c
	@echo Compiling: "src/zlib/gzio.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/gzio.c" -o obj/Debug/src/zlib/gzio.o

obj/Debug/src/zlib/infback.o: src/zlib/infback.c
	@echo Compiling: "src/zlib/infback.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/infback.c" -o obj/Debug/src/zlib/infback.o

obj/Debug/src/zlib/inffast.o: src/zlib/inffast.c
	@echo Compiling: "src/zlib/inffast.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/inffast.c" -o obj/Debug/src/zlib/inffast.o

obj/Debug/src/zlib/inflate.o: src/zlib/inflate.c
	@echo Compiling: "src/zlib/inflate.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/inflate.c" -o obj/Debug/src/zlib/inflate.o

obj/Debug/src/zlib/inftrees.o: src/zlib/inftrees.c
	@echo Compiling: "src/zlib/inftrees.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/inftrees.c" -o obj/Debug/src/zlib/inftrees.o

obj/Debug/src/zlib/trees.o: src/zlib/trees.c
	@echo Compiling: "src/zlib/trees.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/trees.c" -o obj/Debug/src/zlib/trees.o

obj/Debug/src/zlib/uncompr.o: src/zlib/uncompr.c
	@echo Compiling: "src/zlib/uncompr.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/uncompr.c" -o obj/Debug/src/zlib/uncompr.o

obj/Debug/src/zlib/zutil.o: src/zlib/zutil.c
	@echo Compiling: "src/zlib/zutil.c"
	@gcc -Wall  -DUNIX $(FLAGS)    -c "src/zlib/zutil.c" -o obj/Debug/src/zlib/zutil.o

"src/main.cpp": main.h Dirutil.h

"src/main.h": Ioutil.h Iofrontend.h

"src/Traza.h": Constant.h

"src/Fileio.cpp": Fileio.h

"src/Fileio.h": Excepcion.h Constant.h Traza.h

"src/Ioutil.cpp": Ioutil.h

"src/Ioutil.h": SDL/SDL.h SDL/SDL_ttf.h SDL/SDL_image.h SDL/SDL_rotozoom.h Fileio.h Constant.h stdlib.h ImagenGestor.h Traza.h Menuobject.h Icogestor.h sdl/SDL_draw.h joymapper.h thread.h roms/gestorroms.h

"src/Traza.cpp": Traza.h

"src/Dirutil.cpp": Dirutil.h

"src/Dirutil.h": Fileattr.h Traza.h ListaSimple.h

"src/SHA1.cpp": SHA1.h

"src/Database.cpp": Database.h

"src/Database.h": sql/sqlite3.h Excepcion.h roms/estructuras.h

"src/sql/sqlite3.c": config.h sqlite3rtree.h

"src/roms/estructuras.cpp": estructuras.h

"src/roms/estructuras.h": ../Fileattr.h ../Traza.h listaIni.h

"src/roms/listaIni.h": ../ListaSimple.h

"src/roms/gestorroms.cpp": gestorroms.h

"src/roms/gestorroms.h": ../Database.h ../ListaSimple.h ../Dirutil.h ../Menuobject.h estructuras.h

"src/sdl/SDL_draw.c": SDL_draw.h Draw_Pixel.c Draw_Pixel.c Draw_Pixel.c Draw_Pixel.c Draw_Line.c Draw_Line.c Draw_Line.c Draw_Line.c Draw_Circle.c Draw_Circle.c Draw_Circle.c Draw_Circle.c Draw_FillCircle.c Draw_FillCircle.c Draw_FillCircle.c Draw_FillCircle.c Draw_HLine.c Draw_HLine.c Draw_HLine.c Draw_HLine.c Draw_VLine.c Draw_VLine.c Draw_VLine.c Draw_VLine.c Draw_Rect.c Draw_Rect.c Draw_Rect.c Draw_Rect.c Draw_Ellipse.c Draw_Ellipse.c Draw_Ellipse.c Draw_Ellipse.c Draw_FillEllipse.c Draw_FillEllipse.c Draw_FillEllipse.c Draw_FillEllipse.c Draw_Round.c Draw_Round.c Draw_Round.c Draw_Round.c Draw_FillRound.c Draw_FillRound.c Draw_FillRound.c Draw_FillRound.c

"src/Launcher.cpp": Launcher.h

"src/Launcher.h": unzip/unziptool.h Dirutil.h

"src/unzip/unziptool.h": ../zlib/zlib.h unzip.h crypt.h ../Fileio.h ../Dirutil.h

"src/zlib/zlib.h": zconf.h

"src/unzip/unzip.h": ../zlib/zlib.h ioapi.h

"src/unzip/ioapi.c": ../zlib/zlib.h ioapi.h

"src/unzip/unzip.c": ../zlib/zlib.h unzip.h crypt.h

"src/unzip/unziptool.cpp": unziptool.h

"src/zlib/adler32.c": zlib.h

"src/zlib/compress.c": zlib.h

"src/zlib/crc32.c": zutil.h crc32.h

"src/zlib/zutil.h": zlib.h

"src/zlib/deflate.c": deflate.h

"src/zlib/deflate.h": zutil.h

"src/zlib/gzio.c": zutil.h

"src/zlib/infback.c": zutil.h inftrees.h inflate.h inffast.h inffixed.h

"src/zlib/inffast.c": zutil.h inftrees.h inflate.h inffast.h

"src/zlib/inflate.c": zutil.h inftrees.h inflate.h inffast.h inffixed.h

"src/zlib/inftrees.c": zutil.h inftrees.h

"src/zlib/trees.c": deflate.h trees.h

"src/zlib/uncompr.c": zlib.h

"src/zlib/zutil.c": zutil.h

"src/joymapper.h": Constant.h ListaSimple.h

"src/joymapper.cpp": joymapper.h

"src/Constant.cpp": Constant.h

"src/thread.h": constant.h gmutex.h

"src/Fileattr.cpp": Fileattr.h

clean: 
	@echo rm -f $(OBJS_CLEAN) bin/Debug/MinimalUI
	-@rm -f $(OBJS_CLEAN) bin/Debug/MinimalUI




