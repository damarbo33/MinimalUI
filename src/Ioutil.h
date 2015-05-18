#ifndef Ioutil_H
#define Ioutil_H

    #include <iostream>
    #include <map>
    #include <cmath>        // std::abs
    #include <sstream>

    #include "SDL/SDL.h"
    #include "SDL/SDL_ttf.h"
    #include "SDL/SDL_image.h"
    #include "SDL/SDL_rotozoom.h"
    #include "sdl/SDL_draw.h"

    #include "Fileio.h"
    #include "Constant.h"
    #include "ImagenGestor.h"
    #include "Traza.h"
    #include "Menuobject.h"
    #include "Icogestor.h"
    #include "joymapper.h"
    #include "thread.h"
    #include "roms/gestorroms.h"

    #include "uiinput.h"
    #include "uiprogressbar.h"
    #include "uicombobox.h"

    struct tInput{
        int cursorX;
        int cursorY;
        string texto;
    };

    class Ioutil : public Fileio{
        public:
            Ioutil();
            virtual ~Ioutil();
            tEvento WaitForKey(); //Espera a que se pulse una tecla
            void initSDL(bool); //Inicia el subsistema de SDL
            void killSDL();
            void toggleFullScreen();

            void drawText(const char* , int , int , t_color ); //Escribe texto en la pantalla
            void drawTextInt(int , int , int ,  t_color ); // Escribe un entero en la pantalla
            void drawTextCent( const char* , int , int , bool , bool , t_color );
            void drawTextInArea( const char*, int, int, t_color, SDL_Rect *);
            void drawRect(int, int, int, int, t_color); //Dibuja un rectangulo rellenado
            void drawRectAlpha(int, int, int, int, t_color, int); //Dibuja un rectangulo rellenado con valor alpha de transparencia
            void drawRectLine(int , int , int , int , int , t_color);//Dibuja un rectangulo sin relleno del ancho especificado
            bool drawImgMem(ImagenGestor *); //Dibuja la imagen que se le ha pasado mediante un array en la memoria
            bool drawImgMem(ImagenGestor *, int , int , int, t_region );
            bool drawImgObj(Object *);
            bool drawFondoImgSel(tEvento, int, int, int, t_region, t_color);
            bool drawImg(ImagenGestor *); //Dibuja la imagen desde la ruta del disco duro pasada por parametro
            void drawUITitleBorder(const char *);
            void drawUITitleBorder(const char *, int, t_color);
            void drawUIButton(Object *obj);
            void drawUIListBox(Object *obj);
            void drawUIPopupMenu(Object *obj);
            void drawUIPicture(Object *obj);
            void drawUIInputWide(Object *obj);
            void drawUIArt(Object *obj);
            void drawUILabel(Object *obj);
            void drawUIProgressBar(Object *obj);
            void drawUIPanel(Object *obj);
            void drawUILetraPopup(Object *obj);
            void drawUIPopupFondo(Object *obj, int);
            void drawUIComboBox(Object *obj);
            void drawListContent(Object *obj, int x, int y, int w, int h);
            void marcarPos(int x, int y);
            void drawUIListGroupBox(Object *obj);
            void drawListGroupContent(Object *obj, int x, int y, int w, int h);
            void drawScrollBar(UIListCommon *obj);

            void drawIco(int , int , int , int , int );
            void drawIco(int numIco, int angle, int x, int y, int w, int h);
            void pintarCirculo (int, int, int, t_color);
            void pintarSemiCirculo (int x, int y, int r, t_color color, int angle);
            void pintarFillCircle(SDL_Surface *surface, int cx, int cy, int radius, Uint32 pixel);
            void pintarLinea (int , int , int , int  , t_color );
            void pintarTriangulo (int , int , int , int , bool , t_color);
            void pintarContenedor(int x1, int y1, int w1, int h1, bool selected, Object *obj, t_color color);
            void pintarDegradado(int x1, int y1, int x2, int y2, int lineas, int grayIni, int grayFin);

            void printContImg(const char *);
            void printGrafica(double, int);
            void showCheck(Object *);

            void fps();
            void clearScr(); //Borra el contenido de la pantalla
            void clearScr(t_color);
            void clearScr(t_color, t_region);
            void flipScr(); //Actualiza la pantalla
            void updateScr(t_region *);

            bool drawImgMoved(ImagenGestor *);
            bool drawZoomImgMem(ImagenGestor *);
            int fontStrLen(string); //Calcula el ancho de un string en pixels
            int fontStrLenCent(string); //Calcula el ancho de un string en pixels para poder centrarlo

            int calcMaxY(int, int);
            int calcMaxX(int, int);
            void getMousePos(int *, int *);
            unsigned int getPosThumb(int, int,  t_region);
            int getWidth(){return w;} //Devuelve el tamanyo de la imagen cargada en memoria
            int getHeight(){return h;}
            void takeScreenShot(SDL_Surface **);
            void takeScreenShot(SDL_Surface **, SDL_Rect);
            void printScreenShot(SDL_Surface **);
            void printScreenShot(SDL_Surface **, SDL_Rect);
            void showMessage(string, unsigned long);
            void drawIco(int numIco, int x, int y);
            void clearEvento(tEvento *);
            void showAutoMessage();
            void setAutoMessage(string);
            void makeMoveSurface(SDL_Surface *);
            void desactivarEventos();
            void activarEventos();
            void pintarIconoProcesando(Thread<Gestorroms> *);
            void msg_processing(unsigned int, unsigned int);
            bool isCanFlip(){return canFlip;}
            void setCanFlip(bool var){canFlip = var;}
            void clearLastEvento(){clearEvento(&lastEvento);}
            string configButtonsJOY(tEvento *evento);
            TTF_Font* getFont(){ return font; }


        protected:
            bool ignoreButtonRepeats;
            SDL_Surface* screen; //Puntero para dibujar por pantalla
            void drawObject(Object *);


        private:
            std::map<int, int>* mPrevAxisValues; //Almacena los valores de los ejes de cada joystick
            std::map<int, int>* mPrevHatValues; //Almacena los valores de las crucetas de cada joystick
            SDL_Joystick** mJoysticks;
            int mNumJoysticks;
            void initJoyMapper();
            TTF_Font* font; //Fuente de textos
            SDL_RWops *RWFont;
            int fontAscent;
            int fontDescent;
            int fontHeight;

            int WINDOW_WIDTH_FULLSCREEN;
            int WINDOW_HEIGHT_FULLSCREEN;
            int SCREEN_BITS_FULLSCREEN;
            int w,h;
            Uint32 fullsflags;
            bool autosize;
            int frames;
            int time;
            long lastTime;
            int totalMuestreo;
            int mediaFps;
            Fileio *fontfile;
            SDL_Surface * moveSurface;
            string mensajeAuto;
            bool canFlip;
            tEvento evento;
            tEvento lastEvento;

            void cachearObjeto(Object *obj);
            void loadImgFromMem(char *, int, SDL_Surface **); //Carga una imagen desde un array
            void loadImgFromMem(ImagenGestor *imgGestor, SDL_Surface **destino); //Carga una imagen desde el objeto imgGestor
            void loadImgFromFile(const char *, SDL_Surface **); //Carga una imagen desde un fichero del disco
            bool redimension(SDL_Surface *,SDL_Surface *, ImagenGestor *, SDL_Surface **);
            void ResizeSurface(float , SDL_Surface *, SDL_Surface **);
            void loadFont(unsigned char); //Carga la fuente de determinado tamanyo
            void loadFontFromFile(const char *);
            bool blitImage(SDL_Surface *, SDL_Surface *,SDL_Rect *, bool);
            void calcRectCent( SDL_Rect *, int , int , int , int, ImagenGestor *);
            inline Uint32 getpixel(SDL_Surface *, const int, const int);
            inline void putpixel(SDL_Surface *, const int, const int, const Uint32);
            inline void putpixelSafe(SDL_Surface *, const int , const int , const Uint32 );
            void Zoom(int );
            float relacion(SDL_Surface *visor, int alto, int ancho, ImagenGestor *);
            bool updateImgScr(SDL_Surface *,SDL_Surface *,ImagenGestor *);
            tInput calculaTextoInput(Object *);
            IcoGestor *gestorIconos;
            SDL_Surface *screenShotSurface;
            SDL_Surface *screenShotThumbSurface;
    };
#endif //Ioutil_H
