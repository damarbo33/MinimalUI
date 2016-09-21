#ifndef VLCPLAYER_H
#define VLCPLAYER_H

#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <assert.h>

#include <SDL/SDL.h>
#include <SDL/SDL_mutex.h>
#include <vlc/vlc.h>
#include <string>
#include <iostream>
#include <limits.h>
#include "Ioutil.h"

using namespace std;

struct ctx
{
    SDL_Surface *surf;
    SDL_mutex *mutex;
    tmenu_gestor_objects *obj;
};

/**
*
*/
class VlcPlayer: public Ioutil{
    public:
        VlcPlayer();
        ~VlcPlayer();
        int play(string);
        int pause();

        void setScreen( SDL_Surface* var) {
            screen = var;
            WIDTH = screen->w;
            HEIGHT = screen->h;
            VIDEOWIDTH = screen->w;
            VIDEOHEIGHT = screen->h;
        }


        void blitVideo();
        void stop();
        void destroy();
        void salto10PercentForward();
        void salto10PercentBackwards();
        void setPositionPercent(float);
        float getPositionPercent();
        long getTotalSecs();

        string getTimePos();
        string getTimeTotal();
        void setSizeToVideo();

        void setObjsMenuVideo(tmenu_gestor_objects *var){
            surfaceVideo.obj = var;
        }

    protected:
    private:
        static int VIDEOWIDTH, VIDEOHEIGHT,WIDTH,HEIGHT, BPP;
        static void display(void *data, void *id);
        static void unlock(void *data, void *id, void *const *p_pixels);
        static void * lock(void *data, void **p_pixels);

        libvlc_instance_t *libvlc;
        libvlc_media_t *media;
        libvlc_media_player_t *mplayer;
        static int isPaused;
        struct ctx surfaceVideo;
        static SDL_Rect rect;
};



#endif // VLCPLAYER_H
