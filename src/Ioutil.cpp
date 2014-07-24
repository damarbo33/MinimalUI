#include "Ioutil.h"


#ifdef WIN
    const int SCREEN_MODE = SDL_SWSURFACE|SDL_RESIZABLE; // SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN|SDL_SWSURFACE|SDL_RESIZABLE
#elif UNIX
    const int SCREEN_MODE = SDL_SWSURFACE|SDL_FULLSCREEN; // SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_FULLSCREEN|SDL_SWSURFACE|SDL_RESIZABLE
    //const int SCREEN_MODE = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
#endif // UNIX
const int SURFACE_MODE = SDL_SWSURFACE;
/**
*
*/
Ioutil::Ioutil(){
    canFlip = true;
    autosize = true;
    frames = 0;
    mediaFps = 0;
    time = 0;
    totalMuestreo = 0;
    lastTime = SDL_GetTicks();
    font = NULL;
    RWFont = NULL;
    screenShotSurface = NULL;
    screenShotThumbSurface = NULL;
    fontAscent = 0;
    fontDescent = 0;
    fontHeight = 0;
    moveSurface = NULL;
    mensajeAuto = "";
    fontfile = new Fileio();
    gestorIconos = new IcoGestor();
    loadFontFromFile(FONT_TYPE);
    WINDOW_WIDTH_FULLSCREEN = 0;
    WINDOW_HEIGHT_FULLSCREEN = 0;
    SCREEN_BITS_FULLSCREEN = 0;
    this->w = 0;
    this->h = 0;
    this->fullsflags = SCREEN_MODE;
    ignoreButtonRepeats = false;
    clearEvento(&evento);
    lastEvento.keyjoydown = false;
    initSDL(true);
}




/**
*
*/
Ioutil::~Ioutil(){
    Traza::print("Destructor de Ioutil", W_DEBUG);
    if (screenShotSurface != NULL) SDL_FreeSurface( screenShotSurface );
    if (screenShotThumbSurface != NULL) SDL_FreeSurface( screenShotThumbSurface );
    delete gestorIconos;

    if(mJoysticks != NULL){
//		for(int i = 0; i < mNumJoysticks; i++){
//			if (mJoysticks[i] != NULL)
//                SDL_JoystickClose(mJoysticks[i]);
//		}

		delete[] mJoysticks;
		mJoysticks = NULL;

		delete[] mPrevAxisValues;
		mPrevAxisValues = NULL;

		delete [] mPrevHatValues;
		mPrevHatValues = NULL;
	}

	TTF_CloseFont(font);															// Release the memory allocated to font
	TTF_Quit();																		// Close SDL_TTF
    SDL_Quit();
    delete fontfile;
    Traza::print("Destructor de ioutil FIN", W_DEBUG);
    //Mix_CloseAudio();
}

/**
*
*/
void Ioutil::toggleFullScreen(){
    //SDL_WM_ToggleFullScreen(screen);
    Uint32 flags = SCREEN_MODE; /* Start with whatever flags you prefer */

    /* -- Portable Fullscreen Toggling --
    As of SDL 1.2.10, if width and height are both 0, SDL_SetVideoMode will use the
    width and height of the current video mode (or the desktop mode, if no mode has been set).
    Use 0 for Height, Width, and Color Depth to keep the current values. */
    int tempw = 0, temph = 0, tempbpp = 0;

    flags = screen->flags; /* Save the current flags in case toggling fails */
    if (screen->flags & SDL_FULLSCREEN){
        tempw = Constant::getWINDOW_WIDTH();
        temph = Constant::getWINDOW_HEIGHT();
        tempbpp = SCREEN_BITS;
        screen->flags |= SDL_RESIZABLE;
    } else {
        tempw = WINDOW_WIDTH_FULLSCREEN;
        temph = WINDOW_HEIGHT_FULLSCREEN;
        tempbpp = SCREEN_BITS_FULLSCREEN;
    }

    screen = SDL_SetVideoMode(tempw, temph, tempbpp, screen->flags ^ SDL_FULLSCREEN); /*Toggles FullScreen Mode */
    this->fullsflags = screen->flags;
    this->w = tempw;
	this->h = temph;

    if(screen == NULL) screen = SDL_SetVideoMode(Constant::getWINDOW_WIDTH(), Constant::getWINDOW_HEIGHT(), SCREEN_BITS, flags); /* If toggle FullScreen failed, then switch back */
    if(screen == NULL) exit(1); /* If you can't switch back for some reason, then epic fail */
}



/**
*/
void Ioutil::initSDL(bool calcFS){
    Traza::print("Iniciando SDL", W_DEBUG);
    SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER);			// Initialize SDL

    //Iniciamos la rutina para buscar Joysticks
    Traza::print("Buscando joysticks", W_DEBUG);
    mNumJoysticks = SDL_NumJoysticks();
    mJoysticks = new SDL_Joystick*[mNumJoysticks];
    mPrevAxisValues = new std::map<int, int>[mNumJoysticks];
    mPrevHatValues = new std::map<int, int>[mNumJoysticks];
    int axis = 0;
    int hats = 0;

    for(int i = 0; i < mNumJoysticks; i++){
        mJoysticks[i] = SDL_JoystickOpen(i);
        axis = SDL_JoystickNumAxes(mJoysticks[i]);
        hats = SDL_JoystickNumHats(mJoysticks[i]);
        //cout << "hay " + Constant::TipoToStr(axis) + " axis en el joystick: " + Constant::TipoToStr(i) << endl;
        for(int k = 0; k < axis; k++){
            mPrevAxisValues[i][k] = 0;
        }
        for(int k = 0; k < hats; k++){
            mPrevHatValues[i][k] = 0;
        }
    }
    JoyMapper::initJoyMapper();
    Traza::print("Joystick abierto", W_DEBUG);

    screen = NULL;
    //Iniciamos en modo Fullscreen para saber la resolucion del escritorio
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo ();
    WINDOW_WIDTH_FULLSCREEN = videoInfo->current_w;
    WINDOW_HEIGHT_FULLSCREEN = videoInfo->current_h;
    SCREEN_BITS_FULLSCREEN = videoInfo->vfmt->BitsPerPixel ;

    if(!screen){
        //Comprobamos si es la primera vez que arrancamos la aplicacion y cargamos los valores definidos en el
        //fichero de configuracion
        try{
            if (this->w == 0 || this->h == 0){
                string configIniFile = Constant::getAppDir() + Constant::getFileSep() + "config.ini";
                ListaIni<Data> *config = new ListaIni<Data>();
                Traza::print("Cargando configuracion", W_DEBUG);
                config->loadFromFile(configIniFile);
                config->sort();
                this->w = Constant::strToTipo<int>(config->get(config->find("width")).getValue());
                this->h = Constant::strToTipo<int>(config->get(config->find("height")).getValue());
                if (Constant::strToTipo<int>(config->get(config->find("fullscreen")).getValue()) == 1)
                    this->fullsflags = this->fullsflags | SDL_FULLSCREEN;
                Constant::setTrazaLevel(Constant::strToTipo<int>(config->get(config->find("loglevel")).getValue()));
                delete config;
            }
        } catch (Excepcion &e){
            Traza::print("Error al cargar la configuracion", W_ERROR);
        }

        //En caso de que no se especifique una resolucion de pantalla, obtenemos la del sistema operativo
        Uint8 bpp = SCREEN_BITS;
        if (this->w == 0 || this->h == 0){
            bpp = SCREEN_BITS_FULLSCREEN;
            this->w = WINDOW_WIDTH_FULLSCREEN;
            this->h = WINDOW_HEIGHT_FULLSCREEN;
        }
        Traza::print("Ancho", this->w, W_DEBUG);
        Traza::print("Alto", this->h, W_DEBUG);

        //Si finalmente no hemos encontrado un alto y ancho adecuados, lo ponemos por defecto
        if (this->w == 0 || this->h == 0){
            this->w = 640;
            this->h = 480;
            bpp = 16;
        }

        //Finalmente establecemos el modo del video
        screen = SDL_SetVideoMode(this->w, this->h, bpp, this->fullsflags);
        if(!screen){
            cerr << "Error iniciando la pantalla width: " << this->w << " height: " << this->h << ". Saliendo de la aplicación" << endl;
            exit(0);
        }
	}

    Traza::print("Mostrando cursor", W_DEBUG);
	SDL_ShowCursor(CURSORVISIBLE);	// Disable mouse cursor on gp2x
	SDL_WM_SetCaption( WINDOW_TITLE, 0 );	// Sets the window title (not needed for gp2x)
	Traza::print("Iniciando TTF", W_DEBUG);
	TTF_Init();		// Initialize SDL_TTF
	Traza::print("Cargando fuente", W_DEBUG);
	loadFont(FONTSIZE);
	Traza::print("FIN initSDL", W_DEBUG);
}

/**
*
*/
void Ioutil::killSDL(){
	TTF_Quit();																		// Close SDL_TTF
    SDL_JoystickEventState(SDL_DISABLE);
    SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Quit();
    TTF_CloseFont(font);
    font = NULL;
    JoyMapper::clearJoyMapper();
}

/**
*
*/
void Ioutil::loadFontFromFile(const char *filename){
    string dir = Constant::getAppDir() + Constant::getFileSep() + filename;
    try{
        //La llamada al loadFromFile ya libera recursos en caso de necesitarlos. No nos preocupamos por ello
        Traza::print("Abriendo fuente: " + dir, W_DEBUG);
        fontfile->loadFromFile(dir.c_str());
    } catch (Excepcion &e){
        Traza::print("loadFontFromFile: No se encuentra el fichero de la fuente" + string(dir), W_ERROR);
    }
}


/**
*
*/
void Ioutil::loadFont(unsigned char size){
    if(font != NULL){
        Traza::print("Cerrando la fuente", W_PARANOIC);
        TTF_CloseFont(font);
        Traza::print("Asignando a null", W_PARANOIC);
        font = NULL;
    }
    Traza::print("Cargando de memoria", W_PARANOIC);
    SDL_RWops *RWOps = SDL_RWFromMem(fontfile->getFile(), fontfile->getFileSize());

    Traza::print("Abriendo la fuente", W_PARANOIC);
    if (RWOps != NULL){
        font = TTF_OpenFontRW(RWOps,1,size);
        fontAscent = TTF_FontAscent(font);
        fontDescent = TTF_FontDescent(font);
        fontHeight = TTF_FontHeight(font);
        //Establecemos el tam de los menus a un 50% mas que el tam de la fuente
        Constant::setMENUSPACE(ceil(fontHeight * 1.5));
        Constant::setINPUTH(ceil(fontHeight * 1.2));
    } else {
        Traza::print("No se puede cargar la fuente", W_ERROR);
    }
}

/**
*
*/
void Ioutil::clearScr(){
    //SDL_BlitSurface(empty, NULL, screen, NULL);
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, cBgMenu.r,cBgMenu.g,cBgMenu.b));
}

/**
*/
void Ioutil::clearScr(t_color color){
    SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, color.r,color.g,color.b));
}

/**
*/
void Ioutil::clearScr(t_color color, t_region region){
    SDL_Rect location = {(short int)region.selX, (short int)region.selY, (short unsigned int)region.selW, (short unsigned int)region.selH};
    SDL_FillRect(screen, &location, SDL_MapRGB(screen->format, color.r,color.g,color.b));
}


/**
*/
void Ioutil::flipScr(){
    if (canFlip){
//        if ( fbdev >= 0 ) {
//            int arg = 0;
//            ioctl( fbdev, FBIO_WAITFORVSYNC, &arg );
//        }
        SDL_Flip(screen);
        //updateScr(NULL);
    }
}

/**
*/
void Ioutil::updateScr(t_region *region){
    //SDL_Flip(screen);
    if (region != NULL){
        SDL_UpdateRect(screen, region->selX, region->selY, region->selW, region->selH);
    } else {
        SDL_UpdateRect(screen, 0,0,0,0);
    }

}


/**
*
*/
void Ioutil::getMousePos(int *mouse_x, int *mouse_y){
    SDL_GetMouseState(mouse_x, mouse_y);
}

void Ioutil::clearEvento(tEvento *evento){
    evento->key = INT_MIN;
    evento->joy = INT_MIN;
    evento->isJoy = false;
    evento->isKey = false;
    evento->isMouse = false;
    evento->keyMod = INT_MIN;
    evento->unicode = INT_MIN;
    evento->resize = false;
    evento->quit = false;
    evento->isMousedblClick = false;
    evento->isMouseMove = false;
    evento->isRegionSelected = false;
    evento->mouse_state = INT_MIN;
    evento->mouse = INT_MIN;
    evento->mouse_x = 0;
    evento->mouse_y = 0;
    evento->keyjoydown = false;
    //t_region region;
}

void Ioutil::desactivarEventos(){
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_IGNORE);
    SDL_EventState(SDL_KEYDOWN, SDL_IGNORE);
    SDL_EventState(SDL_KEYUP, SDL_IGNORE);
}

void Ioutil::activarEventos(){
    SDL_EventState(SDL_MOUSEBUTTONDOWN, SDL_ENABLE);
    SDL_EventState(SDL_KEYDOWN, SDL_ENABLE);
    SDL_EventState(SDL_KEYUP, SDL_ENABLE);


}

/**
*/
tEvento Ioutil::WaitForKey(){

    static unsigned long lastClick = 0;
    static unsigned long lastKeyDown = 0;
    static unsigned long retrasoTecla = KEYRETRASO;
    static unsigned long lastMouseMove = 0;

    clearEvento(&evento);
    SDL_Event event;
    //while( SDL_PollEvent( &event ) ){
    if( SDL_PollEvent( &event ) ){
    //if (SDL_WaitEvent (&event)){
        switch( event.type ){
            case SDL_VIDEORESIZE:
                screen = SDL_SetVideoMode (event.resize.w, event.resize.h, SCREEN_BITS, SCREEN_MODE);
                this->w = event.resize.w;
                this->h = event.resize.h;
                evento.resize = true;
                break;
            case SDL_JOYBUTTONDOWN: // JOYSTICK/GP2X buttons
                if (event.jbutton.button >= 0 && event.jbutton.button < MAXJOYBUTTONS){
//                    evento.joy = Constant::getJoyMapper(event.jbutton.button);
                    evento.joy = event.jbutton.button;
                    evento.isJoy = true;
                    evento.keyjoydown = true;
                    lastEvento = evento;    //Guardamos el ultimo evento que hemos lanzado desde el teclado
                    lastKeyDown = SDL_GetTicks();  //reseteo del keydown
                }
                break;
            case SDL_JOYBUTTONUP:
                lastEvento = evento;
                evento.keyjoydown = false;
                break;
            case SDL_JOYHATMOTION:
                mPrevHatValues[event.jhat.which][event.jhat.hat] = event.jhat.value;
                evento.isJoy = true;


                if (event.jhat.value & SDL_HAT_UP){
                    evento.joy = JOYHATOFFSET + event.jhat.value;
                } else if (event.jhat.value & SDL_HAT_DOWN){
                    evento.joy = JOYHATOFFSET + event.jhat.value;
                } else if (event.jhat.value & SDL_HAT_LEFT){
                    evento.joy = JOYHATOFFSET + event.jhat.value;
                } else if (event.jhat.value & SDL_HAT_RIGHT){
                    evento.joy = JOYHATOFFSET + event.jhat.value;
                }

                if (event.jhat.value == 0) evento.keyjoydown = false;
                else {
                    evento.keyjoydown = true;
                    lastKeyDown = SDL_GetTicks();  //reseteo del keydown
                }
                lastEvento = evento;    //Guardamos el ultimo evento que hemos lanzado

                break;
            case SDL_JOYAXISMOTION:
                if((abs(event.jaxis.value) > DEADZONE) != (abs(mPrevAxisValues[event.jaxis.which][event.jaxis.axis]) > DEADZONE))
                {
                    int normValue;
                    evento.isJoy = true;

                    if(abs(event.jaxis.value) <= DEADZONE){
                        normValue = 0;
                        evento.keyjoydown = false;
                    } else {
                        if(event.jaxis.value > 0)
                            normValue = 1;
                        else
                            normValue = -1;

                        evento.keyjoydown = true;
                        lastKeyDown = SDL_GetTicks();  //reseteo del keydown
                    }

                    int valor = (abs(normValue) << 4 | event.jaxis.axis) * normValue;
                    evento.joy = JOYAXISOFFSET + valor;
                    lastEvento = evento;    //Guardamos el ultimo evento que hemos lanzado desde el teclado

                }

                mPrevAxisValues[event.jaxis.which][event.jaxis.axis] = event.jaxis.value;

                break;
            case SDL_KEYDOWN: // PC buttons
                evento.key = event.key.keysym.sym;
                evento.keyMod = event.key.keysym.mod;
                evento.unicode = event.key.keysym.unicode;
                evento.isKey = true;
                evento.keyjoydown = true;
//                if (evento.keyMod & KMOD_LCTRL && evento.key == SDLK_c) evento.quit = true;
                lastEvento = evento;    //Guardamos el ultimo evento que hemos lanzado desde el teclado
                lastKeyDown = SDL_GetTicks();  //reseteo del keydown
                break;
            case SDL_KEYUP: // PC button keyup
                lastEvento = evento;
                break;
            case SDL_MOUSEBUTTONDOWN: // Mouse buttons SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE, SDL_BUTTON_RIGHT, SDL_BUTTON_WHEELUP, SDL_BUTTON_WHEELDOWN
                evento.mouse = event.button.button;
                evento.mouse_x = event.button.x;
                evento.mouse_y = event.button.y;
                evento.mouse_state = event.button.state;
                evento.isMouse = true;
                if (SDL_GetTicks() - lastClick < DBLCLICKSPEED){
                   evento.isMousedblClick = true;
                   lastClick = SDL_GetTicks() - DBLCLICKSPEED;  //reseteo del dobleclick
                } else {
                    lastClick = SDL_GetTicks();
                }

                break;
            case SDL_MOUSEBUTTONUP:
                evento.mouse = event.button.button;
                evento.mouse_x = event.button.x;
                evento.mouse_y = event.button.y;
                evento.mouse_state = event.button.state;
                evento.isMouse = true;
                break;
            case SDL_MOUSEMOTION:
                evento.mouse = event.button.button;
                evento.mouse_x = event.button.x;
                evento.mouse_y = event.button.y;
                evento.mouse_state = event.button.state;
                evento.isMouseMove = true;
                lastMouseMove = SDL_GetTicks();
                if (SDL_ShowCursor(SDL_QUERY) == SDL_DISABLE)    SDL_ShowCursor(SDL_ENABLE);
                break;
            case SDL_QUIT:
                evento.quit = true;
                break;
            default :
                break;
        }
    }

    unsigned long now = SDL_GetTicks();

    //En algunas ocasiones se repiten eventos. Con este flag los controlamos
    if (ignoreButtonRepeats){
        lastEvento.keyjoydown = false;
        ignoreButtonRepeats = false;
        Traza::print("Ignorando retraso de teclas", W_DEBUG);
    }

    if (lastEvento.keyjoydown == true){
        if (now > lastKeyDown + KEYDOWNSPEED + retrasoTecla){
            Traza::print("Repitiendo tecla", W_DEBUG);
            lastKeyDown = SDL_GetTicks();
            evento = lastEvento;
            retrasoTecla = 0;
        }
    } else {
        retrasoTecla = KEYRETRASO;
    }

    if (now > lastMouseMove + MOUSEVISIBLE && SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE){
        SDL_ShowCursor(SDL_DISABLE);
    }

    return evento;
}

/**
*
*/
void Ioutil::printGrafica(double fase, int altura){
    int funcion = 0;
    int periodo = 80;
    int amplitud = 15;

    for (int i=0;i < this->getWidth(); i++){
        funcion = (int)(amplitud*sin(2*PI/periodo*i + fase) + altura);
        if (funcion > altura){
            funcion = altura - (funcion - altura);
        }

        for (int j=0; j < amplitud; j++){
           if (funcion + j >= 0 && funcion + j < this->getHeight())
            putpixelSafe(screen,i, funcion + j,SDL_MapRGB(screen->format, 255, 255, 255));
        }
    }
    drawRect(0, altura, this->getWidth(), this->getHeight()-altura, cRojo);
}


/**
*
*/
void Ioutil::fps(){
    drawTextInt(mediaFps,this->getWidth() - 60,0,cRojo);
    if (SDL_GetTicks() - lastTime > FRAMEPERIOD){
        mediaFps = frames;
        frames = 0;
        lastTime = SDL_GetTicks();
    } else{
        frames++;
    }
}

/**
*
*/
void Ioutil::printContImg(const char *dato){
    int pixelDato = 0;
    TTF_SizeText(this->font,dato,&pixelDato,NULL );
    SDL_Rect textLocation = {(short int)(this->getWidth() - pixelDato), (short int)(this->getHeight() - Constant::getMENUSPACE()), (short unsigned int)pixelDato, (short unsigned int)Constant::getMENUSPACE()};
    SDL_FillRect(screen, &textLocation, SDL_MapRGB(screen->format, 0,0,0));
    drawText(dato,this->getWidth() - pixelDato,this->getHeight() - Constant::getMENUSPACE(),cBlanco);
}

/**
*
*/
void Ioutil::drawTextCent( const char* dato, int x, int y, bool centx, bool centy, t_color color){
    if (font != NULL){
        int pixelDato = 0;
        TTF_SizeText(this->font,dato,&pixelDato,NULL );
        int posDatox = x;
        int posDatoy = y;

        if (centx){
            posDatox = (screen->w - pixelDato)/2;
            posDatox += x;
        }
        if (centy){
            posDatoy = (screen->h)/2;
            posDatoy += y;
        }
        drawText(dato,posDatox,posDatoy,color);
    } else {
        Traza::print("Fallo en drawTextCent: La fuente es NULL", W_ERROR);
    }
}

/**
*
*/
void Ioutil::drawText( const char* dato, int x, int y, t_color color){
        drawTextInArea(dato, x, y, color, NULL);
}

/**
*
*/
void Ioutil::drawTextInArea( const char* dato, int x, int y, t_color color, SDL_Rect *textLocation){
    if (font != NULL){
        SDL_Color foregroundColor = { (unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b };
        SDL_Surface* textSurface =  TTF_RenderText_Blended(font,dato, foregroundColor);
        SDL_Rect screenLocation = { (short int)x, (short int)y, 0, 0 };
        SDL_BlitSurface(textSurface, textLocation, screen, &screenLocation);
        SDL_FreeSurface(textSurface);
    } else {
        Traza::print("Fallo en drawTextInArea: La fuente es NULL", W_ERROR);
    }
}


/**
*/
void Ioutil::drawTextInt(int number, int x, int y, t_color color){
    if (font != NULL){
        char tmp_num[16];
        sprintf((char*)tmp_num, "%d", number);
        SDL_Color foregroundColor = { (unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b };
        SDL_Surface* textSurface =  TTF_RenderText_Blended(font,tmp_num, foregroundColor);
        SDL_Rect screenLocation = { (short int)x, (short int)y, 0, 0 };
        SDL_BlitSurface(textSurface, NULL, screen, &screenLocation);
        SDL_FreeSurface(textSurface);
    } else {
        Traza::print("Fallo en drawTextInt: La fuente es NULL", W_ERROR);
    }
}

/**
*
*/
void Ioutil::drawRectAlpha(int x, int y, int w, int h, t_color color, int colorAlpha){
    SDL_Surface *mySurface = NULL;
    SDL_Rect dstrect = { (short int)x, (short int)y, (short unsigned int)w, (short unsigned int)h };

    if (w == 0)
        dstrect.w = screen->w;

    if (h == 0)
        dstrect.h = screen->h;

    if (w > 0 && h > 0){
        //Si tenemos que generar una surface con alpha, va a ralentizar a la aplicacion dependiendo mucho del tamanyo de la pantalla
        if (colorAlpha > 0 && colorAlpha <= 255){
            mySurface = SDL_CreateRGBSurface(SURFACE_MODE, dstrect.w, dstrect.h, screen->format->BitsPerPixel,0, 0, 0, 0);
            SDL_FillRect( mySurface, NULL, SDL_MapRGB(screen->format, color.r, color.g, color.b) );
            SDL_SetColorKey(mySurface, SDL_SRCCOLORKEY, 128);
            SDL_SetAlpha(mySurface, SDL_SRCALPHA, colorAlpha);
            SDL_BlitSurface(mySurface, NULL, screen, &dstrect);
            SDL_FreeSurface( mySurface );
        } else {
             SDL_FillRect( screen, &dstrect, SDL_MapRGB(screen->format, color.r, color.g, color.b) );
        }

    } else {
        Traza::print("Error de ancho y alto en drawRectAlpha" + Constant::TipoToStr(x) + ";"+ Constant::TipoToStr(y) + ";"
                     + Constant::TipoToStr(w) + ";"+ Constant::TipoToStr(h) + ";", W_ERROR);
        //throw(Excepcion(ERANGO));
    }
}

/**
*
*/
void Ioutil::drawRect(int x, int y, int w, int h, t_color color){
//    try{
        drawRectAlpha(x,y,w,h,color,-1);
//    } catch (Excepcion &e){
//        Traza::print("Excepcion drawRect" + string(e.getMessage()), W_ERROR);
//    }

}

/**
* Necesito pasar un puntero al puntero porque sino el valor del puntero se parasaria por valor. De esta forma el puntero
* queda con el valor almacenado.
*/
void Ioutil::takeScreenShot(SDL_Surface **pointerSurface){
    if (*pointerSurface != NULL) SDL_FreeSurface( *pointerSurface );
        *pointerSurface = SDL_CreateRGBSurface(SURFACE_MODE, screen->w, screen->h, screen->format->BitsPerPixel,0, 0, 0, 0);
    SDL_BlitSurface(screen, NULL, *pointerSurface, NULL);
}

/**
*
*/
void Ioutil::takeScreenShot(SDL_Surface **pointerSurface, SDL_Rect dstrect){
    if (*pointerSurface != NULL) SDL_FreeSurface( *pointerSurface );
        *pointerSurface = SDL_CreateRGBSurface(SURFACE_MODE, dstrect.w, dstrect.h, screen->format->BitsPerPixel,0, 0, 0, 0);
    SDL_BlitSurface(screen, &dstrect, *pointerSurface, NULL);
}

/**
*
*/
void Ioutil::printScreenShot(SDL_Surface **pointerSurface){
    SDL_BlitSurface(*pointerSurface, NULL,screen , NULL);
}

/**
*
*/
void Ioutil::printScreenShot(SDL_Surface **pointerSurface, SDL_Rect dstrect){
    SDL_BlitSurface(*pointerSurface, NULL,screen , &dstrect);
}

/**
*
*/
//Los campos x0, x1, y0, y1 representan las esquinas
void Ioutil::drawRectLine(int x0, int y0, int w0, int h0, int lineWidth, t_color color){

    int x1 = (x0 + w0) < this->w ? (x0 + w0) : this->w-1 ;
    int y1 = (y0 + h0) < this->h ? (y0 + h0) : this->h-1 ;

    if (y0+lineWidth < this->w)
        for (int i=0;i<lineWidth;i++){
            pintarLinea(x0,y0+i, x1, y0+i, color); //Superior
            pintarLinea(x0,y1-i, x1, y1-i, color); //Inferior
            pintarLinea(x0+i,y0, x0+i, y1, color); //Izquierda
            pintarLinea(x1-i,y0, x1-i, y1, color);//Derecha
        }
}

/**
*
*/
void Ioutil::pintarContenedor(int x1, int y1, int w1, int h1, bool selected, Object *obj){
    if (w1 >= 0 && h1 >= 0 && w1 >= INPUTBORDER && h1 >= INPUTBORDER){
        if (obj->isVerContenedor()){
            if ( (selected && obj == NULL) || (selected && obj != NULL && obj->showShadow()) ){
                drawRectLine(x1+INPUTBORDER, y1 + INPUTBORDER, w1, h1, BORDERSELECTION, cAzulOscuro);//Dibujo la sombra del elemento seleccionado
            }
            if (obj->getAlpha() >= 0){
                drawRectAlpha(x1+INPUTBORDER,y1+INPUTBORDER,w1-INPUTBORDER,h1-INPUTBORDER,cInputContent, obj->getAlpha()); // Dibujo el contenedor
            } else {
                drawRect(x1+INPUTBORDER,y1+INPUTBORDER,w1-INPUTBORDER,h1-INPUTBORDER,cInputContent); // Dibujo el contenedor
            }

            drawRectLine(x1,y1,w1,h1,INPUTBORDER,cInputBorder);//Dibujo el borde
        }
    } else {
        Traza::print("Error del objeto " + obj->getLabel() + " con ancho y alto en pintarContenedor: " + Constant::TipoToStr(x1) + ";"+ Constant::TipoToStr(y1) + ";"
                     + Constant::TipoToStr(w1) + ";"+ Constant::TipoToStr(h1) + ";", W_ERROR);
    }
}

//void Ioutil::pintarLinea (int x1, int y1, int x2, int y2 , t_color color){
//    //Draw_Line(screen, x1,y1,x2,y2, SDL_MapRGB(screen->format, color.r, color.g, color.b));
//}

/**
*
*/
void Ioutil::pintarLinea (int x1, int y1, int x2, int y2 , t_color color)
{
    if (x1 >= 0 && y1 < this->getWidth()-1 && x2 >= 0 && y2 < this->getHeight()-1){
        int tempInt = 0;
        int i = x1;
        int j = y1;
        int tempj = 0;
        int numerador = (y1-y2);
        int denominador = (x1-x2);
        float pendiente = 0;
        int dif = 0;
        int contj = 0;
        int conti = 0;
        int difx = (x2 > x1) ? x2-x1+1 : x1-x2+1;
        int dify = (y2 > y1) ? y2-y1+1 : y1-y2+1;

        if (denominador != 0){
            pendiente = numerador/(float)denominador;
            while (conti < difx && conti < screen->w){
                //Funcion de la recta -> j = (y1-y2)/(x1-x2) * (i-x1) + y1
                j = (int)(pendiente * (i-x1)) + y1;
                tempj = (int)(pendiente * (i+1-x1)) + y1;
                dif = (j > tempj) ? j-tempj : tempj-j;

                if (dif > 1){
                    contj = 0;
                    while (contj < dif && (conti+1)*dif < dify){
                        putpixelSafe(screen, i, (j > tempj) ? j-contj : j + contj, SDL_MapRGB(screen->format, color.r, color.g, color.b));
                        contj++;
                    }
                } else {
                    putpixelSafe(screen,i,j,SDL_MapRGB(screen->format, color.r, color.g, color.b));
                }

                if ((pendiente > 0. && denominador < 0.) ||
                    (pendiente < 0. && denominador < 0.) ||
                    (pendiente == 0 && denominador < 0.)){
                        i++;
                } else if((pendiente < 0. && denominador > 0.) ||
                        (pendiente > 0. && denominador > 0.) ||
                        (pendiente == 0 && denominador > 0.)) {
                        i--;
                }

                conti++;
            }
        } else {
            //    //Comprobamos que el x1 sea siempre el menor punto
            if (x1 > x2) {
                tempInt = x1;
                x1 = x2;
                x2 = tempInt;
            }

            if (y1 > y2) {
                tempInt = y1;
                y1 = y2;
                y2 = tempInt;
            }
            i = x1;
            j = y1;

            while (j <= y2 && j < screen->h){
                putpixelSafe(screen,i,j,SDL_MapRGB(screen->format, color.r, color.g, color.b));
                j++;
            }
        }
    }
}

/**
*
*/
unsigned int Ioutil::getPosThumb(int mouse_x, int mouse_y, t_region regionPantalla){
    int posClick_x = 0;
    int posClick_y = 0;
    int destw = THUMBW;
    int desth = THUMBH;
    int maxX = calcMaxX(destw, regionPantalla.selW);
    int maxY = calcMaxY(desth, regionPantalla.selH);
    unsigned int posicion = 0;

    if ((destw <= 0 || desth <= 0) && maxX > 0 && maxY > 0){
            destw  = (regionPantalla.selW - SEPTHUMB - SEPTHUMB * maxX) / maxX;
            desth = (regionPantalla.selH - SEPTHUMB - SEPTHUMB * maxY) / maxY;
    }

    posClick_x = (mouse_x - regionPantalla.selX) / (destw+SEPTHUMB);
    posClick_y = (mouse_y - regionPantalla.selY) / (desth+SEPTHUMB);

    posicion = posClick_y * maxX + posClick_x;

    return posicion;
}

/**
* drawImgMem - Dibuja por pantalla la imagen pasada por parametro en un conjunto de
* imagenes en forma de grid.
*/
bool Ioutil::drawImgMem(ImagenGestor *imgGestor, int indice, int destw, int desth, t_region regionPantalla){
        try{
            SDL_Surface *mySurface;
            int maxX = calcMaxX(destw, regionPantalla.selW);
            int maxY = calcMaxY(desth, regionPantalla.selH);
            Traza::print("maxX: " + Constant::TipoToStr(maxX) + " maxY: " + Constant::TipoToStr(maxY), W_PARANOIC);

            if ((destw <= 0 || desth <= 0) && maxX > 0 && maxY > 0){
                destw  = (regionPantalla.selW - SEPTHUMB - SEPTHUMB * maxX) / maxX;
                desth = (regionPantalla.selH - SEPTHUMB - SEPTHUMB * maxY) / maxY;
            }
            const int maxImg = maxX * maxY;

            if (indice >= maxImg) return false;
            Traza::print("Calculando posiciones", W_PARANOIC);
            //Calculamos las filas por columnas y la posicion
            const int fila = maxX > 0 ? indice / maxX : 0; //El cociente son las filas
            Traza::print("filas: " + Constant::TipoToStr(fila), W_PARANOIC);
            const int col  = maxX > 0 ? indice % maxX : 0; //El resto son las columnas
            Traza::print("cols: " + Constant::TipoToStr(col), W_PARANOIC);
            const short int posX = (short int)(regionPantalla.selX + col*(destw+SEPTHUMB)+ ((col > 0) ? SEPTHUMB : 0));
            const short int posY = (short int)(regionPantalla.selY + fila*(desth+SEPTHUMB)+ ((fila > 0) ? SEPTHUMB : 0));

            Traza::print("Drawing at position posX: " + Constant::TipoToStr(posX) + " posY: " + Constant::TipoToStr(posY), W_PARANOIC);

            SDL_Rect imgLocation = { posX,
                                     posY,
                                     (short unsigned int)destw,
                                     (short unsigned int)desth };

            loadImgFromMem(imgGestor->getFile(),imgGestor->getFileSize(), &mySurface);
            //Creamos la imagen y la pintamos por pantalla
            SDL_Surface *thumbSurface = SDL_CreateRGBSurface(SURFACE_MODE, destw, desth, screen->format->BitsPerPixel, 0, 0, 0, 0);

            if (mySurface != NULL) {
                updateImgScr(mySurface, thumbSurface, imgGestor);
                SDL_BlitSurface(thumbSurface, NULL, screen, &imgLocation);
            }
            SDL_FreeSurface( thumbSurface );
            return true;
        } catch (Excepcion &e) {
             Traza::print("Excepcion Ioutil::drawImgMem:" + string(e.getMessage()), W_ERROR);
             return false;
        }
}

/**
*
*/
void Ioutil::drawUIArt(Object *obj){

    SDL_Rect imgLocation = { (short int)obj->getX() , (short int)obj->getY(), (short unsigned int)obj->getW(), (short unsigned int)obj->getH() };

    if (!obj->getImgDrawed()){
        int fontStyle = TTF_GetFontStyle(font);
        if (!(fontStyle & TTF_STYLE_BOLD)) {
            TTF_SetFontStyle(font, fontStyle | TTF_STYLE_BOLD);
        }

        int tamLabel = fontStrLen(obj->getLabel().c_str());

        drawRect(obj->getX(), obj->getY(), obj->getW(), obj->getH(), cAzul);
        drawRectLine(obj->getX(), obj->getY(), obj->getX() + obj->getW()-1, obj->getY() + obj->getH()-1,1, cAzulOscuro);
        int posx = obj->getW() - tamLabel;
        int posy = (obj->getH() - FONTSIZE)/2;

        //Si el tamanyo del texto a pintar es menor que el disponible, centro el texto
        if (tamLabel < obj->getW()){
            posx = posx / 2;
        }

        drawText(obj->getLabel().c_str(),posx, posy, cBlanco);
        TTF_SetFontStyle(font, fontStyle);

        //Reseteamos el surface
        obj->getImgGestor()->setSurface(NULL);
        //Guardamos la imagen en el objeto
        takeScreenShot(&obj->getImgGestor()->surface, imgLocation);
        //Indicamos que ya hemos pintado la imagen
        obj->setImgDrawed(true);
    } else {
        //SDL_Surface *pointerSurface = obj->getImgGestor()->getSurface();
        printScreenShot(&obj->getImgGestor()->surface, imgLocation);
    }
}

/**
*
*/
bool Ioutil::drawImgObj(Object *obj){
    bool salida = true;
    SDL_Rect imgLocation = { (short int)obj->getX() , (short int)obj->getY(), (short unsigned int)obj->getW(), (short unsigned int)obj->getH() };

    if (!obj->getImgDrawed()){
        Traza::print("Drawing image from object", W_PARANOIC);
        t_region regionPantalla = {obj->getX() , obj->getY(), obj->getW(), obj->getH()};
        Traza::print("Region generated", W_PARANOIC);
        salida = drawImgMem(obj->getImgGestor(), -1, obj->getW(), obj->getH(), regionPantalla);
        Traza::print("Image Drawed", W_PARANOIC);

        if (salida == false){
            int pixelDato = 0;
            char msg[] = {"Fin/inicio del contenido"};
            TTF_SizeText(this->font,msg,&pixelDato,NULL );
            drawText("Fin/inicio del contenido", obj->getX() + (obj->getW() - pixelDato) / 2, obj->getY() + (obj->getH() - Constant::getMENUSPACE()) / 2, cBlanco);
        }
    } else {
        SDL_Rect imgRect = { (short int)obj->getImgGestor()->getLeftDif() ,
                             (short int)obj->getImgGestor()->getTopDif(),
                             (short unsigned int)(obj->getImgGestor()->getLeftDif()  + obj->getW()),
                             (short unsigned int)(obj->getImgGestor()->getTopDif() + obj->getH()) };
        Traza::print("getLeftDif",obj->getImgGestor()->getLeftDif(),W_PARANOIC);
        SDL_BlitSurface(obj->getImgGestor()->getSurface(), &imgRect, screen, &imgLocation);
    }
    return salida;
}


/**
*
*/
bool Ioutil::drawImgMem(ImagenGestor *imgGestor){
        try{
            SDL_Surface *mySurface = NULL;
            loadImgFromMem(imgGestor->getFile(),imgGestor->getFileSize(), &mySurface);

            if (mySurface != NULL) {
                return updateImgScr(mySurface, screen, imgGestor);
            } else {
                return false;
            }
        } catch (Excepcion &e) {
             Traza::print("Excepcion Ioutil::drawImg:" + string(e.getMessage()), W_ERROR);
             return false;
         }
}

/**
*
*/
void Ioutil::drawIco(int numIco, int x, int y){
    gestorIconos->drawIcono(numIco, screen, x, y);
}

/**
*
*/
void Ioutil::drawIco(int numIco, int x, int y, int w, int h){
    gestorIconos->drawIcono(numIco, screen, 0, x, y, w, h);
}

/**
*
*/
void Ioutil::drawIco(int numIco, int angle, int x, int y, int w, int h){
    gestorIconos->drawIcono(numIco, screen, angle, x, y, w, h);
}



/**
* hace lo mismo que drawImgMem pero almacena en un surface temporal la imagen redimensionada
* para que se pueda mover a través de ella mas rapidamente
*/
bool Ioutil::drawZoomImgMem(ImagenGestor *imgGestor){
    try{
        SDL_Surface *mySurface = NULL;
        SDL_Surface *bitmap = NULL;
        bool salida = false;
        SDL_Rect dstRect = { 0,0,0,0 };

        loadImgFromMem(imgGestor->getFile(),imgGestor->getFileSize(), &mySurface);

        if (mySurface != NULL) {
            clearScr(cBgImageBin);
            if (imgGestor->isResize()){
                if (redimension(mySurface, screen, imgGestor, &bitmap)){
                    makeMoveSurface(bitmap);
                    calcRectCent(&dstRect, bitmap->w, bitmap->h, screen->w, screen->h, imgGestor);
                    salida = blitImage(bitmap, screen,&dstRect, true);
                } else {
                    makeMoveSurface(mySurface);
                    calcRectCent(&dstRect, mySurface->w, mySurface->h, screen->w, screen->h, imgGestor);
                    salida = blitImage(mySurface, screen,&dstRect, true);
                }
                //objTraza->print("Con resize",(int)(SDL_GetTicks()-now));
            } else {
                makeMoveSurface(mySurface);
                calcRectCent(&dstRect, mySurface->w, mySurface->h, screen->w, screen->h, imgGestor);
                salida = blitImage(mySurface, screen,&dstRect, true);
                //objTraza->print("Sin resize",(int)(SDL_GetTicks()-now));
            }
            flipScr();
            return salida;
        } else {
            return false;
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion Ioutil::drawImg:" + string(e.getMessage()), W_ERROR);
         return false;
     }
}


void Ioutil::makeMoveSurface(SDL_Surface *mySurface){
    if (mySurface != NULL) {
        if (mySurface->w > getWidth() || mySurface->h > getHeight())
            moveSurface = SDL_ConvertSurface(mySurface, mySurface->format, mySurface->flags);
    }
}

/**
* En base al surface temporal creado en la llamada a drawZoomImgMem mueve el punto de
* vista para situarnos donde sea necesario
*/
bool Ioutil::drawImgMoved(ImagenGestor *imgGestor){
    SDL_Rect dstRect = { 0,0,0,0 };
    bool salida = false;
    if (moveSurface != NULL){
        clearScr(cBgImageBin);
        calcRectCent(&dstRect, moveSurface->w, moveSurface->h, screen->w, screen->h, imgGestor);
        salida = blitImage(moveSurface, screen,&dstRect, false);
        flipScr();
    }
    //objTraza->print("Con resize",(int)(SDL_GetTicks()-now));
    return salida;
}

/**
*
*/
int Ioutil::calcMaxX(int destw, int surfaceW)
{
    int maxX = MAXTHUMBX;
    if (destw > 0 && destw + SEPTHUMB > 0 && surfaceW >= SEPTHUMB){
        maxX = (surfaceW - SEPTHUMB) / (destw + SEPTHUMB);
    }
    //if (maxX == 0) return 1;
    return maxX;
}

/**
*
*/
int Ioutil::calcMaxY(int desth,  int surfaceH)
{
    int maxY = MAXTHUMBY;
    if (desth > 0 && desth + SEPTHUMB > 0 && surfaceH >= SEPTHUMB){
        maxY = (surfaceH - SEPTHUMB) / (desth + SEPTHUMB);
    }
    if (maxY == 0) return 1;
    return maxY;
}

/**
*
*/
bool Ioutil::drawFondoImgSel(tEvento evento, int indice, int destw, int desth, t_region regionPantalla, t_color color){
    //Pintamos El fondo para la imagen seleccionada
    int maxX = calcMaxX(destw, regionPantalla.selW);
    int maxY = calcMaxY(desth, regionPantalla.selH);

    if (maxX > 0 || maxY > 0){
        return false;
    }

    if (destw <= 0 || desth <= 0){
        destw  = (regionPantalla.selW - SEPTHUMB - SEPTHUMB * maxX) / maxX;
        desth = (regionPantalla.selH - SEPTHUMB - SEPTHUMB * maxY) / maxY;
    }
    const int maxImg = maxX * maxY;

    if (indice >= maxImg){
        return false;
    } else {
        //Calculamos las filas por columnas y la posicion
        int fila = 0;
        int col  = 0;
        int selX = 0;
        int selY = 0;
        int selW = 0;
        int selH = 0;
        //Se repinta el fondo con un color definido
        for (int i=0;i<maxImg;i++){
            fila = i / maxX; //El cociente son las filas
            col  = i % maxX; //El resto son las columnas
            selX = regionPantalla.selX + col*(destw+SEPTHUMB);
            selY = fila*(desth+SEPTHUMB);
            selW = destw+SEPTHUMB*2;
            selH = desth+SEPTHUMB*2;
            //drawRectLine(selX, selY, selX+selW-1, selY+selH-1, SEPTHUMB, cBgImageBin);
            drawRectLine(selX, selY, selW-1, selH-1, SEPTHUMB, cBgImageBin);
        }
        int posFinal = 0;

        if (evento.isRegionSelected == true){
            posFinal = getPosThumb(evento.region.selX + evento.region.selW, evento.region.selY + evento.region.selH, regionPantalla);
        } else {
            posFinal = indice;
        }

        int filaFinal = posFinal / maxX; //El cociente son las filas
        int colFinal = posFinal % maxX; //El resto son las columnas
        int colInicial = indice % maxX; //El resto son las columnas

        //Se pinta el elemento o elementos seleccionados
        for (int i=indice; i<= posFinal; i++){
            fila = i / maxX; //El cociente son las filas
            col  = i % maxX; //El resto son las columnas

            if (fila <= filaFinal && col <= colFinal && col >= colInicial){
                selX = regionPantalla.selX + col*(destw+SEPTHUMB);
                selY = fila*(desth+SEPTHUMB);
                selW = destw+SEPTHUMB*2;
                selH = desth+SEPTHUMB*2;
                //drawRectLine(selX, selY, selX+selW-1, selY+selH-1, SEPTHUMB, color);
                drawRectLine(selX, selY, selW-1, selH-1, SEPTHUMB, color);
            }
        }
        return true;
    }
}

/**
*
*/
bool Ioutil::drawImg(ImagenGestor *imgGestor){
     try{
         SDL_Surface *mySurface = NULL;
         loadImgFromFile(imgGestor->getRuta().c_str(), &mySurface); //Se carga la imagen
         if (mySurface != NULL) {
             return updateImgScr(mySurface,screen, imgGestor);
         } else {
             return false;
         }
     } catch (Excepcion &e) {
         Traza::print("Excepcion Ioutil::drawImg:" + string(e.getMessage()), W_ERROR);
         return false;
     }
}

/**
*
*/
bool Ioutil::updateImgScr(SDL_Surface * srcSurface, SDL_Surface * dstSurface, ImagenGestor *imgGestor){
    SDL_Rect dstRect = { 0,0,0,0 };
    SDL_Surface *bitmap = NULL;

    bool salida = false;
    //double now = SDL_GetTicks();
    if (imgGestor->isResize()){
        if (redimension(srcSurface,dstSurface, imgGestor, &bitmap)){
            calcRectCent(&dstRect, bitmap->w, bitmap->h, dstSurface->w, dstSurface->h, imgGestor);
            salida = blitImage(bitmap, dstSurface,&dstRect, true);
        } else {
            calcRectCent(&dstRect, srcSurface->w, srcSurface->h, dstSurface->w, dstSurface->h, imgGestor);
            salida = blitImage(srcSurface, dstSurface,&dstRect, true);
        }
        //objTraza->print("Con resize",(int)(SDL_GetTicks()-now));
    } else {
        makeMoveSurface(srcSurface);    //Para que podamos movernos por la imagen si la estamos viendo a pantalla completa
        calcRectCent(&dstRect, srcSurface->w, srcSurface->h, dstSurface->w, dstSurface->h, imgGestor);
        salida = blitImage(srcSurface, dstSurface,&dstRect, true);
        //objTraza->print("Sin resize",(int)(SDL_GetTicks()-now));
    }
    return salida;
}

/**
* calculamos el rectangulo para que quede centrada la imagen
*/
void Ioutil::calcRectCent( SDL_Rect *rectCentrado, int srcW, int srcH, int dstW, int dstH, ImagenGestor *imgGestor){

    int bordesX = imgGestor->getBordeLeft() + imgGestor->getBordeRight();
    int bordesY = imgGestor->getBordeTop() + imgGestor->getBordeBottom();

    int centrox = (dstW - (int)srcW + bordesX)/2;
    int centroy = (dstH - (int)srcH + bordesY)/2;

    rectCentrado->x=centrox;
    rectCentrado->y=centroy;
    rectCentrado->w=dstW;
    rectCentrado->h=dstH;

    rectCentrado->x -= imgGestor->getLeftDif();
    rectCentrado->y -= imgGestor->getTopDif();

    if (srcW < dstW){
        //Si el ancho de la imagen cabe en pantalla no necesitamos moverla.
        //La devolvemos al centro de la pantalla
        if (rectCentrado->x > 0)
            rectCentrado->x = centrox;
    } else {
        //Si la imagen no cabe, intentamos que no sobrepase los limites de la pantalla
        if (rectCentrado->x >= 0){
            rectCentrado->x = 0;
            imgGestor->setLeftDif(centrox);
        } else if (rectCentrado->x <= dstW - srcW){
            rectCentrado->x = dstW - srcW;
            imgGestor->setLeftDif(centrox + (srcW - dstW) );
        }
    }

    if (srcH < dstH){
        //Si el alto de la imagen cabe en pantalla no necesitamos moverla.
        //La devolvemos al centro de la pantalla
        if (rectCentrado->y > 0)
            rectCentrado->y = centroy;
    } else {
        //Si la imagen no cabe, intentamos que no sobrepase los limites de la pantalla
        if (rectCentrado->y >= 0){
            rectCentrado->y = 0;
            imgGestor->setTopDif(centroy);
        } else if (rectCentrado->y <= dstH - srcH){
            rectCentrado->y = dstH - srcH;
            imgGestor->setTopDif(centroy + (srcH - dstH) );
        }
    }
}

/**
* Devuelve un nuevo surface en base a un contenedor
* *** LIBERA LA MEMORIA DEL SURFACE PASADO POR PARAMETRO
*
* Si redimensiona la imagen, la deja en el puntero destino y devuelve true.
* Si no la redimensiona, solo devuelve false y deja tal cual la variable srcSurface
*/
bool Ioutil::redimension(SDL_Surface *srcSurface, SDL_Surface *dstSurface, ImagenGestor *imgGestor,  SDL_Surface **destino){
    float i_relacion = 1;
    int h_destino = dstSurface->h - imgGestor->getBordeTop() - imgGestor->getBordeBottom();
    int w_destino = dstSurface->w - imgGestor->getBordeLeft() - imgGestor->getBordeRight();

    //i_relacion = relacion(srcSurface,dstSurface->h,dstSurface->w, imgGestor);//Calcula cuanto hay que reducir la imagen para que quepa por pantalla
    i_relacion = relacion(srcSurface,h_destino,w_destino, imgGestor);//Calcula cuanto hay que reducir la imagen para que quepa por pantalla

    if (i_relacion != 1. && i_relacion != 0.){
        if (imgGestor->isBestfit()){
            *destino = rotozoomSurfaceXY(srcSurface, 0., 1/imgGestor->getZoomX(), 1/imgGestor->getZoomY(), imgGestor->isSmooth());
        } else {
            *destino = rotozoomSurface(srcSurface, 0., 1/i_relacion, imgGestor->isSmooth());
        }
        SDL_FreeSurface(srcSurface);

        if (destino == NULL){
            imgGestor->setZoom(0);
        }
        return true;
    } else {
        return false;
    }
}

/**
* Devuelve la relacion de una imagen para hacerla mas pequenya o mas grange
* Si isBestfit=true intenta aprovechar toda la pantalla disponible aunque la imagen sea
* mas pequenya
*/
float Ioutil::relacion(SDL_Surface *source, int alto, int ancho, ImagenGestor *imgGestor)
{
   int destWidth = ancho;
   int destHeight = alto;
   float nPercent = 0;

   imgGestor->setZoomX(((float)source->w / (float)destWidth));
   imgGestor->setZoomY(((float)source->h / (float)destHeight));

   if (imgGestor->getZoomY() > imgGestor->getZoomX())
      nPercent = imgGestor->getZoomY();
   else
      nPercent = imgGestor->getZoomX();

   Traza::print("nPercent",(int)nPercent, W_DEBUG);
   Traza::print("zoom",(int)imgGestor->getZoom(), W_DEBUG);

   if (nPercent + imgGestor->getZoom() < 0.5){
        imgGestor->setZoom(0.5 - nPercent);
        nPercent = 0.5; //No podemos pasar del doble del tamanyo de la imagen
   } else {
        nPercent += imgGestor->getZoom();
   }

   if (!imgGestor->isBestfit() && nPercent < 1. && imgGestor->getZoom() == 0) nPercent = 1.;

   return nPercent;
}

/**
* Redimensionamos una superficie al tamanyo que queramos en base a la dimension
* relacion y la almacena en destino
*/
void Ioutil::ResizeSurface(float relacion, SDL_Surface *varBitmap, SDL_Surface **destino)
{
    if (varBitmap != NULL && relacion != 0.){
        const int newWidth =  (int)(varBitmap->w / relacion);
        const int newHeight = (int)(varBitmap->h / relacion);
        const int maxi = newHeight-1;
        const int maxj = newWidth-1;
        *destino = SDL_CreateRGBSurface( SURFACE_MODE, newWidth, newHeight, varBitmap->format->BitsPerPixel,
                                        varBitmap->format->Rmask,varBitmap->format->Gmask,varBitmap->format->Bmask,
                                        varBitmap->format->Amask);

        if (*destino != NULL){
            if ( SDL_MUSTLOCK(screen) ) {
                if ( SDL_LockSurface(screen) < 0 ) {
                    Traza::print("Can't lock screen:" + string(SDL_GetError()), W_ERROR);
                }
            }

            //Calculamos el array de los pixeles
            for (int ti = 0; ti <= maxi; ti++)
                for (int tj = 0; tj <= maxj; tj++)
                    putpixel(*destino, tj, ti,  getpixel(varBitmap,(int)(tj * relacion),(int)(ti * relacion)));

            if ( SDL_MUSTLOCK(screen) ) {
                SDL_UnlockSurface(screen);
            }
        } else {
            Traza::print("Ioutil::ResizeSurface:" + string(IMG_GetError()), W_ERROR);
        }
    }
}


/**
* *** ATENCION: LIBERA LA MEMORIA DEL SURFACE PASADO POR PARAMETRO
*/
bool Ioutil::blitImage(SDL_Surface *src, SDL_Surface *dst, SDL_Rect *dstRect, bool freesrc){
    if (src != NULL){
       SDL_BlitSurface(src, NULL, dst, dstRect);
       if (freesrc){
            SDL_FreeSurface(src);
            src = NULL;
       }

       return true;
    } else {
        //drawTextCent("Se ha producido algun error al cargar la imagen",0,0,true, true,0,0,0);
        Traza::print("Error: blitImage:: Se ha producido algun error al cargar la imagen:" + string(IMG_GetError()), W_ERROR);
        return false;
    }
}

/**
* throws Excepcion
*/
void Ioutil::loadImgFromFile(const char *uri, SDL_Surface **destino){
    this->loadFromFile(uri); //Almacena el fichero en la memoria para cargarlo despues
    loadImgFromMem(this->getFile(), this->getFileSize(), destino);
}

/**
*
*/
void Ioutil::loadImgFromMem(char *fileArray, int size, SDL_Surface **destino){
    /*
    IMG_Load_RW(SDL_RWops *src, int freesrc);
    src - The source SDL_RWops as a pointer. The image is loaded from this.
    freesrc - A non-zero value mean is will automatically close/free the src for you.
    */

    if (moveSurface != NULL){
        SDL_FreeSurface(moveSurface);
        moveSurface = NULL;
    }
    SDL_RWops *tempSurfWop = SDL_RWFromMem(fileArray,size);

    if (VIEWALPHA){
        SDL_Surface *memImg = IMG_Load_RW(tempSurfWop,0);
        SDL_FreeRW(tempSurfWop);
        if (memImg == NULL) throw Excepcion(EFIO);
        *destino = SDL_DisplayFormatAlpha(memImg);
        SDL_FreeSurface(memImg);
    } else {
        *destino = IMG_Load_RW(tempSurfWop,0);
        if (destino == NULL) throw Excepcion(EFIO);
    }
}

/**
*
*/
void Ioutil::drawUITitleBorder(const char *title, int sizeBorder, t_color colorBorder){
    //Dibujando el titulo
    loadFont(TITLEFONTSIZE);
    drawText(title,sizeBorder + TITLESPACE, sizeBorder - TITLEFONTSIZE/2,colorBorder);

    int pixelTitle = 0;
    TTF_SizeText(this->font,title,&pixelTitle,NULL );
    const int tamTitle = sizeBorder + 10 + pixelTitle;

    pintarLinea(sizeBorder,sizeBorder,sizeBorder + TITLESPACE/2,sizeBorder,colorBorder);
    pintarLinea(sizeBorder+1,sizeBorder+1,sizeBorder + TITLESPACE/2,sizeBorder+1,cBlanco);

    pintarLinea(sizeBorder,sizeBorder,sizeBorder,screen->h -sizeBorder,colorBorder);
    pintarLinea(sizeBorder+1,sizeBorder+1,sizeBorder+1,screen->h -sizeBorder -1,cBlanco);

    pintarLinea(sizeBorder,screen->h -sizeBorder,screen->w -sizeBorder,screen->h -sizeBorder,colorBorder);
    pintarLinea(sizeBorder,screen->h -sizeBorder+1,screen->w -sizeBorder,screen->h -sizeBorder+1,cBlanco);

    pintarLinea(screen->w -sizeBorder,sizeBorder,screen->w -sizeBorder,screen->h -sizeBorder,colorBorder);
    pintarLinea(screen->w -sizeBorder+1,sizeBorder,screen->w -sizeBorder+1,screen->h -sizeBorder+1,cBlanco);

    pintarLinea(tamTitle + sizeBorder,sizeBorder,screen->w -sizeBorder,sizeBorder,colorBorder);
    pintarLinea(tamTitle + sizeBorder,sizeBorder+1,screen->w -sizeBorder-1,sizeBorder+1,cBlanco);

    loadFont(FONTSIZE);
}

/**
*
*/
void Ioutil::drawUITitleBorder(const char *title){
    t_color color = {128,128,128};
    drawUITitleBorder(title,TITLEBORDER, color);
}

/**
*
*/
void Ioutil::drawUIPanel(Object *obj){
    if (obj->isVisible()){
        drawRectAlpha(obj->getX(), obj->getY(), obj->getW(), obj->getH() , cNegro, obj->getAlpha());
    }
}

/**
*
*/
void Ioutil::drawUIPicture(Object *obj){
    if (obj->isVisible()){
        int x_ = obj->getX();
        int y_ = obj->getY();
        int w_ = obj->getW();
        int h_ = obj->getH();

        if (!obj->getImgDrawed()){
            pintarContenedor(x_,y_,w_,h_,obj->isFocus() && obj->isEnabled(), obj);
            if (obj->getImgGestor() != NULL){
                drawImgObj(obj);
                if (obj->getAlpha() >= 0)
                    drawRectAlpha(x_+INPUTBORDER,y_+INPUTBORDER,w_-INPUTBORDER,h_-INPUTBORDER,cInputContent, obj->getAlpha()); // Difumino la imagen
            }
            cachearObjeto(obj);
        } else {
            cachearObjeto(obj);
        }

    }
}

/**
*
*/
void Ioutil::cachearObjeto(Object *obj){
    if (obj->isVisible()){
        int borde = 1;

        if (obj->showShadow()){
            borde += BORDERSELECTION;
        }

        SDL_Rect imgLocation = { (short int)obj->getX() , (short int)obj->getY(), (short unsigned int)(obj->getW() + borde), (short unsigned int)(obj->getH() + borde) };
        if (!obj->getImgDrawed()){
            //Reseteamos el surface
            obj->getImgGestor()->setSurface(NULL);
            //Guardamos la imagen en el objeto
            takeScreenShot(&obj->getImgGestor()->surface, imgLocation);
            //Indicamos que ya hemos pintado la imagen
            obj->setImgDrawed(true);
        } else {
            printScreenShot(&obj->getImgGestor()->surface, imgLocation);
        }
    }
}

/**
*
*/
void Ioutil::drawUIPopupFondo(Object *obj, int borde){
    if (obj->isVisible()){
        int x = obj->getX();
        int y = obj->getY();
        int w = obj->getW();
        int h = obj->getH();
        if (w > borde*2 && h > borde*2)
            drawRectAlpha(x + borde, y + borde, w - borde*2, h-borde*2, cNegro, 128);
    }
}

/**
*
*/
void Ioutil::drawUILetraPopup(Object *obj){

    UIList *listObj = (UIList *)dynamic_cast<UIList*>(obj);

    if (listObj->isVisible()){

        int x = listObj->getX();
        int y = listObj->getY();
        int w = listObj->getW();
        int h = listObj->getH();
        const int borde = 20;
        int xCent = (x + listObj->getW()) / 2;
        int yCent = (y + listObj->getH()) / 2;
        int radio = h/2 - borde;
        if (w/2 - borde < radio)
            radio = w/2 - borde;
        int bordeLetra = 40;

        radio = radio - bordeLetra;


        //Draw_Circle(screen, xCent + x/2, yCent + y/2, radio, SDL_MapRGB(screen->format, 0,0,0));
//        Draw_FillCircle(screen, xCent + x/2, yCent + y/2, radio, SDL_MapRGB(screen->format, 0,0,0));



        const unsigned int numAngles = 26;
        const int sepLetras = 10;
        float minAngle = 2 * PI / (float)numAngles;
        float angle = 0;
        int xCircle = xCent + x/2;
        int yCircle = yCent + y/2;
        int h1 = fontHeight/2;
        int w1 = fontStrLen("M")/2;
        const char *letra ;
        Uint32 azulOscuro = SDL_MapRGB(screen->format, 50,104,202);

        for (unsigned int i = 0; i < numAngles; i++){
            angle = minAngle * i;
            //Draw_Line(screen, xCircle, yCircle, xCircle + radio * cos(angle), yCircle - radio * sin(angle), gris);

            if (i + 65 == listObj->getLetraPopup()){
                for (float alpha = angle; alpha < minAngle * (i+1); alpha += 0.0001 ){
                    Draw_Line(screen, xCircle, yCircle, xCircle + radio * cos(alpha), yCircle - radio * sin(alpha), azulOscuro);
                }
            }

            letra = Constant::TipoToStr((char)(65+i)).c_str();
            drawText(letra,
                     xCircle - w1 + (radio + sepLetras) * cos(angle + minAngle/2),
                     yCircle - h1 - (radio + sepLetras) * sin(angle + minAngle/2),
                     cBlanco);
        }

        letra = Constant::TipoToStr((char)listObj->getLetraPopup()).c_str();
        loadFont(FONTSIZE*5);
        int xCentChar = xCent - (fontStrLen(letra) / 2);
        int yCentChar = yCent - (fontHeight / 2);
        //Draw_FillCircle(screen, xCent + x/2, yCent + y/2, fontHeight / 2, SDL_MapRGB(screen->format, 0,0,0));
        //pintarSemiCirculo(xCent + x/2, yCent + y/2, fontHeight / 2, cNegro, minAngle * (listObj->getLetraPopup() - 65));
        //pintarCirculo(xCent + x/2, yCent + y/2, radio, cNegro);
        drawText(letra, xCentChar , yCentChar , cBlanco);
        loadFont(FONTSIZE);
    }
}

/**
*
*/
void Ioutil::drawUIPopupMenu(Object *obj){
     if (obj->isVisible()){
        int x = obj->getX();
        int y = obj->getY();
        int w = obj->getW();
        int h = obj->getH();

        UIPopupMenu *listObj = (UIPopupMenu *)obj;

        if (!listObj->getImgDrawed()){
            int centeredY = (Constant::getMENUSPACE() - fontHeight) / 2;
            Traza::print("Repintando popup: " + listObj->getLabel(), W_PARANOIC);
            Traza::print("Alto del popup: ", h, W_PARANOIC);
            pintarContenedor(x,y,w,h, listObj->isFocus() && listObj->isEnabled(), obj);
            x += INPUTCONTENT;
            y += INPUTCONTENT;

            t_color colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
            int cont = 0;
            if (listObj->getObjectType() == GUIPOPUPMENU && listObj->getSize() > 0){
                int icono = -1;
                //Ponemos el color por defecto para las listas
                colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
                TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
                Traza::print("listObj->getPosIniLista(): ", listObj->getPosIniLista(), W_DEBUG);
                Traza::print("listObj->getPosFinLista(): ", listObj->getPosFinLista(), W_DEBUG);

                for (unsigned int i=listObj->getPosIniLista(); i <= listObj->getPosFinLista(); i++ ){
                    Traza::print("pintando: " + listObj->getListNames()->get(i), W_DEBUG);
                    icono = listObj->getListIcons()->get(i);
                    if (i == listObj->getPosActualLista()){
                        drawRectAlpha(x - INPUTCONTENT, y + cont*Constant::getMENUSPACE(), listObj->getW(), Constant::getMENUSPACE(), cAzulOscuro, listObj->isFocus() ? 255 : 128);
                        colorText = cBlanco;
                        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
                        drawText(listObj->getListNames()->get(i).c_str() , x + ((icono >= 0) ? ICOSPACE : 0), y + centeredY + cont*Constant::getMENUSPACE(), colorText);
                        colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
                        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
                    } else {
                        drawText(listObj->getListNames()->get(i).c_str() , x + ((icono >= 0) ? ICOSPACE : 0), y + centeredY + cont*Constant::getMENUSPACE(), colorText);
                    }

                    //pintarLinea(x + ((icono >= 0) ? ICOSPACE : 0), y + (cont+1)*Constant::getMENUSPACE(), w - ((icono >= 0) ? ICOSPACE : 0),  y + (cont+1)*Constant::getMENUSPACE(), cSeparator);
                    pintarLinea(x + ((icono >= 0) ? ICOSPACE : 0), y + (cont+1)*Constant::getMENUSPACE(), w,  y + (cont+1)*Constant::getMENUSPACE(), cSeparator);

                    if (icono >= 0){
                        drawIco(icono, x, y + centeredY + cont*Constant::getMENUSPACE());
                    }

                    cont++;
                }
            }
            cachearObjeto(obj);
        } else {
            //Traza::print("Alto de la lista: ", h, W_DEBUG);
            cachearObjeto(obj);
        }
    }
}

/**
*
*/
void Ioutil::drawUIListBox(Object *obj){
    if (obj->isVisible()){
        int x = obj->getX();
        int y = obj->getY();
        int w = obj->getW();
        int h = obj->getH();

        UIList *listObj = (UIList *)obj;

        if (listObj->isShowLetraPopup()){
                //Forzamos para que se repinte lo ultimo que habia por pantalla
                if (!listObj->getImgDrawed()){
                    listObj->setImgDrawed(true);
                }
                cachearObjeto(obj);
                //Pintamos el fondo traslucido para darle un efecto de ventana emergente
                //y lo volvemos a cachear una sola vez
                if (listObj->getBgLetraPopup() == false){
                    listObj->setBgLetraPopup(true);
                    listObj->setImgDrawed(false);
                    drawUIPopupFondo(obj, 20);
                    cachearObjeto(obj);
                }
                //Llamamos a la funcion que dibuja la seleccion de letras
                drawUILetraPopup(obj);
        } else if (!listObj->getImgDrawed()){
            int centeredY = (Constant::getMENUSPACE() - fontHeight) / 2;
            listObj->setBgLetraPopup(false);
            Traza::print("Repintando lista: " + listObj->getLabel(), W_PARANOIC);
            Traza::print("Alto de la lista: ", h, W_PARANOIC);
            pintarContenedor(x,y,w,h,listObj->isFocus() && listObj->isEnabled(), obj);
            x += INPUTCONTENT;
            y += INPUTCONTENT;

            t_color colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
            int cont = 0;
            if (listObj->getObjectType() == GUILISTBOX && listObj->getSize() > 0){
                int icono = -1;

                //Ponemos el color por defecto para las listas
                colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
                TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
                Traza::print("listObj->getPosIniLista(): ", listObj->getPosIniLista(), W_PARANOIC);
                Traza::print("listObj->getPosFinLista(): ", listObj->getPosFinLista(), W_PARANOIC);

                for (unsigned int i=listObj->getPosIniLista(); i <= listObj->getPosFinLista(); i++ ){
                    Traza::print("pintando: " + listObj->getListNames()->get(i), W_PARANOIC);
                    icono = listObj->getListIcons()->get(i);
                    if (i == listObj->getPosActualLista()){
                        drawRectAlpha(x - INPUTCONTENT, y + cont*Constant::getMENUSPACE(), listObj->getW(), Constant::getMENUSPACE(), cAzulOscuro, listObj->isFocus() ? 255 : 128);
                        colorText = cBlanco;
                        TTF_SetFontStyle(font, TTF_STYLE_BOLD);
                        drawText(listObj->getListNames()->get(i).c_str() , x + ((icono >= 0) ? ICOSPACE : 0), y + centeredY + cont*Constant::getMENUSPACE(), colorText);
                        colorText = listObj->isEnabled() && listObj->isFocus() ? cNegro : cGris;
                        TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
                    } else {
                        drawText(listObj->getListNames()->get(i).c_str() , x + ((icono >= 0) ? ICOSPACE : 0), y + centeredY + cont*Constant::getMENUSPACE(), colorText);
                    }

                    pintarLinea(x + ((icono >= 0) ? ICOSPACE : 0), y + (cont+1)*Constant::getMENUSPACE(), w - ((icono >= 0) ? ICOSPACE : 0),  y + (cont+1)*Constant::getMENUSPACE(), cSeparator);

                    if (icono >= 0){
                        drawIco(icono, x, y + centeredY + cont*Constant::getMENUSPACE());
                    }

                    if (listObj->getPosIniLista() > 0){
                        pintarTriangulo (listObj->getX() + listObj->getW() - 6,listObj->getY() + INPUTCONTENT, 8, 8, true, colorText);
                    }

                    if (listObj->getPosFinLista() + 1 < (unsigned int)listObj->getSize()){
                        pintarTriangulo (listObj->getX() + listObj->getW() - 6,listObj->getY() + listObj->getH() - INPUTCONTENT, 8, 8, false, colorText);
                    }
                    cont++;
                }
            }
            cachearObjeto(obj);
        } else {
            Traza::print("Alto de la lista: ", h, W_PARANOIC);
            cachearObjeto(obj);
        }
    }
}

/**
*
*/
void Ioutil::drawUIInputWide(Object *obj){
    if (obj->isVisible()){
        int x = obj->getX();
        int y = obj->getY();
        int w = obj->getW();
        int h = obj->getH();

        //Dibujando el inputText
        pintarContenedor(x,y,w,h,obj->isFocus() && obj->isEnabled(), obj);
        //Dibujando el label del input
        drawText(obj->getLabel().c_str(),x,y-FONTSIZE-4,cAzulOscuro);

        x += INPUTCONTENT;
        y += INPUTCONTENT;

        bool activo = true;
        static long before = SDL_GetTicks();

        if (obj->isFocus()){
            long now = SDL_GetTicks();
            if ((now - before) < 250){
                activo = true;
            } else if ((now - before) < 500 && (now - before) > 250){
                activo = false;
            } else if ((now - before) > 500){
                before = now;
            }
        } else {
            activo = false;
        }

        t_color colorText = obj->isEnabled() ? cNegro : cGris;
        tInput myInput = calculaTextoInput(obj);
        drawTextInArea(myInput.texto.c_str(), x, y, colorText, NULL);

        if (activo && obj->isEnabled())
            drawText("|", myInput.cursorX, myInput.cursorY, colorText);

    }
}

/**
* El sizeInput es el tamanyo en pixels del input
*/
tInput Ioutil::calculaTextoInput(Object *obj){

    UIInput *objInput = (UIInput *)obj;

    int sizeInput = objInput->getW() - 2*INPUTCONTENT;
    tInput dato;                       //Devuelve el contenido que se mostrara en el input
    string text = objInput->getText();   //Texto completo
    unsigned int totalTexto = text.length();
    unsigned int offset = objInput->getOffset(); //Establece la primera letra que se pinta en el input
    unsigned int tamText = text.length() - offset;   //Establece el tamanyo del texto que se va a pintar
    int textSizeResult = 0;                 //Se usa para calcular el tamanyo del texto que podemos mostrar para que quepa en el input
    unsigned int posFin = 0;                //Establece la ultima letra que estamos pintando en el input

    dato.cursorX = objInput->getX() + INPUTCONTENT;
    dato.cursorY = objInput->getY() + INPUTCONTENT;

    //Si el cursor esta en una letra que no se muestra por pantalla, forzamos el offset
    //a ir a la posicion del cursor. Luego recalcularemos la ultima letra que mostramos
    //por pantalla en el bucle siguiente
    if (objInput->getPosChar() < offset){
        offset = objInput->getPosChar();
    }

    do{
        //Calculamos el resto del texto que cabe en el input a partir de la posicion
        //de la letra que indica la variable offset
        if (offset < totalTexto){
            dato.texto = text.substr(offset, tamText);
            while ((textSizeResult = fontStrLen(dato.texto)) > sizeInput && tamText > 0){
                tamText--;
                dato.texto = text.substr(offset, tamText);
            }
        }
        //Calculamos cual es la posicion de la ultima letra que hemos pintado
        posFin = offset + tamText;
        //En el caso de que la posicion del cursor del input este mas alejado de la
        //ultima letra que pintamos, debemos mostrar una letra mas del principio del texto
        if (objInput->getPosChar() > posFin){
            offset++;
        }
        //Si hemos movido el offset por la condicion anterior, debemos volver a recalcular el final
        //Por eso realizamos un do...while
    }while (objInput->getPosChar() > posFin && offset < totalTexto);

    //Calculamos la posicion del cursor
    if (posFin == objInput->getPosChar()){
        dato.cursorX += textSizeResult;
    } else if (objInput->getPosChar() > offset && objInput->getPosChar() < totalTexto){
        dato.cursorX += fontStrLen(text.substr(offset, objInput->getPosChar() - offset));
    }

    //Guardamos la posicion del offset en el objeto
    objInput->setOffset(offset);
    return dato;
}

/**
*
*/
void Ioutil::drawUIProgressBar(Object *obj){

    UIProgressBar *objProg = (UIProgressBar *)obj;
    if (obj->isVisible()){
        int x = obj->getX();
        int y = obj->getY();
        int w = obj->getW();
        int h = obj->getH();

        pintarContenedor(x,y,w,h,objProg->isFocus() && objProg->isEnabled(), obj);

        if (w > 0 && h > 0){
            int wsel = (objProg->getProgressPos() / (float)objProg->getProgressMax()) * w;
            if (wsel > 0)
                drawRect(x+INPUTBORDER,y+INPUTBORDER,wsel,h-INPUTBORDER,cAzul); // Dibujo el contenedor
        }
    }
}


/**
*
*/
void Ioutil::drawUILabel(Object *obj){
    if (obj->isVisible()){
        int textx = obj->getX();
        int texty = obj->getY();
        int txtcent = 0;

        if (obj->isCentered()){
            txtcent = fontStrLen(obj->getLabel()) / 2;
        }

        drawText(obj->getLabel().c_str(), textx - txtcent, texty, cBlanco);
    }
}

/**
*
*/
void Ioutil::drawUIButton(Object *obj){
    if (obj->isVisible()){
        SDL_Rect imgLocation = { (short int)obj->getX() , (short int)obj->getY(), (short unsigned int)obj->getW(), (short unsigned int)obj->getH() };

        if (!obj->getImgDrawed() || !obj->isVerContenedor()){
            int textx = obj->getX();
            int texty = obj->getY();
            int txtcentY = (obj->getH() - fontHeight)/2;
            int txtcentX = (obj->getW() - fontStrLen(obj->getLabel())) / 2;
            int icocentY = (obj->getH() - FAMFAMICONH) / 2;

            if (txtcentX < 0 || fontStrLen(obj->getLabel()) + txtcentX + 5 >= obj->getW()  ) txtcentX = 0;
            if (txtcentY < 0) txtcentY = 0;
            if (icocentY < 0) icocentY = 0;

            if (obj->isVerContenedor()){
                textx += INPUTCONTENT;
                //Se pinta el fondo del boton. Para los botones es una imagen que hemos guardado en el gestor de iconos
                if (obj->isFocus()){
                    gestorIconos->drawIcono(boton_selected, screen, 0, obj->getX(), obj->getY(), obj->getW(), obj->getH());
                } else {
                    gestorIconos->drawIcono(boton, screen, 0, obj->getX(), obj->getY(), obj->getW(), obj->getH());
                }
                //Pintamos el texto del boton
                drawText(obj->getLabel().c_str(), textx + txtcentX, texty + txtcentY, cBlanco);
            }

            //Pintamos el icono del boton
            drawIco(obj->getIcon(), obj->isVerContenedor() ? textx + 3 : textx, texty + icocentY);

            if (obj->isVerContenedor()){
                    //Reseteamos el surface
                    obj->getImgGestor()->setSurface(NULL);
                    //Guardamos la imagen en el objeto
                    takeScreenShot(&obj->getImgGestor()->surface, imgLocation);
                    //Indicamos que ya hemos pintado la imagen
                    obj->setImgDrawed(true);

            }
        } else {
            printScreenShot(&obj->getImgGestor()->surface, imgLocation);
        }
    }
}

/**
*
*/
void Ioutil::showMessage(string mensaje, unsigned long delay){
    const int borde = 20;
//    SDL_Surface *pointerSurface = NULL;
//    if (pointerSurface == NULL){
//        takeScreenShot(&pointerSurface);
//    }
//    printScreenShot(&pointerSurface);
    drawRectAlpha(borde, borde, this->w - 2*borde , this->h - 2*borde , cNegro, 200);
    t_color color = {128,128,128};
    drawUITitleBorder("Aviso",borde + 10, color);
    drawTextCent(mensaje.c_str(),0,0,true, true, cBlanco);
    flipScr();
    SDL_Delay(delay);
}

/**
*
*/
void Ioutil::setAutoMessage(string var){
    if (mensajeAuto == "" ) {
        mensajeAuto = var;
    }
}

/**
*
*/
void Ioutil::showAutoMessage(){
    static int time = 2 * FRAMELIMIT; //2 segundos
    static SDL_Surface *pointerSurface = NULL;

    if(mensajeAuto != "" && time >= 0){
        if (mensajeAuto != ""){
            int pixelDato = 0;
            TTF_SizeText(this->font,mensajeAuto.c_str(),&pixelDato,NULL );

            if (pointerSurface == NULL){
                takeScreenShot(&pointerSurface);
            }

            drawRect( 0, 0, pixelDato, FONTSIZE+2, cNegro);
            drawText(mensajeAuto.c_str(),0,0,cBlanco);
            SDL_UpdateRect(screen, 0, 0, pixelDato, FONTSIZE+2);
        }

        if (time > 0){
            time--;
        } else {
            int pixelDato = 0;
            TTF_SizeText(this->font,mensajeAuto.c_str(),&pixelDato,NULL );

            if (pointerSurface != NULL){
                SDL_Rect textLocation = {0, 0, (short unsigned int)pixelDato, (short unsigned int)(FONTSIZE+2)};
                SDL_BlitSurface(pointerSurface, &textLocation, screen, &textLocation);
                SDL_UpdateRect(screen, 0, 0, pixelDato, FONTSIZE+2);
                SDL_FreeSurface(pointerSurface);
                pointerSurface = NULL;
            }
            time = 2 * FRAMELIMIT; //2 segundos
            mensajeAuto = "";
        }
    }
}


/**
*
*/
void Ioutil::showCheck(Object *obj){

    int x = obj->getX();
    int y = obj->getY();

    pintarContenedor(x,y,CHECKW, CHECKH, obj->isFocus(), obj);
    //Escribir el label del input text
    drawText(obj->getLabel().c_str(),x+5+CHECKW,y,cNegro);

    if (obj->isChecked())
        drawRect(x+2,y+2,CHECKW-4,CHECKH-4,cAzul); // Dibujo el input como seleccionado o no
}

/**
*
*/
int Ioutil::fontStrLen(string str){
    int pixelSize = 0;
    TTF_SizeText(this->font,str.c_str(),&pixelSize,NULL );
    return pixelSize;
}

/**
*
*/
int Ioutil::fontStrLenCent(string str)
{
    return -1*fontStrLen(str)/2;
}

/**
 * Return the pixel value at (x, y)
 * NOTE: The surface must be locked before calling this!
 */
inline Uint32 Ioutil::getpixel(SDL_Surface *surface, const int x, const int y)
{
    //int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;

    switch(surface->format->BytesPerPixel) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

/**
* Hace lo mismo que putpixel pero comprobando que no se superen las dimensiones de la pantalla
*/
inline void Ioutil::putpixelSafe(SDL_Surface *surface, const int x, const int y, const Uint32 pixel){

    if (x < this->w && y < this->h && x >= 0 && y >= 0)
        putpixel(surface, x, y, pixel);
//    else
//        Traza::print("Pintando en zona no segura: " + Constant::TipoToStr(x) + "; " + Constant::TipoToStr(y)+ "; w: " +  Constant::TipoToStr(this->w) + "; h: " +  Constant::TipoToStr(this->h), W_DEBUG);

}

inline void Ioutil::putpixel(SDL_Surface *surface, const int x, const int y, const Uint32 pixel)
{
     Draw_Pixel(surface, x,y, pixel);
}
///**
// * Set the pixel at (x, y) to the given value
// * NOTE: The surface must be locked before calling this!
// */
//inline void Ioutil::putpixel(SDL_Surface *surface, const int x, const int y, const Uint32 pixel)
//{
//    //int bpp = surface->format->BytesPerPixel;
//    /* Here p is the address to the pixel we want to set */
//    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * surface->format->BytesPerPixel;
//
//    switch(surface->format->BytesPerPixel) {
//    case 1:
//        *p = pixel;
//        break;
//
//    case 2:
//        *(Uint16 *)p = pixel;
//        break;
//
//    case 3:
//        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
//            p[0] = (pixel >> 16) & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[2] = pixel & 0xff;
//        } else {
//            p[0] = pixel & 0xff;
//            p[1] = (pixel >> 8) & 0xff;
//            p[2] = (pixel >> 16) & 0xff;
//        }
//        break;
//
//    case 4:
//        *(Uint32 *)p = pixel;
//        break;
//    }
//}

/**
*
*/
void Ioutil::pintarCirculo (int n_cx, int n_cy, int r, t_color color)
{
//    int centro_x=0;
//    int centro_y=0;
//
//    int x_izq=x - r;
//    int x_der=x + r;
//    int y_izq=y - r;
//    int y_der=y + r;
//
//    if (x_izq < 0) x_izq = 0;
//    if (x_der > screen->w) x_der = screen->w - 1;
//    if (y_izq < 0) y_izq = 0;
//    if (y_der > screen->h) y_der = screen->h - 1;
//
//    int raiz=0;
//    int radio2 = r*r;
//
//    int i=x_izq;
//    int j=y_izq;
//
//    Uint32 r_color = SDL_MapRGB(screen->format, color.r,color.g,color.b);
//
//    /* Lock the screen for direct access to the pixels */
//    if ( SDL_MUSTLOCK(screen) ) {
//        if ( SDL_LockSurface(screen) < 0 ) {
//            Traza::print("Can't lock screen:" + string(SDL_GetError()), W_ERROR);
//        }
//    }
//
//    while (i < x_der){
//        while (j < y_der){
//            centro_x = i - x;
//            centro_y = j - y;
//            //Funcion del circulo -> x^2 + y^2 = r^2
//            raiz = centro_x*centro_x + centro_y*centro_y;
//            //el -2 es un factor de corrección necesario para círculos pequenyos, sino queda muy pixelado
//            if (raiz < radio2-4){
//                putpixelSafe(screen,i,j,r_color);
//            }
//            j++;
//        }
//        i++;
//        j=y_izq;
//    }
//
//    if ( SDL_MUSTLOCK(screen) ) {
//        SDL_UnlockSurface(screen);
//    }

 // if the first pixel in the screen is represented by (0,0) (which is in sdl)
    // remember that the beginning of the circle is not in the middle of the pixel
    // but to the left-top from it:

    double error = (double)-r;
    double x = (double)r -0.5;
    double y = (double)0.5;
    double cx = n_cx - 0.5;
    double cy = n_cy - 0.5;
    Uint32 r_color = SDL_MapRGB(screen->format, color.r,color.g,color.b);

    while (x >= y)
    {
        putpixelSafe(screen, (int)(cx + x), (int)(cy + y), r_color);
        putpixelSafe(screen, (int)(cx + y), (int)(cy + x), r_color);

        if (x != 0)
        {
            putpixelSafe(screen, (int)(cx - x), (int)(cy + y), r_color);
            putpixelSafe(screen, (int)(cx + y), (int)(cy - x), r_color);
        }

        if (y != 0)
        {
            putpixelSafe(screen, (int)(cx + x), (int)(cy - y), r_color);
            putpixelSafe(screen, (int)(cx - y), (int)(cy + x), r_color);
        }

        if (x != 0 && y != 0)
        {
            putpixelSafe(screen, (int)(cx - x), (int)(cy - y), r_color);
            putpixelSafe(screen, (int)(cx - y), (int)(cy - x), r_color);
        }

        error += y;
        ++y;
        error += y;

        if (error >= 0)
        {
            --x;
            error -= x;
            error -= x;
        }
    }
}

/*
 * SDL_Surface 32-bit circle-fill algorithm without using trig
 *
 * While I humbly call this "Celdecea's Method", odds are that the
 * procedure has already been documented somewhere long ago.  All of
 * the circle-fill examples I came across utilized trig functions or
 * scanning neighbor pixels.  This algorithm identifies the width of
 * a semi-circle at each pixel height and draws a scan-line covering
 * that width.
 *
 * The code is not optimized but very fast, owing to the fact that it
 * alters pixels in the provided surface directly rather than through
 * function calls.
 *
 * WARNING:  This function does not lock surfaces before altering, so
 * use SDL_LockSurface in any release situation.
 */
void pintarFillCircle(SDL_Surface *surface, int cx, int cy, int radius, Uint32 pixel)
{
    // Note that there is more to altering the bitrate of this
    // method than just changing this value.  See how pixels are
    // altered at the following web page for tips:
    //   http://www.libsdl.org/intro.en/usingvideo.html
    static const int BPP = 4;

    double r = (double)radius;

    for (double dy = 1; dy <= r; dy += 1.0)
    {
        // This loop is unrolled a bit, only iterating through half of the
        // height of the circle.  The result is used to draw a scan line and
        // its mirror image below it.

        // The following formula has been simplified from our original.  We
        // are using half of the width of the circle because we are provided
        // with a center and we need left/right coordinates.

        double dx = floor(sqrt((2.0 * r * dy) - (dy * dy)));
        int x = cx - dx;

        // Grab a pointer to the left-most pixel for each half of the circle
        Uint8 *target_pixel_a = (Uint8 *)surface->pixels + ((int)(cy + r - dy)) * surface->pitch + x * BPP;
        Uint8 *target_pixel_b = (Uint8 *)surface->pixels + ((int)(cy - r + dy)) * surface->pitch + x * BPP;

        for (; x <= cx + dx; x++)
        {
            *(Uint32 *)target_pixel_a = pixel;
            *(Uint32 *)target_pixel_b = pixel;
            target_pixel_a += BPP;
            target_pixel_b += BPP;
        }
    }
}

void Ioutil::pintarSemiCirculo (int x, int y, int r, t_color color, int angle)
{

    int centro_x=0;
    int centro_y=0;

    int x_izq=x - r;
    int x_der=x + r;
    int y_izq=y - r;
    int y_der=y + r;

    if (x_izq < 0) x_izq = 0;
    if (x_der > screen->w) x_der = screen->w - 1;
    if (y_izq < 0) y_izq = 0;
    if (y_der > screen->h) y_der = screen->h - 1;

    int raiz=0;
    int radio2 = r*r;

    int i=x_izq;
    int j=y_izq;

    Uint32 r_color = SDL_MapRGB(screen->format, color.r,color.g,color.b);

    /* Lock the screen for direct access to the pixels */
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            Traza::print("Can't lock screen:" + string(SDL_GetError()), W_ERROR);
        }
    }

    while (i < x_der){
        while (j < y_der){
            centro_x = i - x;
            centro_y = j - y;
            //Funcion del circulo -> x^2 + y^2 = r^2
            raiz = centro_x*centro_x + centro_y*centro_y;
            //el -2 es un factor de corrección necesario para círculos pequenyos, sino queda muy pixelado
            if (raiz < radio2-4){
                putpixelSafe(screen,i,j,r_color);
            }
            j++;
        }
        i++;
        j=y_izq;
    }

    if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }

}

/**
*
*/
void Ioutil::pintarIconoProcesando(Thread<Gestorroms> *thread){
        long delay = 0;
        unsigned long before = 0;
        int grados = 0;
        int x = screen->w/2 - 50, y = screen->h/2 + 20;
        SDL_Rect iconRect = {(short int)x, (short int)y, 50, 50};
        SDL_Rect iconRectFondo = {(short int)x, (short int)y, 100, 100};
        SDL_Surface *mySurface = NULL;
        takeScreenShot(&mySurface, iconRectFondo);
        int limite = FRAMEPERIOD/25;

        while (thread->isRunning()){
            before = SDL_GetTicks();
            printScreenShot(&mySurface, iconRectFondo);
            drawIco(reloj_de_arena, grados, iconRect.x, iconRect.y, iconRect.w, iconRect.h);
            grados= (grados + 5);
            flipScr();
            delay = before - SDL_GetTicks() + limite;
            if(delay > 0) SDL_Delay(delay);
        }

        SDL_FreeSurface(mySurface);
}

/**
*
*/
void Ioutil::pintarTriangulo (int x, int y, int base, int altura, bool isUP, t_color color)
{
    //x e y son la cima del triangulo
    //      (x,y)
    //       /\    -
    //      /  \   |  altura
    //      ----   -
    //      base
    //      |--|

    // a^2 = b^2+c^2
    //
    // Parte derecha => Punto central Base = (x, y - altura)
    //                  vertice derecho = (x + base/2, y - altura)

//    int x_arriba=x - base/2;
    int x_der=x + base/2;
    int y_arriba=y;
    int y_debajo=y + altura;

    int numerador = (y_arriba - y_debajo);
    int denominador = (x - x_der);

    float pendiente = numerador/(float)denominador;

    //Funcion de la recta -> j = (y1-y2)/(x1-x2) * (i-x1) + y1
    int calc_j = 0;
    int baseNuevoPlano = y + altura;

    Uint32 r_color = SDL_MapRGB(screen->format, color.r,color.g,color.b);

	/* Lock the screen for direct access to the pixels */
    if ( SDL_MUSTLOCK(screen) ) {
        if ( SDL_LockSurface(screen) < 0 ) {
            Traza::print("Can't lock screen:" + string(SDL_GetError()), W_ERROR);
        }
    }

    for (int j = y_arriba; j < y_debajo; j++)
    {
        for (int i = x; i < x_der; i++)
        {
            calc_j = (int)(pendiente * (i-x)) + y;

            if (j > calc_j)
            {
                if (!isUP)
                {
                    putpixelSafe(screen,i,2*baseNuevoPlano - j - 2*altura,r_color);
                    putpixelSafe(screen,(2*x - i),2*baseNuevoPlano - j - 2*altura,r_color);
                }
                else
                {
                    putpixelSafe(screen,i,j,r_color);
                    putpixelSafe(screen,(2*x - i),j,r_color);
                }
            }
        }
    }

   	if ( SDL_MUSTLOCK(screen) ) {
        SDL_UnlockSurface(screen);
    }

}

/**
*
*/
void Ioutil::msg_processing(unsigned int posString, unsigned int tam)
{
   if (posString <= tam && tam > 0)
   {
        const char *msg={"Espere un momento porfavor"};
        int sizeMsg = fontStrLen(msg);
        int sizePercent = fontStrLen("100%");

        int pos_v = (this->getHeight() - (2*Constant::getMENUSPACE()))/2;
        //const int radio = 10;
        const int radio = FONTSIZE/2;
        const int posX = 40;
        int ancho = this->getWidth() - 2*posX -radio;
        const int posY = pos_v + FONTSIZE + Constant::getMENUSPACE() + radio;
        char datosProgreso[30];
        SDL_Rect rectTitulo = { (short int)(posX + radio/2-1),(short int)(posY - radio+1), (short unsigned int)ancho, (short unsigned int)(radio*2-1)};

        clearScr(cNegro);
        drawText(msg,(this->getWidth() - sizeMsg)/2,pos_v,cBlanco);

        pintarCirculo(posX, posY , radio, cBlanco);
        pintarCirculo(posX + ancho, posY , radio, cBlanco);
        drawRect(rectTitulo.x,rectTitulo.y,rectTitulo.w,rectTitulo.h,cBlanco);
        ancho = ancho * (int)(posString/(float)tam);

        if (ancho > 0){
            pintarCirculo(posX, posY , radio-1, cAzul);
            pintarCirculo(posX + ancho, posY , radio-1,cAzul);
            SDL_Rect rectTitulo2 = { (short int)(posX + radio/2-1),(short int)(posY - radio+1+1), (short unsigned int)ancho, (short unsigned int)((radio-1)*2-1)};
            drawRect(rectTitulo2.x,rectTitulo2.y,rectTitulo2.w,rectTitulo2.h,cAzul);
        }

        sprintf(datosProgreso,"%2.0f",posString/(float)tam*100);
        strcat(datosProgreso,"%");
        t_color colorTemp;

        if (posString > tam/2){
            colorTemp = cBlanco;
        } else {
            colorTemp = cNegro;
        }

        drawText(datosProgreso,(this->getWidth() - sizePercent)/2,pos_v + FONTSIZE + Constant::getMENUSPACE(),colorTemp);
        flipScr();
   }
}

/**
*
*/
string Ioutil::configButtonsJOY(){
    Traza::print("casoJOYBUTTONS: Inicio", W_PARANOIC);
    bool salir = false;
    string salida = "";

    long delay = 0;
    unsigned long before = 0;
    const char* JoystickButtonsMSG[] = {"Arriba","Abajo","Izquierda","Derecha","Aceptar","Cancelar", "Página anterior", "Página siguiente", "Select", "Buscar elemento"};
    int JoyButtonsVal[] = {JOY_BUTTON_UP, JOY_BUTTON_DOWN, JOY_BUTTON_LEFT, JOY_BUTTON_RIGHT, JOY_BUTTON_A, JOY_BUTTON_B, JOY_BUTTON_L, JOY_BUTTON_R, JOY_BUTTON_SELECT, JOY_BUTTON_R3};
    int tam = 10;
    int i=0;

    do{
        SDL_Event event;
        before = SDL_GetTicks();
        clearScr(cBgScreen);
        drawTextCent(JoystickButtonsMSG[i], 0, 0, true, true, cNegro);
        flipScr();
        if( SDL_PollEvent( &event ) ){
             switch( event.type ){
                case SDL_QUIT:
                    salir = true;
                    break;
                case SDL_JOYBUTTONDOWN :
                    JoyMapper::setJoyMapper(JoyButtonsVal[i], event.jbutton.button);
                    i++;
                    break;
                case SDL_JOYHATMOTION:
                    if (event.jhat.value != 0){ //Solo en el momento del joydown
                        JoyMapper::setJoyMapper(JoyButtonsVal[i], JOYHATOFFSET + event.jhat.value);
                        i++;
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    int normValue;
                    if((abs(event.jaxis.value) > DEADZONE) != (abs(mPrevAxisValues[event.jaxis.which][event.jaxis.axis]) > DEADZONE))
                    {
                        if(abs(event.jaxis.value) <= DEADZONE){
                            normValue = 0;
                        } else {
                            if(event.jaxis.value > 0)
                                normValue = 1;
                            else
                                normValue = -1;
                        }

                        if (normValue != 0){
                            int valor = (abs(normValue) << 4 | event.jaxis.axis) * normValue;
                            JoyMapper::setJoyMapper(JoyButtonsVal[i], JOYAXISOFFSET + valor);
                            i++;
                        }
                    }
                    mPrevAxisValues[event.jaxis.which][event.jaxis.axis] = event.jaxis.value;
                    break;
             }
        }

        if (i == tam){
            salir = true;
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);
    JoyMapper::saveJoyConfig();
    return salida;
}

/**
*
*/
void Ioutil::drawObject(Object *obj){
    switch(obj->getObjectType()){
        case GUIPOPUPMENU:
            drawUIPopupMenu(obj);
            break;
        case GUILABEL:
            drawUILabel(obj);
            break;
        case GUIBUTTON:
            drawUIButton(obj);
            break;
        case GUILISTBOX:
            drawUIListBox(obj);
            break;
        case GUIPICTURE:
            drawUIPicture(obj);
            break;
        case GUIARTSURFACE:
            drawUIArt(obj);
            break;
        case GUIINPUTWIDE:
            drawUIInputWide(obj);
            break;
        case GUICHECK:
            showCheck(obj);
            break;
        case GUIPANELBORDER:
            drawUITitleBorder(obj->getLabel().c_str());
            break;
        case GUIPANEL:
            drawUIPanel(obj);
            break;
        case GUIPROGRESSBAR:
            drawUIProgressBar(obj);
            break;
        default:
            break;
    }
}
