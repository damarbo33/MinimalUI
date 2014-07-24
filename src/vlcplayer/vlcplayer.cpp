#include "vlcplayer.h"

int quit = 0;
int VlcPlayer::isPaused = 0;
int VlcPlayer::VIDEOWIDTH, VlcPlayer::VIDEOHEIGHT,VlcPlayer::WIDTH,VlcPlayer::HEIGHT;
int VlcPlayer::BPP;

SDL_Rect VlcPlayer::rect;

VlcPlayer::VlcPlayer(){
    Traza::print("Constructor de VlcPlayer", W_DEBUG);
    mplayer = NULL;
    BPP = 32;
    surfaceVideo.surf = NULL;
    surfaceVideo.obj = NULL;
}

VlcPlayer::~VlcPlayer(){
}

void * VlcPlayer::lock(void *data, void **p_pixels)
{
    struct ctx *cotx = (ctx *) data;
    SDL_LockMutex(cotx->mutex);
    SDL_LockSurface(cotx->surf);

    *p_pixels = cotx->surf->pixels;
    // VIDEOWIDTH * (BPP/8) --> Es el pitch del video en bytes. Significa el tamanyo en bytes de una linea
    //Desplazamos el puntero para empezar a pintar el video centrado en pantalla
//    for (int i=0; i < VIDEOWIDTH * (BPP/8) * rect.y; i++){
//        *p_pixels++;
//    }
    *p_pixels += (VIDEOWIDTH * (BPP/8) * rect.y);


    return NULL; /* picture identifier, not needed here */
}

void VlcPlayer::unlock(void *data, void *id, void *const *p_pixels)
{
    struct ctx *cotx = (ctx *) data;

    /* VLC just rendered the video, but we can also render stuff */
//    uint16_t *pixels = (uint16_t *) *p_pixels;
//    int x, y;
//
//    for(y = 10; y < 40; y++)
//        for(x = 10; x < 40; x++)
//            if(x < 13 || y < 13 || x > 36 || y > 36)
//                pixels[y * VIDEOWIDTH + x] = 0xffff;
//            else
//                pixels[y * VIDEOWIDTH + x] = 0x0;

//    tmenu_objects *obj;
//    if (cotx->obj != NULL){
//         for (int i=0;i<cotx->obj->getSize();i++){
//            obj = cotx->obj->getByPos(i);
//            drawObj(obj);
//         }
//    }

    SDL_UnlockSurface(cotx->surf);
    SDL_UnlockMutex(cotx->mutex);
    assert(id == NULL); /* picture identifier, not needed here */
}

/**
*
*/
void VlcPlayer::display(void *data, void *id)
{
    /* VLC wants to display the video */
    (void) data;
    assert(id == NULL);
}

/**
*
*/
int VlcPlayer::play(string file)
{

    static char const *vlc_argv[] =
    {
        "--no-xlib", /* tell VLC to not use Xlib */
        #ifdef UNIX
        "--vout"
        ,"omxil_vout",
        #endif // UNIX
    };



    int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);
    //surfaceVideo.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT, 16, 0x001f, 0x07e0, 0xf800, 0);
    surfaceVideo.surf = screen;
    surfaceVideo.mutex = SDL_CreateMutex();

    if(!screen){
        printf("cannot set video mode\n");
        return EXIT_FAILURE;
    }

    string videoFormat = "RV" + Constant::TipoToStr(BPP);


    /**
     *  Initialise libVLC
     */
    libvlc = libvlc_new(vlc_argc, vlc_argv);
    media = libvlc_media_new_path(libvlc, file.c_str());
    mplayer = libvlc_media_player_new_from_media(media);
    libvlc_media_release(media);
    //Llamadas para dar video
    libvlc_video_set_callbacks(mplayer, lock, unlock, display, &surfaceVideo);
    libvlc_video_set_format(mplayer, videoFormat.c_str(), VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH*(BPP/8));
    libvlc_media_player_play(mplayer);

    rect.x = (WIDTH - VIDEOWIDTH) / 2;
    rect.y = (HEIGHT - VIDEOHEIGHT) / 2;
    rect.w = 0;
    rect.h = 0;

    return EXIT_SUCCESS;
}

/**
*
*/
int VlcPlayer::pause(){
    if (mplayer != NULL){
        isPaused = !isPaused;
        if (libvlc_media_player_can_pause(mplayer))
            libvlc_media_player_set_pause(mplayer, isPaused);

    }

    return 0;
}

/**
*
*/
void VlcPlayer::stop(){
    /*
     * Stop stream and clean up libVLC
     */
    if (mplayer != NULL){
        libvlc_media_player_stop(mplayer);
        libvlc_media_player_release(mplayer);
        libvlc_release(libvlc);
    }
}

/**
*
*/
void VlcPlayer::destroy(){
     /*
     * Close window and clean up libSDL
     */
     if (mplayer != NULL){
        SDL_DestroyMutex(surfaceVideo.mutex);
        //SDL_FreeSurface(surfaceVideo.surf);
     }
}

/**
*
*/
void VlcPlayer::blitVideo(){
    /* Blitting the surface does not prevent it from being locked and
     * written to by another thread, so we use this additional mutex. */
    SDL_LockMutex(surfaceVideo.mutex);
    //SDL_BlitSurface(surfaceVideo.surf, NULL, screen, &rect);
    SDL_UnlockMutex(surfaceVideo.mutex);
}

/**
*
*/
void VlcPlayer::salto10PercentForward(){
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        float pos = libvlc_media_player_get_position(mplayer);
        if (pos + 0.1 < 1)
            libvlc_media_player_set_position(mplayer, pos + 0.1);
    }
}

/**
*
*/
void VlcPlayer::salto10PercentBackwards(){
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        float pos = libvlc_media_player_get_position(mplayer);
        if (pos - 0.1 > 0)
            libvlc_media_player_set_position(mplayer, pos - 0.1);
    }
}

long VlcPlayer::getTotalSecs(){
    long res = 0;
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer) >= 0){
        res = libvlc_media_player_get_length(mplayer)/1000;
    }
    return res;
}

/**
*
*/
void VlcPlayer::setPositionPercent(float pos){
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        if (pos >= 0 && pos <= 1)
            libvlc_media_player_set_position(mplayer, pos);
    }
}

float VlcPlayer::getPositionPercent(){
    float res = 0;

    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        res = libvlc_media_player_get_position(mplayer);
    }
    return res;
}

string VlcPlayer::getTimePos(){
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        return Constant::timeFormat(libvlc_media_player_get_time(mplayer)/1000);
    }
    return "";
}

string VlcPlayer::getTimeTotal(){
    if (mplayer != NULL)
    if (libvlc_media_player_is_seekable(mplayer)){
        return Constant::timeFormat(libvlc_media_player_get_length(mplayer)/1000);
    }
    return "";
}

/**
*
*/
void VlcPlayer::setSizeToVideo(){
    unsigned int ancho;
    unsigned int alto;

    libvlc_video_get_size(mplayer, 0, &ancho, &alto);
    Traza::print("Tam Original del video: " + Constant::TipoToStr(ancho) + "x" + Constant::TipoToStr(alto), W_DEBUG);
    float relacion = IcoGestor::relacion(alto, ancho, HEIGHT, WIDTH);

    VIDEOWIDTH = (int)(ancho / relacion);
    VIDEOHEIGHT = (int)(alto / relacion);
    Traza::print("Tam Redimensionado del video: " + Constant::TipoToStr(VIDEOWIDTH) + "x" + Constant::TipoToStr(VIDEOHEIGHT), W_DEBUG);

    rect.x = (WIDTH - VIDEOWIDTH) / 2;
    rect.y = (HEIGHT - VIDEOHEIGHT) / 2;
    rect.w = 0;
    rect.h = 0;
}

