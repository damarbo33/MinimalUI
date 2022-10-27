/**
* Clase que sirve para dibujar los menus o cualquier pantalla
* de la aplicacion
*/

#ifndef Iofrontend_H
#define Iofrontend_H

#include <string>
#include <vector>
#include "uiobjects/Colorutil.h"
#include "uiobjects/Menuobject.h"
#include "uiobjects/ImagenGestor.h"
#include "uiobjects/common/Icogestor.h"
#include "uiobjects/Launcher.h"
#include "roms/gestorroms.h"
#include "uiobjects/thread.h"
#include "vlcplayer/vlcplayer.h"
#include "roms/EmuProperties.h"

#include "uiobjects/uipicture.h"
#include "uiobjects/uiinput.h"
#include "uiobjects/uipicture.h"
#include "uiobjects/uiart.h"
#include "uiobjects/uibutton.h"
#include "uiobjects/uiprogressbar.h"
#include "uiobjects/uipopupmenu.h"
#include "uiobjects/common/BaseFrontend.h"

#define MENUINICIAL           "MENUINICIAL"
#define MENUJUEGOS            "MENUJUEGOS"
#define MENUOPCIONES          "MENUOPCIONES"
#define PANTALLAOPCIONRUTAS   "PANTALLAOPCIONRUTAS"
#define PANTALLAEDITAREMU     "PANTALLAEDITAREMU"
#define PANTALLAEDITARRUTAS   "PANTALLAEDITARRUTAS"
#define PANTALLALISTADODIR    "PANTALLALISTADODIR"
#define PANTALLAROMS          "PANTALLAROMS"
#define LANZARROM             "LANZARROM"
#define PANTALLACONTROLES     "PANTALLACONTROLES"
#define PANTALLAOPENMEDIA     "PANTALLAOPENMEDIA"
#define PANTALLAREPRODUCTOR   "PANTALLAREPRODUCTOR"
#define MENUUTILS             "MENUUTILS"
#define MENUPOPUPS            "MENUPOPUPS"
#define PANTALLAGROUPLIST     "PANTALLAGROUPLIST"
#define PANTALLAOPCIONESROM   "PANTALLAOPCIONESROM"
#define MENUOPCIONESGENERALES "MENUOPCIONESGENERALES"


class Iofrontend : public BaseFrontend{
    public :
        Iofrontend();
        ~Iofrontend();

        ImagenGestor *imgGestor ;
        bool isMenuLeftVisible();
        int accionesBtnAceptarOpciones(tEvento *);
        int launchEmuForConfig(tEvento *evento);

     private :

        void setDinamicSizeObjects();
        void initUIObjs();
        void cargaMenu(string, string, tEvento *);

        Gestorroms *gestorRoms;
        Colorutil *convColor;
        VlcPlayer player;

        void playMedia(tEvento *evento);
        void obtenerImgCateg(tEvento *);
        bool importRetroarchConfig();
        long waitMedia();
        void setPanelMediaVisible(bool );
        bool bucleReproductor(string);
        int calculaPosPanelMedia();
        int accionesMediaAvanzar(tEvento *);
        int accionesMediaRetroceder(tEvento *);
        int accionesMediaPause(tEvento *);
        int accionesMediaStop(tEvento *);
        int mediaClicked(tEvento *);
        int accionesBtnVolverConfigEmu(tEvento *);
        bool cargarListaEmuladores(string, string, string);
        bool cargarListaRoms(string, string, string);
        bool actualizarRoms();
        void cargaMenuFromLista(UIListCommon *, tEvento *);
        bool cargarDatosEmulador(string );
        int accionesBtnEliminarEmu(tEvento *);
        bool lanzarPrograma(string);
        bool browser(int, string, int, string, string);
        void clearEmuFields();
        void comprobarUnidad(string);
        void comprobarFixesSO(FileLaunch *emulInfo);
        int accionConfigEmusPopup(tEvento *evento);
        bool procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento);
        DWORD setInfoRomValues();
        void crearComboSistemas();
        //Muestra el menu especificado como si fuera emergente
        int buscarInfoRoms(tEvento *evento);
        int volverInfoRoms(tEvento *evento);
        int accionesBtnAceptarOpcionesGenerales(tEvento *evento);
        bool cargarOpcionesGenerales();
        void scrapAllRoms();
        void scrapEmuRoms(string codEmu);
        int ImgEmuladorClicked(tEvento *evento);
        int ImgBoxArtFullClicked(tEvento *evento);
};
#endif //Iofrontend_H
