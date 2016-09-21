/**
* Clase que sirve para dibujar los menus o cualquier pantalla
* de la aplicacion
*/

#ifndef Iofrontend_H
#define Iofrontend_H

#include <string>
#include <vector>
#include "Ioutil.h"
#include "Colorutil.h"
#include "Menuobject.h"
#include "ImagenGestor.h"
#include "Icogestor.h"
#include "Launcher.h"
#include "roms/gestorroms.h"
#include "thread.h"
#include "vlcplayer/vlcplayer.h"
#include "roms/EmuProperties.h"

#include "uipicture.h"
#include "uiinput.h"
#include "uipicture.h"
#include "uiart.h"
#include "uibutton.h"
#include "uiprogressbar.h"
#include "uipopupmenu.h"

//*********************************************************************************/
//Cuidado con no sobrepasar el numero de elementos que define la constante MAXMENU
//*********************************************************************************/
 typedef enum { MENUINICIAL,     //0
            MENUJUEGOS,
            MENUOPCIONES,
            PANTALLAOPCIONRUTAS,
            PANTALLAEDITAREMU,
            PANTALLAEDITARRUTAS,
            PANTALLAPREGUNTA,
            PANTALLALISTADODIR,
            PANTALLAROMS,
            LANZARROM,
            PANTALLACONTROLES,
            PANTALLACONFIRMAR,
            PANTALLABROWSER2,
            PANTALLAOPENMEDIA,
            PANTALLAREPRODUCTOR,
            MENUUTILS,
            MENUPOPUPS,
            PANTALLAGROUPLIST,
            PANTALLAOPCIONESROM,
            MENUOPCIONESGENERALES,
            MAXMENU
    } enumMenus;

class Iofrontend : public Ioutil{

     private :
        void playMedia(tEvento *evento);
        Colorutil *convColor;
        void obtenerImgCateg(tEvento *);
        void setDinamicSizeObjects();
        void mensajeCompilando(tEvento, string, bool);
        bool procesarBoton(Object * obj, tmenu_gestor_objects *gestorMenu);
        string casoJOYBUTTONS(tEvento *evento);
        bool importRetroarchConfig();
        long waitMedia();
        void setPanelMediaVisible(bool );
        bool bucleReproductor(string);
        int calculaPosPanelMedia();
        Gestorroms *gestorRoms;

        //Funciones para anyadir punteros a funciones
        typedef int (Iofrontend::*typept2Func)(tEvento *); //Se declara el puntero a funcion
        typept2Func pt2Func[MAXMENU*MAXOBJECTS]; //Se declara un array de punteros a funcion

        struct tprops{
            vector<string> name;
            vector<string> parms;
            int size;
        } propertiesPt2Func;

        struct tscreenobj{
            bool drawComponents;
            bool execFunctions;
        };

        void addEvent(string, typept2Func);
        void addEvent(string, typept2Func, int);
        int findEventPos(string);
        void setEvent(string, typept2Func);
        void setEvent(string nombre, typept2Func funcion, int parms);
        int selMenu;
        //Eventos asignados a los botones
        void simularEscape(tEvento *);
        void simularIntro(tEvento *);
        int marcarBotonSeleccionado(tEvento *);
        int casoDEFAULT(tEvento );
        int accionesMenu(tEvento *);
        int accionesGotoPantalla(tEvento *);
        int accionesCargaPantalla(tEvento *);
        int loadDirFromExplorer(tEvento *);
        int accionesBtnAceptarDir(tEvento *);
        int accionesMediaAvanzar(tEvento *);
        int accionesMediaRetroceder(tEvento *);
        int accionesMediaPause(tEvento *);
        int accionesMediaStop(tEvento *);
        int mediaClicked(tEvento *);
        int accionesBtnVolverConfigEmu(tEvento *);
        bool cargarListaEmuladores(int,int, string);
        bool cargarListaRoms(int, string, string);
        int accionesListaExplorador(tEvento *);
        bool actualizarRoms();
        string showExplorador(tEvento *);
        void cargaMenuFromLista(UIListCommon *, tEvento *);
        void cargaMenu(int, string, tEvento *);
        bool cargarDatosEmulador(string );
        int accionesBtnEliminarEmu(tEvento *);
        bool lanzarPrograma(string);
        bool browser(int, string, int, string, string);
        void comprobarUnicode(int);
        VlcPlayer player;
        void setTextFromExplorador(tEvento *, UIInput *);
        void clearEmuFields();
        void comprobarUnidad(string);
        UIPopupMenu * addPopup(int pantalla, string popupName, string callerName);
        bool procesarPopups(tmenu_gestor_objects *objMenu, tEvento *evento);
        void comprobarFixesSO(FileLaunch *emulInfo);
        int accionConfigEmusPopup(tEvento *evento);
        bool procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento);
        DWORD setInfoRomValues();
        void crearComboSistemas();
        //Muestra el menu especificado como si fuera emergente
        void showMenuEmergente(int menu, string objImagenFondo);
        int buscarInfoRoms(tEvento *evento);
        int volverInfoRoms(tEvento *evento);
        int accionesBtnAceptarOpcionesGenerales(tEvento *evento);
        bool cargarOpcionesGenerales();
        bool scrapAllRoms();
        int accionCombo(tEvento *evento);
        void loadComboUnidades(string objName, int pantalla, int types);



    public :
        Iofrontend();
        ~Iofrontend();
        bool drawMenu(tEvento );
        void resizeMenu();
        void popUpMenu(tEvento);
        void initUIObjs();
        ImagenGestor *imgGestor ;
        bool isMenuLeftVisible();
        string casoPANTALLAPREGUNTA(string, string);
        bool casoPANTALLACONFIRMAR(string, string);
        //DEBERIAN SER PRIVATE!!!!!!!!!!!!!!!!!!!!!!!!!!!
        tmenu_gestor_objects *ObjectsMenu[MAXMENU];  //Creo tantos punteros a tmenu_gestor_objects como menus dispongo
                                                    //De esta forma podre asignar objetos a cada menu

        bool procesarControles(tmenu_gestor_objects *, tEvento *, tscreenobj *);
        void setSelMenu(int var){ selMenu = var;}
        int getSelMenu(){ return selMenu;}
        int accionesBtnAceptarOpciones(tEvento *);
        int accionCopiarTextoPopup(tEvento *evento);
        int launchEmuForConfig(tEvento *evento);

};
#endif //Iofrontend_H
