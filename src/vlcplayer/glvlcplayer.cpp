#include "glvlcplayer.h"

GLVlcPlayer::GLVlcPlayer()
{
    //ctor
}

GLVlcPlayer::~GLVlcPlayer()
{
    //dtor
}

    /*
    Modified for openGL
    Draws a video to a opengl rect on a SDL surface

     */
    #include <stdio.h>
    #include <stdint.h>

    #include <stdlib.h>
    #include <assert.h>

    #include <SDL/SDL.h>
    #include <SDL/SDL_mutex.h>


    #include <GL/glu.h>

    #include <GL/glext.h>

    #include <vlc/vlc.h>

    #define WIDTH 640
    #define HEIGHT 480

    #define VIDEOWIDTH 320
    #define VIDEOHEIGHT 240

    struct ctx
    {
        SDL_Surface *surf;
        SDL_mutex *mutex;
    };

    static void *lock(void *data, void **p_pixels)
    {
        struct ctx *ctx = (struct ctx *)data;

        SDL_LockMutex(ctx->mutex);
        SDL_LockSurface(ctx->surf);
        *p_pixels = ctx->surf->pixels;
        return NULL; /* picture identifier, not needed here */
    }

    static void unlock(void *data, void *id, void *const *p_pixels)
    {
        struct ctx *ctx = (struct ctx *)data;

        /* VLC just rendered the video, but we can also render stuff */
        uint16_t *pixels = (uint16_t *)p_pixels;
       /* int x, y;
       //annoying box that was being drawn
        for(y = 10; y < 40; y++)
            for(x = 10; x < 40; x++)
                if(x < 13 || y < 13 || x > 36 || y > 36)
                    pixels[y * VIDEOWIDTH + x] = 0xffff;
                else
                    pixels[y * VIDEOWIDTH + x] = 0x0;*/

        SDL_UnlockSurface(ctx->surf);
        SDL_UnlockMutex(ctx->mutex);

        assert(id == NULL); /* picture identifier, not needed here */
    }

    int loadTexture(SDL_Surface *sdlimage) {

       GLuint retval = NULL;

       void *raw;

       int w, h, i, j, bpp;

       Uint8 *srcPixel, *dstPixel;

       Uint32 truePixel;

       GLenum errorCode;



       if (sdlimage->format->BytesPerPixel < 2) {

          printf("Bad image -- not true color!\n");

          SDL_FreeSurface(sdlimage);

          return 0;

       }



       w = sdlimage->w;

       h = sdlimage->h;



       raw = (void *) malloc(w * h * 4);

       dstPixel = (Uint8 *) raw;



       bpp = sdlimage->format->BytesPerPixel;



       for (i = h - 1; i >= 0; i--) {

          for (j = 0; j < w; j++) {

             srcPixel = (Uint8 *) sdlimage->pixels + i * sdlimage->pitch + j   * bpp;

             switch (bpp) {

             case 1:

                truePixel = *srcPixel;

                break;



             case 2:

                truePixel = *(Uint16 *) srcPixel;

                break;



             case 3:

                if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {

                   truePixel = srcPixel[0] << 16 | srcPixel[1] << 8 | srcPixel[2];

                } else {

                   truePixel = srcPixel[0] | srcPixel[1] << 8 | srcPixel[2] << 16;

                }

                break;



             case 4:

                truePixel = *(Uint32 *) srcPixel;

                break;



             default:

                printf("Image bpp of %d unusable\n", bpp);

                SDL_UnlockSurface(sdlimage);

                SDL_FreeSurface(sdlimage);

                free(raw);

                return 0;

             }



             SDL_GetRGBA(truePixel, sdlimage->format, &(dstPixel[0]), &(dstPixel[1]), &(dstPixel[2]), &(dstPixel[3]));

             dstPixel++;

             dstPixel++;

             dstPixel++;

             dstPixel++;

          }

       }



       while (glGetError()) {

          ;

       }



       glBindTexture(GL_TEXTURE_2D, retval);

       //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

       //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);





       errorCode = glGetError();

       if (errorCode != 0) {

          if (errorCode == GL_OUT_OF_MEMORY) {

             printf("Out of texture memory!\n");

          }



          glDeleteTextures(1, &retval);

          free(raw);

          return 0;

       }



       //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, (Uint8 *) raw);
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
       glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

       glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, VIDEOWIDTH, VIDEOHEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE,(Uint8 *) raw);



       errorCode = glGetError();

       if (errorCode != 0) {

          if (errorCode == GL_OUT_OF_MEMORY) {

             printf("Out of texture memory!\n");

          }



          glDeleteTextures(1, &retval);

          free(raw);

          return 0;

       }

       free(raw);

       return retval;

    }





    static void display(void *data, void *id)
    {
        /* VLC wants to display the video */
        (void) data;
        assert(id == NULL);
    }

    int main(int argc, char *argv[])
    {
       char width[32], height[32], pitch[32];
        libvlc_instance_t *libvlc;
        libvlc_media_t *m;
        libvlc_media_player_t *mp;
        char const *vlc_argv[] =
        {
            "--quiet",

            "--ignore-config",

          "--no-video-title",
            "--no-xlib", /* tell VLC to not use Xlib */
        };
        int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

        SDL_Surface *screen;
        SDL_Event event;

        int done = 0, action = 0, pause = 0, n = 0;

        struct ctx ctx;

        if(argc < 2)
        {
            printf("Usage: %s <filename>\n", argv[0]);
            return EXIT_FAILURE;
        }

        /*
         *  Initialise libSDL
         */
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTTHREAD) == -1)
        {
            printf("cannot initialize SDL\n");
            return EXIT_FAILURE;
        }


        ctx.surf = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEOWIDTH, VIDEOHEIGHT,
                                        16, 0x001f, 0x07e0, 0xf800, 0);
        ctx.mutex = SDL_CreateMutex();

        int options = SDL_ANYFORMAT | SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL;

        screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
        if(!screen)
        {
            printf("cannot set video mode\n");
            return EXIT_FAILURE;
        }

        /*
         *  Initialise libVLC
         */
        libvlc = libvlc_new(vlc_argc, vlc_argv);
        m = libvlc_media_new_path(libvlc, argv[1]);
        mp = libvlc_media_player_new_from_media(m);
        libvlc_media_release(m);

        libvlc_video_set_callbacks(mp, lock, unlock, display, &ctx);
        libvlc_video_set_format(mp, "RV16", VIDEOWIDTH, VIDEOHEIGHT, VIDEOWIDTH*2);
        libvlc_media_player_play(mp);

        /*
         *  Main loop
         */


       glEnable(GL_TEXTURE_2D);

        GLuint image;

        glGenTextures(1, &image);


        while(!done)
        {
            action = 0;

            /* Keys: enter (fullscreen), space (pause), escape (quit) */
            while( SDL_PollEvent( &event ) )
            {
                switch(event.type)
                {
                case SDL_QUIT:
                    done = 1;
                    break;
                case SDL_KEYDOWN:
                    action = event.key.keysym.sym;
                    break;
                }
            }

            switch(action)
            {
            case SDLK_ESCAPE:
                done = 1;
                break;
            case SDLK_RETURN:
                options ^= SDL_FULLSCREEN;
                screen = SDL_SetVideoMode(WIDTH, HEIGHT, 0, options);
                break;
            case ' ':
                pause = !pause;
                break;
            }

            if(!pause)
                n++;

          glClear(GL_COLOR_BUFFER_BIT);
            /* Blitting the surface does not prevent it from being locked and
             * written to by another thread, so we use this additional mutex. */
            SDL_LockMutex(ctx.mutex);
            //SDL_BlitSurface(ctx.surf, NULL, screen, NULL);
          image = loadTexture(ctx.surf);
            SDL_UnlockMutex(ctx.mutex);

            //SDL_Flip(screen);

    glBegin(GL_QUADS);

             glTexCoord2d(0, 1);

             glVertex2f(0, 0.9);

             glTexCoord2d(1, 1);

             glVertex2f(0.9, 0.9);

             glTexCoord2d(1, 0);

             glVertex2f(0.9, 0);

             glTexCoord2d(0, 0);

             glVertex2f(0, 0);

          glEnd();



          glFlush();

            SDL_GL_SwapBuffers();



            //SDL_Delay(5);

            //SDL_BlitSurface(empty, NULL, screen, NULL);
          //use SDL timers to determine playback
          //printf("%i  %i\n",libvlc_media_player_get_length(mp)-1000,libvlc_media_player_get_time(mp));
          if(libvlc_media_player_get_length(mp)-1000 == libvlc_media_player_get_time(mp)){
          done = 1;
          }
        }
       glDeleteTextures(1, &image);

        /*
         * Stop stream and clean up libVLC
         */
        libvlc_media_player_stop(mp);
        libvlc_media_player_release(mp);
        libvlc_release(libvlc);

        /*
         * Close window and clean up libSDL
         */
        SDL_DestroyMutex(ctx.mutex);
        SDL_FreeSurface(ctx.surf);
        //SDL_FreeSurface(empty);

        SDL_Quit();

        return 0;
    }
