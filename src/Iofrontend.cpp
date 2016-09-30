#include "Iofrontend.h"
#include "uilistgroup.h"
#include "beans/listgroupcol.h"
#include "uilistcommon.h"


/**
* Constructor
* HEREDA DE Ioutil
*
*/
Iofrontend::Iofrontend(){
    Traza::print("Constructor de IoFrontend", W_DEBUG);
    gestorRoms = new Gestorroms(dirInicial);

    convColor = new Colorutil();
    imgGestor = new ImagenGestor();

    Traza::print("Creando objetos de cada menu", W_PARANOIC);
    for (int i=0; i < MAXMENU; i++){
        ObjectsMenu[i] = new tmenu_gestor_objects(this->getWidth(), this->getHeight()); //Inicializo el puntero asignado a cada menu. En el constructor de esta clase,                                                    //se crean tantos objetos como se defina en la constante MAXOBJECTS
    }

    for (int i=0; i < MAXMENU*MAXOBJECTS; i++){
        pt2Func[i] = NULL;
    }
    propertiesPt2Func.size = 0;
    Traza::print("Asignando elementos y acciones", W_PARANOIC);
    initUIObjs();
    Traza::print("Fin Constructor de IoFrontend", W_PARANOIC);
    comprobarUnidad(dirInicial);
    selMenu = MENUJUEGOS;
    cargaMenu(selMenu, "", NULL);
    //buscarInfoRoms();
}



/**
* Destructor
*/
Iofrontend::~Iofrontend(){
    Traza::print("Destructor de IoFrontend", W_DEBUG);
    delete convColor;

//  A los objetos no los podemos eliminar porque no fueron creados dinamicamente
    Traza::print("Eliminando objetos de cada Menu", W_DEBUG);
    for (int i=0; i < MAXMENU; i++){
        Traza::print("Eliminando menu: ", i, W_PARANOIC);
        delete ObjectsMenu[i];
    }
    Traza::print("Destructor de IoFrontend FIN", W_DEBUG);
}

/**
* Comprueba que la unidad de las roms sea la misma unidad que la del ejecutable
* del lanzador.
*/
void Iofrontend::comprobarUnidad(string diractual){
    string unidadActual = "", unidadRoms = "";
    bool errorRutas = false;
    //Obtenemos la unidad del directorio inicial
    if (diractual.length() >= 2){
        unidadActual = diractual.substr(0,2);
    }
    //Comprobamos si la unidad actual corresponde con la almacenada en las tablas
    errorRutas = gestorRoms->comprobarUnidad(diractual, &unidadRoms);

    //Si hay error, lanzamos aviso para actualizar las roms a la unidad correcta
    if (errorRutas){
        Traza::print("ERROR DE UNIDADES: Unidad actual= " + unidadActual + ", Unidad de las roms: " + unidadRoms, W_DEBUG);
        string pregunta = "La unidad del disco duro " + unidadActual + " es distinta a las rutas de los emuladores "
        + unidadRoms + ". ¿Deseas actualizar?";
        bool confirmed = casoPANTALLACONFIRMAR("Confirmacion", pregunta);
        if (confirmed){
            gestorRoms->updateRutas(unidadRoms, unidadActual);
        }
    }
}

/**
* Inicializa los objetos que se pintaran por pantalla para cada pantalla
* Debe llamarse a este metodo despues de haber inicializado SDL para que se puedan centrar los componentes correctamente
**/

void Iofrontend::initUIObjs(){

    ObjectsMenu[MENUINICIAL]->add("ListaMenuInicial", GUILISTBOX, 0, 0, 0, 0, "ListaMenuInicial", true)->setVerContenedor(true);
    tmenu_gestor_objects *objMenu = ObjectsMenu[MENUINICIAL];

    ObjectsMenu[MENUJUEGOS]->add("ImgFondo", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondo", true)->setEnabled(false);
    ObjectsMenu[MENUJUEGOS]->add("ListaMenuJuegos", GUILISTBOX, 0, 0, 0, 0, "ListaMenuJuegos", true)->setVerContenedor(false);
    ((UIPicture*) ObjectsMenu[MENUJUEGOS]->getObjByName("ImgFondo"))->loadImgFromFile(dirInicial +  Constant::getFileSep()
        + "emuImgs" + Constant::getFileSep() + "54573da0d72cc.jpeg");
    ObjectsMenu[MENUJUEGOS]->getObjByName("ImgFondo")->setAlpha(100);

    ((UIList *)ObjectsMenu[MENUJUEGOS]->getObjByName("ListaMenuJuegos"))->setListScheme(SCHEMEICONS);

    UIPopupMenu * popupJuegos = addPopup(MENUJUEGOS, "popupEmusConfig", "ListaMenuJuegos");
    if (popupJuegos != NULL){
        popupJuegos->addElemLista("Config. Emu ", "ConfigEmu", controller);
        popupJuegos->addElemLista("Actualizar Información ", "ConfigEmu", arrow_refresh);
    }

    ObjectsMenu[MENUOPCIONES]->add("ListaMenuOpciones", GUILISTBOX, 0, 0, 0, 0, "ListaMenuOpciones", true);
    ObjectsMenu[PANTALLAEDITAREMU]->add("listaEditarEmus", GUILISTBOX, 0, 0, 0, 0, "listaEditarEmus", false);

    objMenu = ObjectsMenu[MENUINICIAL];
    UIList * listaObj = (UIList *) objMenu->getObjByName("ListaMenuInicial");
    listaObj->addElemLista("Juegos", "", controller, MENUJUEGOS);
    listaObj->addElemLista("Videos", "", film, PANTALLAOPENMEDIA);
    listaObj->addElemLista("Opciones", "", bullet_wrench, MENUOPCIONES);
    listaObj->addElemLista("Reiniciar","reboot", arrow_refresh);
    listaObj->addElemLista("Apagar","shutdown", door_out);
    listaObj->addElemLista("Salir","salir", application_xp_terminal);
    objMenu->findNextFocus();

    /**Pantalla con el listado del menu de opciones*/
    objMenu = ObjectsMenu[MENUOPCIONES];
    UIList *listaObj2 = (UIList *) objMenu->getObjByName("ListaMenuOpciones");
    listaObj2->addElemLista("Alta emulador", "", add, PANTALLAOPCIONRUTAS);
    listaObj2->addElemLista("Modificar emulador","", application_form_edit, PANTALLAEDITAREMU);
    listaObj2->addElemLista("Escanear roms","scan", arrow_refresh, MENUOPCIONES);
    listaObj2->addElemLista("Actualizar carátulas e info restante","scrapAll", folder_explore, MENUOPCIONES);
    listaObj2->addElemLista("Importar ajustes de Retroarch","importRetroarch", database_add, MENUOPCIONES);
    listaObj2->addElemLista("Asignar botones", "configJoy", controller, MENUOPCIONES);
    listaObj2->addElemLista("Opciones generales", "", page_white_gear, MENUOPCIONESGENERALES);
    listaObj2->addElemLista("Volver", "", bullet_go, MENUINICIAL);


    /**Pantalla de opciones generales*/
    int centroY = (-200 + ((zoomText > 1) ? -Constant::getMENUSPACE() : 0));
    ObjectsMenu[MENUOPCIONESGENERALES]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones Generales", false)->setEnabled(false);
    ObjectsMenu[MENUOPCIONESGENERALES]->add("ComboFixesGeneral", GUICOMBOBOX, 0, centroY, 150, Constant::getCOMBOLISTHEIGHT(), "Corregir problemas en ddraw", true)->setVerContenedor(true);
    objMenu = ObjectsMenu[MENUOPCIONESGENERALES];
    UIComboBox * comboGeneral = (UIComboBox *) objMenu->getObjByName("ComboFixesGeneral");
    comboGeneral->addElemLista("Sin Fix", "0");
    comboGeneral->addElemLista("Windows 8", "1");
    comboGeneral->addElemLista("Windows 8.1", "2");

    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[MENUOPCIONESGENERALES]->add("btnAceptarGeneralOpt",  GUIBUTTON, -(BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Aceptar" , true)->setIcon(tick);
    ObjectsMenu[MENUOPCIONESGENERALES]->add("btnCancelarGeneralOpt", GUIBUTTON, (BUTTONW + 5) , centroY, BUTTONW -15,BUTTONH, "Cancelar", true)->setIcon(cross);

    /**Pantalla de opciones de cada emulador*/
    centroY = (-200 + ((zoomText > 1) ? -Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuName", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Nombre emulador:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuRuta", GUIINPUTWIDE, - 15 , centroY, INPUTW - 30, Constant::getINPUTH(), "Ruta emulador:", true)->setColor(cNegro);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnEmuRuta", GUIBUTTON, INPUTW/2 * zoomText - 10, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuRuta");
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnLaunchEmu", GUIBUTTON, INPUTW/2 * zoomText + 20, centroY,MINIBUTTONH,MINIBUTTONH, "", true)->setIcon(bullet_wrench);;
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));

    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuParms", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Parametros emulador:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuRutaRom", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Ruta roms:", true)->setColor(cNegro);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnRomRuta", GUIBUTTON, INPUTW/2 + 20 * zoomText, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuRutaRom");
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuRomExtension", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Extension:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("EmuImg", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Imagen emulador:", true)->setColor(cNegro);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnEmuImg", GUIBUTTON, INPUTW/2 + 20 * zoomText, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuImg");
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("ComboPlatform", GUICOMBOBOX, 130, centroY, 280, Constant::getCOMBOLISTHEIGHT(), "Sistema para obtener info", true)->setVerContenedor(true);
    crearComboSistemas();
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("CheckComboFixes", GUICHECK, -195, centroY - 60, 150, CHECKH, "Corregir problemas en ddraw", true);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("CheckDescomprimir", GUICHECK, -195, centroY, 150, CHECKH, "Descomprimir roms", true);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("CheckFindTitleRom", GUICHECK, -20, centroY, 150, CHECKH, "Buscar descripción de rom", true);
    centroY += (40 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));

    UIPopupMenu * popup1 = addPopup(PANTALLAOPCIONRUTAS, "popupParmsRom", "EmuParms");
    if (popup1 != NULL){
        popup1->addElemLista("Ruta de la rom", "%ROMPATH%", controller);
        popup1->addElemLista("Nombre de la Rom", "%ROMNAME%", controller);
        popup1->addElemLista("Nombre de la Rom con extensión", "%ROMNAMEXT%", controller);
        popup1->addElemLista("Ruta y nombre de Rom con extensión", "%ROMFULLPATH%", controller);
    }

    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnAceptarEmu", GUIBUTTON, -(BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnEliminarEmu", GUIBUTTON, 0, centroY, BUTTONW -15,BUTTONH, "Eliminar", true)->setIcon(deleteIco)->setVisible(false);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add("btnCancelarEmu", GUIBUTTON, (BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Cancelar", true)->setIcon(cross);

    ObjectsMenu[PANTALLAPREGUNTA]->add("valor", GUIINPUTWIDE, 0, -20 * zoomText, INPUTW, Constant::getINPUTH(), "Dato:", true);
    ObjectsMenu[PANTALLAPREGUNTA]->add("btnAceptarPregunta", GUIBUTTON, -(BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLAPREGUNTA]->add("btnCancelarPregunta", GUIBUTTON, (BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    ObjectsMenu[PANTALLAPREGUNTA]->add("borde", GUIPANELBORDER,0,0,0,0, "Introduzca el dato", false);

    ObjectsMenu[PANTALLACONFIRMAR]->add("labelConfirm", GUILABEL, 0, -20 * zoomText, 50, getFontHeight(), "Aceptar o Cancelar la pregunta", true)->setTextColor(cBlanco);
    ObjectsMenu[PANTALLACONFIRMAR]->add("btnSiConfirma", GUIBUTTON, -(BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLACONFIRMAR]->add("btnNoConfirma", GUIBUTTON, (BUTTONW/2 + 5), 30,BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    ObjectsMenu[PANTALLACONFIRMAR]->add("borde", GUIPANELBORDER,0,0,0,0, "Seleccione una opcion", false);

    ObjectsMenu[PANTALLABROWSER2]->add("ImgFondo", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondo", true)->setEnabled(false);
    ObjectsMenu[PANTALLABROWSER2]->getObjByName("ImgFondo")->setAlpha(150);
    ObjectsMenu[PANTALLABROWSER2]->add(OBJLISTABROWSER2, GUILISTBOX, 0, 0, 0, 0, "LISTADODIR", false)->setVerContenedor(false)->setShadow(false);
    ObjectsMenu[PANTALLABROWSER2]->add(BTNACEPTARBROWSER, GUIBUTTON, -(BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Aceptar", true)->setIcon(tick);
    ObjectsMenu[PANTALLABROWSER2]->add(BTNCANCELARBROWSER, GUIBUTTON, (BUTTONW/2 + 5), 0, BUTTONW,BUTTONH, "Cancelar", true)->setIcon(cross);
    ObjectsMenu[PANTALLABROWSER2]->add(ARTDIRBROWSER, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), Constant::toAnsiString("Dirección Browser"), false)->setEnabled(false);
    ObjectsMenu[PANTALLABROWSER2]->add("comboBrowser", GUICOMBOBOX, 0, 0, 0, 0, "", false);

    ObjectsMenu[MENUINICIAL]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Lanzador", false)->setEnabled(false);

    ObjectsMenu[MENUOPCIONES]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones", false)->setEnabled(false);
    ObjectsMenu[PANTALLAEDITAREMU]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Modificar emulador", false)->setEnabled(false);
    ObjectsMenu[PANTALLAOPCIONRUTAS]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Alta/Modificacion emulador", false)->setEnabled(false);
    ObjectsMenu[MENUJUEGOS]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Emuladores disponibles", false)->setEnabled(false);


    ObjectsMenu[PANTALLAROMS]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Roms", false)->setEnabled(false);
    ObjectsMenu[PANTALLAROMS]->add("ImgEmulador", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgEmulador", true)->setEnabled(false);
    ObjectsMenu[PANTALLAROMS]->add("ListaMenuRoms", GUILISTBOX, 0, 0, 0, 0, "ListaMenuRoms", true)->setVerContenedor(false);
    ObjectsMenu[PANTALLAROMS]->getObjByName("ImgEmulador")->setAlpha(150);

    ObjectsMenu[PANTALLAREPRODUCTOR]->add("panelMedia", GUIPANEL, 0,0,0,0, "", true)->setEnabled(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnBackward", GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_rewind)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnPlay",     GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_play)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnStop",     GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_stop)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("btnForward",  GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_fastforward)->setVerContenedor(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("mediaTimerTotal",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);;
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("mediaTimer",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);;
    ObjectsMenu[PANTALLAREPRODUCTOR]->add("progressBarMedia", GUIPROGRESSBAR, 20, 20, 200, 20, "", true)->setShadow(false);
    ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("panelMedia")->setAlpha(150);


    ObjectsMenu[PANTALLAOPCIONESROM]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones Roms", false)->setEnabled(false);
    ObjectsMenu[PANTALLAOPCIONESROM]->add("ImgFondoRoms", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondoRoms", true)->setEnabled(false);
    ObjectsMenu[PANTALLAOPCIONESROM]->add("panelMedia", GUIPANEL, 0,0,500,350, "", true)->setEnabled(false);
    ObjectsMenu[PANTALLAOPCIONESROM]->add("btnScrapGame", GUIBUTTON, -(BUTTONW/2 + 5), -BUTTONH *2, BUTTONW,BUTTONH, "Buscar info", true)->setTag("");
    ObjectsMenu[PANTALLAOPCIONESROM]->add("btnCancelarOptRoms", GUIBUTTON, 0, 100, BUTTONW,BUTTONH, "Volver", true)->setIcon(cross);
    objMenu = ObjectsMenu[PANTALLAOPCIONESROM];
    UIPanel * panel = (UIPanel *) objMenu->getObjByName("panelMedia");
    panel->setColor(cGrisClaro);


    ObjectsMenu[PANTALLAGROUPLIST]->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Roms", false)->setEnabled(false);
    ObjectsMenu[PANTALLAGROUPLIST]->add("ImgEmulador", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgEmulador", true)->setEnabled(false);
    ObjectsMenu[PANTALLAGROUPLIST]->add("listaGrupoRoms", GUILISTGROUPBOX, 0, 0, 0, 0, "", false)->setVerContenedor(false);
    ObjectsMenu[PANTALLAGROUPLIST]->add("textosBox", GUITEXTELEMENTSAREA, 0, 0, 0, 0, "", true)->setVerContenedor(false)->setEnabled(true);
    ObjectsMenu[PANTALLAGROUPLIST]->add("ImgBoxArt", GUIPICTURE, 0, 0, Constant::getIMGBOXARTWIDTH(), Constant::getIMGBOXARTHEIGHT(), "", true)->setEnabled(true);
    ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("ImgEmulador")->setAlpha(200);
    ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("ImgEmulador")->getImgGestor()->setEnabledMoveImg(false);


    objMenu = ObjectsMenu[PANTALLAGROUPLIST];
    UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
    vector <ListGroupCol *> miCabecera;
    miCabecera.push_back(new ListGroupCol("Nombre Rom", ""));
    miCabecera.push_back(new ListGroupCol("Nº Jugadores", ""));
    listaGrupo->setHeaderLista(miCabecera);
    listaGrupo->adjustToHeader(false);
    listaGrupo->addHeaderWith(500);
    listaGrupo->addHeaderWith(120);

    UIPicture* pict = ((UIPicture*) objMenu->getObjByName("ImgBoxArt"));
    //Para mejorar el rendimiento, las imagenes que se descargan de las caratulas ya estan redimensionadas
    pict->getImgGestor()->setResize(true);
    pict->getImgGestor()->setEnabledMoveImg(false);
    pict->getImgGestor()->setFillBackgroundColour(false);

    int margenX = Constant::getIMGBOXARTWIDTH() + IMGBOXARTMARGIN * 2;
    //Anyadimos los textos en el area que digamos
    UITextElementsArea *infoTextRom = (UITextElementsArea *)objMenu->getObjByName("textosBox");
    t_posicion pos = {margenX + 20, Constant::getMENUSPACE() + IMGBOXARTMARGIN,0,0};
    infoTextRom->addField("txtReleased","RELEASED:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtPlayers","PLAYERS:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtPublisher","PUBLISHER:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtDeveloper","DEVELOPER:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtGenre","GENRE:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtRating","RATING:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtLastPlayed","LAST PLAYED:","",pos, true);
    pos.y += 20;
    infoTextRom->addField("txtTimesPlayed","TIMES PLAYED:","",pos, true);

    t_posicion posDesc = {0, 30 + Constant::getIMGBOXARTHEIGHT() + IMGBOXARTMARGIN * 2,0,0};
    infoTextRom->addField("txtDescripcion","","",posDesc, false);

    UIPopupMenu * popupJuegosG = addPopup(PANTALLAGROUPLIST, "popupEmusConfig", "listaGrupoRoms");
    if (popupJuegosG != NULL){
        popupJuegosG->addElemLista("Config. Emu ", "ConfigEmu", controller);
        popupJuegosG->addElemLista("Actualizar Información ", "ConfigEmu", arrow_refresh);
    }

    //objMenu->findNextFocus();
    //Establecemos los elementos que se redimensionan
    setDinamicSizeObjects();

    //Botones para la pantalla de video
    addEvent("btnPlay",  &Iofrontend::accionesMediaPause);
    addEvent("btnStop",  &Iofrontend::accionesMediaStop);
    addEvent("btnForward",  &Iofrontend::accionesMediaAvanzar);
    addEvent("btnBackward",  &Iofrontend::accionesMediaRetroceder);
    addEvent("progressBarMedia", &Iofrontend::mediaClicked);

	 addEvent("comboBrowser", &Iofrontend::accionCombo);

    //Botones para la pantalla de confirmación
    addEvent("btnSiConfirma", &Iofrontend::marcarBotonSeleccionado);
    addEvent("btnNoConfirma", &Iofrontend::marcarBotonSeleccionado);

    //Botones para la pantalla de los directorios
    addEvent(BTNACEPTARBROWSER, &Iofrontend::marcarBotonSeleccionado);
    addEvent(BTNCANCELARBROWSER, &Iofrontend::marcarBotonSeleccionado);

    //Asignamos acciones a los elementos. NO INSERTAR BOTONES CON EL MISMO NOMBRE AUNQUE ESTEN EN MENUS DISTINTOS
    addEvent("ListaMenuInicial",  &Iofrontend::accionesMenu);
    addEvent("ListaMenuOpciones",  &Iofrontend::accionesMenu);
    addEvent("ListaMenuJuegos",  &Iofrontend::accionesMenu);
    addEvent("ListaMenuRoms",  &Iofrontend::accionesMenu);
    addEvent("listaEditarEmus", &Iofrontend::accionesMenu);
    addEvent("popupParmsRom", &Iofrontend::accionCopiarTextoPopup);
    addEvent("popupEmusConfig", &Iofrontend::accionConfigEmusPopup);

    //Acciones del menu de las opciones
    addEvent("btnEmuRuta",  &Iofrontend::loadDirFromExplorer);
    addEvent("btnRomRuta",  &Iofrontend::loadDirFromExplorer);
    addEvent("btnEmuImg",  &Iofrontend::loadDirFromExplorer);
    addEvent("btnLaunchEmu", &Iofrontend::launchEmuForConfig);
    addEvent("btnAceptarEmu", &Iofrontend::accionesBtnAceptarOpciones);
    addEvent("btnEliminarEmu", &Iofrontend::accionesBtnEliminarEmu);
    addEvent("btnAceptarGeneralOpt", &Iofrontend::accionesBtnAceptarOpcionesGenerales);

    //Acciones por defecto de la lista para directorios
    addEvent("btnCancelarDir", &Iofrontend::accionesGotoPantalla, PANTALLAOPCIONRUTAS);
    addEvent("btnCancelarEmu", &Iofrontend::accionesBtnVolverConfigEmu, PANTALLAEDITAREMU);
    addEvent("btnCancelarGeneralOpt", &Iofrontend::accionesGotoPantalla, MENUOPCIONES);
    addEvent(OBJLISTABROWSER2, &Iofrontend::accionesListaExplorador);
    addEvent("btnScrapGame", &Iofrontend::buscarInfoRoms);
    addEvent("btnCancelarOptRoms", &Iofrontend::volverInfoRoms);
    addEvent("listaGrupoRoms", &Iofrontend::accionesMenu);
    addEvent("ImgBoxArt", &Iofrontend::ImgEmuladorClicked);
}

/**
* Con el menu pasado por parametro lo dibujamos entero
*/
bool Iofrontend::drawMenu(tEvento evento){
    Traza::print("Iofrontend::drawMenu Inicio", W_PARANOIC);
    bool salir = false;
    this->clearScr(cBlanco);
    Traza::print("Iofrontend::clearScr Fin", W_PARANOIC);
    //Realiza las acciones de cada elemento de pantalla
    salir = casoDEFAULT(evento);
    //Muestra un mensaje durante un tiempo determinado
    showAutoMessage();
    //Muestra el contador de cuadros por pantalla
    if (SHOWFPS) fps();
    Traza::print("Iofrontend::drawMenu Fin", W_PARANOIC);
    this->flipScr();
    return salir;
}

/**
*
*/
int Iofrontend::casoDEFAULT(tEvento evento){
    tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
    bool salir = procesarControles(objMenu, &evento, NULL);
    return salir;
}



/**
* Se encarga de procesar las introducciones de teclas o de joystick para cambiar el contenido
* de los botones, inputs, checks, ... y almacenar en ellos los datos correpondientes.
*/
bool Iofrontend::procesarControles(tmenu_gestor_objects *objMenu, tEvento *evento, tscreenobj *screenEvents){
    Traza::print("procesarControles: Inicio", W_PARANOIC);

    bool execFunc = true;
    bool drawComp = true;
    if (screenEvents != NULL){
        execFunc = screenEvents->execFunctions;
        drawComp = screenEvents->drawComponents;
    }

    //Se procesan los eventos de cada uno de los objetos de un menu
    Object *object;

    if (execFunc){
        //Se llama al action del objeto que esta seleccionado en este momento.
        //Cada objeto tiene el suyo propio o heredado de Object
        objMenu->procEvent(*evento);
        //objMenu->procAllEvent(*evento);
        //Mostramos popups si es necesario
        procesarPopups(objMenu, evento);
        //Hacemos llamada para hacer otros procesados. Por ahora solo en la pantalla de seleccion
        //de roms
        procesarMenuActual(objMenu, evento);
    }

    int posBoton = 0;
    bool salir = false;
    bool botonPulsado = false;
    int estado = 0;
    vector<Object *> objPostProcesado;
    int cursorPrincipal = -1;
    bool updateCursor = false;

    //Recorremos todos los objetos para dibujarlos por pantalla
    try{

         //Procesando el redimensionado de ventana
        if (evento->resize)
            resizeMenu();

        //PINTAMOS ANTES DE PROCESAR LAS ACCIONES. NO SE SI ESTO ES BUENA IDEA
        for (int i=0;i<objMenu->getSize();i++){
            object = objMenu->getObjByPos(i);
            //Finalmente dibujamos el objeto
            if (drawComp && object != NULL){
                if (object->getObjectType() == GUICOMBOBOX){
                    objPostProcesado.push_back(object);
                } else {
                    drawObject(object, evento);
                }
            }
        }
        //Para los objetos que son prioritarios de pintar, lo hacemos en ultimo lugar
        //para que se dibujen sobre el resto
        for(vector<Object *>::iterator it = objPostProcesado.begin(); it < objPostProcesado.end(); ++it){
            Object * obj = *it;
            drawObject(obj, evento);
        }
        objPostProcesado.clear();

        //Procesamos las acciones
        for (int i=0;i<objMenu->getSize();i++){
            object = objMenu->getObjByPos(i);

            if (execFunc && object != NULL){
                if (!object->isPopup()){
                    switch(object->getObjectType()){
                        case GUIBUTTON:
                            //En el caso de los botones tambien procesamos sus eventos
                            botonPulsado = procesarBoton(object, objMenu);
                            estado = evento->mouse_state;
                            if (botonPulsado && ( (evento->isMouse && estado == SDL_RELEASED) || evento->isKey || evento->isJoy)){ //Comprobamos si se ha pulsado el elemento
                                posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
                                if (posBoton >= 0){ //Si hemos encontrado una funcion
                                    if (this->pt2Func[posBoton] != NULL){
                                        //Forzamos a que se actualicen todos los elementos
                                        objMenu->resetElements();
                                        /**Los botones no pueden hacer que se salga de la aplicacion. Solo ejecutamos la funcion*/
                                        //salir = (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        Traza::print("procesarControles: Evento lanzado para " + object->getName(), W_INFO);
                                    }
                                }
                            }
                            break;
                        case GUILISTBOX:
                        case GUIPROGRESSBAR:
                        case GUISLIDER:
                        case GUIPOPUPMENU:
                        case GUILISTGROUPBOX:
                        case GUICOMBOBOX:
                        case GUITEXTELEMENTSAREA:
                        case GUIPICTURE:
                            if (procesarBoton(object, objMenu)){ //Comprobamos si se ha pulsado el elemento
                                posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
                                if (posBoton >= 0){ //Si hemos encontrado una funcion
                                    if (this->pt2Func[posBoton] != NULL){
                                        //Forzamos a que se actualicen todos los elementos
                                        objMenu->resetElements();
                                        salir = (*this.*pt2Func[posBoton])(evento); //Ejecutamos la funcion especificada en el puntero a funcion almacenado
                                        Traza::print("procesarControles: Evento lanzado para " + object->getName(), W_INFO);
                                    }
                                }
                            }
                            break;
                        default:
                            break;
                    } // FIN CASE
                }
            } // FIN IF
//            //Finalmente dibujamos el objeto
            /**Aqui iba el dibujado del objeto para aprovechar el bucle pero esto
            * daba problemas. Se corrige para que tenga su propio bucle al inicio
            * de las acciones
            */
            //Comprobamos si el objeto esta lanzando un evento para cambiar el mouse.
            //Solo aceptamos el primer objeto que pida cambiar el mouse
            if (object->getCursor() >= 0){
                updateCursor = true;
                if (cursorPrincipal < 0){
                    cursorPrincipal = object->getCursor();
                }
            }
        }
        if (!updateCursor){
            //Reseteamos el cursor al que hay por defecto
            cursorPrincipal = cursor_arrow;
        }

        //Dibujamos el cursor solo si procede
        this->pintarCursor(evento->mouse_x, evento->mouse_y, cursorPrincipal);
        //Comprobar si al cerrar
        this->ComprobarPopupsCerrados(objMenu, evento);


    } catch (Excepcion &e) {
         Traza::print("Excepcion procesarControles: " + string(e.getMessage()), W_ERROR);
    }
    return salir;
}




/**
*
*/
int Iofrontend::accionesMenu(tEvento *evento){
    bool salir = false;

    try{
        int menu = this->getSelMenu();
        tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
        Object *object = objsMenu->getObjByPos(objsMenu->getFocus());

        if (object != NULL){
            if (object->getObjectType() == GUILISTBOX ||
                object->getObjectType() == GUIPOPUPMENU ||
                object->getObjectType() == GUILISTGROUPBOX){

                UIListCommon *objList = (UIListCommon *)object;
                unsigned int pos = objList->getPosActualLista();
                string valorSelec = objList->getValue(pos);
                int destino = objList->getDestino(pos);
                objList->setImgDrawed(false);

                if (valorSelec.compare("salir") == 0){
                    salir = true;
                } else if (valorSelec.compare("shutdown") == 0 or valorSelec.compare("reboot") == 0){
                    Launcher *launcher = new Launcher();
                    FileLaunch *emulInfo = new FileLaunch();
                    string option = "";

                    #ifdef UNIX
                        if (valorSelec.compare("reboot") == 0){
                            option = "-r";
                        } else if(valorSelec.compare("shutdown") == 0){
                            option = "-h";
                        }

                        emulInfo->fileexe = "sudo shutdown " +  option + " now";
                        emulInfo->parmsexe = "";
                    #elif WIN
                        if (valorSelec.compare("reboot") == 0){
                            option = "-r";
                        } else if(valorSelec.compare("shutdown") == 0){
                            option = "-s";
                        }

                        flipScr();
                        emulInfo->rutaexe = Constant::TipoToStr(getenv("windir")) + "\\system32";
                        emulInfo->fileexe = "shutdown.exe";
                        emulInfo->parmsexe = option + " -f -t 1";
                    #endif

                    string pregunta = "";
                    if (valorSelec.compare("reboot") == 0){
                        pregunta = "¿Esta seguro de que desea reiniciar la maquina?";
                    } else if(valorSelec.compare("shutdown") == 0){
                        pregunta = "¿Esta seguro de que desea apagar la maquina?";
                    }

                    bool confirmed = casoPANTALLACONFIRMAR("Confirmacion", pregunta);

                    if (confirmed){
                        if (valorSelec.compare("reboot") == 0){
                            showMessage("El sistema se va a reiniciar. Espere un momento...", 1);
                        } else if(valorSelec.compare("shutdown") == 0){
                            showMessage("El sistema se va a apagar. Espere un momento...", 1);
                        }
                        Traza::print("Antes de ejecutar programa", W_DEBUG);
                        launcher->lanzarProgramaUNIXFork(emulInfo);
                        salir = true;
                    }

                    delete emulInfo;
                    delete launcher;

                } else if (destino != -1){
                    this->cargaMenuFromLista(objList, evento);
                }
            }
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion accionesMenu" + string(e.getMessage()), W_ERROR);
    }
    return salir;
}

/**
*
*/
void Iofrontend::cargaMenuFromLista(UIListCommon *obj, tEvento *evento){

    if (obj->getPosActualLista() < 0){
        obj->setPosActualLista(0);
    } else {
        int menucarga = obj->getDestino(obj->getPosActualLista());
        string valorSelec = obj->getValue(obj->getPosActualLista());
        //Si hemos pulsado el boton de volver (que es el ultimo de la lista)
        //reiniciamos la posicion por si volvemos a entrar
        if (obj->getPosActualLista() >= obj->getSize() - 1){
            obj->setPosActualLista(0);
            obj->calcularScrPos();
        }
        //Ahora cargamos el siguiente menu
        cargaMenu(menucarga, valorSelec, evento);
    }
}

/**
*
*/
void Iofrontend::cargaMenu(int menucarga, string valorSelec, tEvento *evento){
    comprobarUnicode(menucarga);
    if (menucarga >= 0 && menucarga <= MAXMENU){
        this->setSelMenu(menucarga);
        //Damos el foco al primer elemento que haya en el menu
        this->ObjectsMenu[menucarga]->setFirstFocus();
    }

    tmenu_gestor_objects *objsMenu = ObjectsMenu[menucarga];
    UIListCommon *objTemp = NULL;

    switch (menucarga){

        case MENUJUEGOS :
            //cargarListaEmuladores(menucarga, PANTALLAROMS, "ListaMenuJuegos");
            cargarListaEmuladores(menucarga, PANTALLAGROUPLIST, "ListaMenuJuegos");
            break;

        case MENUOPCIONES:
            if (valorSelec.compare("scan") == 0){
                actualizarRoms();
            } else if (valorSelec.compare("importRetroarch") == 0){
                importRetroarchConfig();
            } else if (valorSelec.compare("configJoy") == 0){
                casoJOYBUTTONS(evento);
            } else if (valorSelec.compare("scrapAll") == 0){
                scrapAllRoms();
            }
            break;

        case PANTALLAROMS:
            cargarListaRoms(menucarga, valorSelec, "ListaMenuRoms");
            break;

        case PANTALLAGROUPLIST:
            cargarListaRoms(menucarga, valorSelec, "listaGrupoRoms");
            objsMenu->setFocus("listaGrupoRoms");
            break;

        case PANTALLAEDITAREMU:
            cargarListaEmuladores(menucarga, PANTALLAOPCIONRUTAS, "listaEditarEmus");
            break;

        case PANTALLAOPCIONRUTAS:
            if (valorSelec.compare("") != 0){
                cargarDatosEmulador(valorSelec);
                objsMenu->getObjByName("btnAceptarEmu")->setLabel("Modificar");
                objsMenu->getObjByName("btnEliminarEmu")->setVisible(true);
            } else {
                clearEmuFields();
                objsMenu->getObjByName("btnAceptarEmu")->setLabel("Aceptar");
                objsMenu->getObjByName("btnEliminarEmu")->setVisible(false);
            }

            objsMenu->getObjByName("btnAceptarEmu")->setTag(valorSelec);
            objsMenu->getObjByName("btnEliminarEmu")->setTag(valorSelec);
            break;

        case LANZARROM:
            objTemp = (UIListCommon *)ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("listaGrupoRoms");
            valorSelec = objTemp->getValue(objTemp->getPosActualLista());
            Traza::print("LANZANDO ROM con id: " + valorSelec, W_DEBUG);
            lanzarPrograma(valorSelec);
            this->setSelMenu(PANTALLAGROUPLIST);
            break;
        case PANTALLAOPENMEDIA:
            playMedia(evento);
            break;
        case PANTALLAREPRODUCTOR:
            break;
        case MENUOPCIONESGENERALES:
            cargarOpcionesGenerales();
            break;
        default:
            break;
    }
}

/**
*
*/
long Iofrontend::waitMedia(){
    unsigned long before = 0;
    long max_ = 0;
    bool playtimeout = false;
    //El reproductor tarda un tiempo en saber el tamanyo del fichero a reproducir
    //con lo que tenemos que esperarnos hasta que devuelva el total en segundos
    before = SDL_GetTicks();
    do{
        max_ = player.getTotalSecs();
        playtimeout = (SDL_GetTicks() - before > 5000);
    } while (max_ == 0 && !playtimeout);

    return max_;
}

/**
*
*/
void Iofrontend::playMedia(tEvento *evento){
    ignoreButtonRepeats = true;
    //Lanzamos la ventana del explorador. Bloquea hasta que se selecciona un fichero
    string file = showExplorador(evento);
    //Intentamos ejecutar el fichero
    Traza::print("Reproduciendo: " + file, W_DEBUG);
    if (file.empty()){
        this->setSelMenu(MENUINICIAL);
    } else {
        player.setScreen(screen);
        clearScr(cNegro);
        Traza::print("player.play(file): " + file, W_DEBUG);
        player.play(file);
        //Ejecutamos el bucle del reproductor
        bucleReproductor(file);
        this->setSelMenu(MENUINICIAL);
        Traza::print("FIN de reproduccion", W_DEBUG);
    }
}

/**
*
*/
int Iofrontend::accionesCargaPantalla(tEvento *evento){
    int posMenu = accionesGotoPantalla(evento);
    if (posMenu >= 0){
        cargaMenu(posMenu, "", evento);
        return true;
    }
    return false;
}



/**
* El campo checked del control se da valor desde la llamada al procEvent del respectivo menu
* En esta funcion se comprueba el valor checked para saber si se ha pulsado el elemento
*/
bool Iofrontend::procesarBoton(Object * obj, tmenu_gestor_objects *gestorMenu){
    try{
        //Traza::print("Checkeando name: " + string(name) + " valor: " + string(objMenu->getObjByName(name)->isChecked()?"S":"N"), W_ERROR);
        if (obj->getObjectType() == GUICOMBOBOX){
            if (((UIComboBox *)obj)->isValueChanged()){
                ((UIComboBox *)obj)->setValueChanged(false);
                ((UIComboBox *)obj)->setChecked(false);
                ((UIComboBox *)obj)->setFocus(false);
                gestorMenu->findNextFocus();
                return true;
            }
        } else if (obj->isChecked()){
            obj->setChecked(false);
            return true;
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion procesarBoton" + string(e.getMessage()), W_ERROR);
    }
    return false;
}
/**
* Se simula que se ha pulsado la tecla Intro o el boton aceptar del joystick
*/
void Iofrontend::simularEscape(tEvento *evento){
    evento->isKey = true;
    evento->key = SDLK_ESCAPE;
    evento->isJoy = true;
    evento->joy = JOY_BUTTON_START;
}

/**
* Se simula que se ha pulsado la tecla Intro o el boton aceptar del joystick
*/
void Iofrontend::simularIntro(tEvento *evento){
    evento->isKey = true;
    evento->key = SDLK_RETURN;
    evento->isJoy = true;
    evento->joy = JOY_BUTTON_A;
}

/**
*
*/
int Iofrontend::marcarBotonSeleccionado(tEvento *evento){

    int menu = getSelMenu();
    tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
    int pos = objMenu->getFocus();

    Traza::print("marcarBotonSeleccionado: " + objMenu->getObjByPos(pos)->getName(), W_DEBUG);
    if (pos >= 0){
        objMenu->getObjByPos(pos)->setTag("selected");
        return true;
    }
    return false;
}

/**
* Se asigna la funcion especificada al boton con el nombre especificado.
* No deberian haber botones del mismo nombre
*/
void Iofrontend::addEvent(string nombre, typept2Func funcion){
    addEvent(nombre, funcion, -1);
}

/**
*
*/
void Iofrontend::addEvent(string nombre, typept2Func funcion, int parms){
    if (propertiesPt2Func.size < MAXMENU*MAXOBJECTS){
        propertiesPt2Func.name.push_back(nombre);
        propertiesPt2Func.parms.push_back(Constant::TipoToStr(parms));
        pt2Func[propertiesPt2Func.size] = funcion;
        propertiesPt2Func.size++;
    }
}

/**
* Se busca la posicion del puntero a funcion asociado con el nombre del boton
*/
int Iofrontend::findEventPos(string nombre){

    int i=0;

    while (i < propertiesPt2Func.size){
        if (nombre.compare(propertiesPt2Func.name[i]) == 0)
            return i;
        i++;
    }
    return -1;
}

/**
* Se busca la posicion del puntero a funcion asociado con el nombre del boton y se modifica
*/
void Iofrontend::setEvent(string nombre, typept2Func funcion){
    int pos = findEventPos(nombre);
    if (pos != -1){
        pt2Func[pos] = funcion;
    }
}

void Iofrontend::setEvent(string nombre, typept2Func funcion, int parms){
    int pos = findEventPos(nombre);
    if (pos != -1){
        pt2Func[pos] = funcion;
        propertiesPt2Func.parms[pos] = Constant::TipoToStr(parms);
    }
}


/**
*
*/
void Iofrontend::popUpMenu(tEvento evento){
}

/**
* Con el menu pasado por parametro lo dibujamos entero
*/
void Iofrontend::resizeMenu(){
    for (int i=0; i< MAXMENU; i++){
        ObjectsMenu[i]->setAreaObjMenu(getWidth(),getHeight());
    }
    setDinamicSizeObjects();
}

/**
*
*/
void Iofrontend::setDinamicSizeObjects(){
    try{
        //Calculamos el tamanyo del titulo de los elementos que lo tengan, y redimensionamos el elemento
        //lista que tenga ese menu con el total de la ventana que queda
        for (int i=0; i<MAXMENU; i++){
            try{
                ObjectsMenu[i]->getObjByName(TITLESCREEN)->setTam( 0, 0, this->getWidth(),Constant::getINPUTH());
                int j = 0;
                Object *posibleObj = NULL;

                while (j < ObjectsMenu[i]->getSize()){
                    posibleObj = ObjectsMenu[i]->getObjByPos(j);
                    if(posibleObj != NULL){
                        if (posibleObj->getObjectType() == GUILISTBOX || ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUIPICTURE
                            || posibleObj->getObjectType() == GUILISTGROUPBOX){
                            posibleObj->setTam(0,Constant::getINPUTH(), this->getWidth(), this->getHeight()-Constant::getINPUTH());
                        }

                        if (ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUILISTBOX){
                            ((UIList *)posibleObj)->calcularScrPos();
                        } else if (ObjectsMenu[i]->getObjByPos(j)->getObjectType() == GUILISTGROUPBOX){
                            ((UIListGroup *)posibleObj)->calcularScrPos();
                        }
                    }
                    j++;
                }
            } catch (Excepcion &e){}
        }

        ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("listaGrupoRoms")->setTam(this->getWidth() / 2 , Constant::getINPUTH(), this->getWidth() / 2, this->getHeight()-Constant::getINPUTH());


        int widthForImgBox = (this->getWidth() / 4 > 250) ? 250 : this->getWidth() / 4;
        Constant::setIMGBOXARTWIDTH(widthForImgBox);
        Constant::setIMGBOXARTHEIGHT(widthForImgBox);

        ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("ImgBoxArt")->setTam(IMGBOXARTMARGIN,
                                                                          Constant::getINPUTH() + Constant::getMENUSPACE() + INPUTCONTENT,
                                                                          Constant::getIMGBOXARTWIDTH(), Constant::getIMGBOXARTHEIGHT());

        UITextElementsArea *infoTextRom = (UITextElementsArea *)ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("textosBox");
        infoTextRom->setTam(0, Constant::getINPUTH(), this->getWidth() / 2, this->getHeight()- Constant::getINPUTH());

        int margenX = Constant::getIMGBOXARTWIDTH() + IMGBOXARTMARGIN + 3;
        t_posicion pos = {margenX, Constant::getMENUSPACE() + IMGBOXARTMARGIN,0,0};
        infoTextRom->setPosition("txtReleased",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtPlayers",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtPublisher",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtDeveloper",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtGenre",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtRating",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtLastPlayed",pos);
        pos.y += Constant::getINPUTH();
        infoTextRom->setPosition("txtTimesPlayed",pos);
        t_posicion posDesc = {0, Constant::getINPUTH() + Constant::getIMGBOXARTHEIGHT() + IMGBOXARTMARGIN * 2,0,0};
        infoTextRom->setPosition("txtDescripcion",posDesc);


        //Redimension para el browser de directorios2
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(OBJLISTABROWSER2)->setTam(0, Constant::getINPUTH() + COMBOHEIGHT + 4,this->getWidth(), this->getHeight() - BUTTONH - Constant::getINPUTH() - COMBOHEIGHT - 10 - 4);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName("comboBrowser")->setTam(1, Constant::getINPUTH() + 4, 160, 100);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(BTNACEPTARBROWSER)->setTam( (this->getWidth() / 2) -(BUTTONW + 5), this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(BTNCANCELARBROWSER)->setTam( (this->getWidth() / 2) + 5, this->getHeight() - BUTTONH - 5, BUTTONW,BUTTONH);
        ObjectsMenu[PANTALLABROWSER2]->getObjByName(ARTDIRBROWSER)->setTam( 0, 0, this->getWidth(), Constant::getINPUTH());

        //Redimension para la pantalla de videos multimedia
        int desp = (this->getWidth() / 2) - FAMFAMICONW*2 - BUTTONW/2;
        int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("panelMedia")->setTam(0, calculaPosPanelMedia(), this->getWidth(), this->getHeight() - calculaPosPanelMedia());
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnBackward")->setTam(desp += (BUTTONW/2), bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnPlay")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnStop")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("btnForward")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarMedia")->setTam( TIMEW + SEPTIMER, bottom - PROGRESSSEPBOTTOM, this->getWidth() - TIMEW*2 - SEPTIMER*2, PROGRESSHEIGHT);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimerTotal")->setTam(SEPTIMER, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
        ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("mediaTimer")->setTam(this->getWidth() - TIMEW, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
    } catch (Excepcion &e){
        Traza::print("setDinamicSizeObjects: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
bool Iofrontend::casoPANTALLACONFIRMAR(string titulo, string txtDetalle){
    ignoreButtonRepeats = true;
    Traza::print("casoPANTALLACONFIRMAR: Inicio", W_PARANOIC);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    bool salida = false;
    int menuInicial = getSelMenu();

    //Procesamos el menu antes de continuar para que obtengamos la captura
    //de pantalla que usaremos de fondo
    procesarControles(ObjectsMenu[menuInicial], &askEvento, NULL);
    SDL_Rect iconRectFondo = {0, 0, this->getWidth(), this->getHeight()};
    SDL_Surface *mySurface = NULL;
    takeScreenShot(&mySurface, iconRectFondo);

    //Seguidamente cambiamos la ñpantalla a la de la confirmacion
    setSelMenu(PANTALLACONFIRMAR);
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLACONFIRMAR];
    objMenu->getObjByName("borde")->setLabel(titulo);
    objMenu->getObjByName("labelConfirm")->setLabel(txtDetalle);
    int txtLen = fontStrLen(txtDetalle);
    objMenu->getObjByName("labelConfirm")->setW(txtLen);

    //Generamos el tamanyo de la ventana que formara el popup

    long delay = 0;
    unsigned long before = 0;
    objMenu->setFocus(0);

    do{
        before = SDL_GetTicks();
        askEvento = WaitForKey();
//        clearScr(cBgScreen);
        printScreenShot(&mySurface, iconRectFondo);
        drawRectAlpha(iconRectFondo.x, iconRectFondo.y, iconRectFondo.w, iconRectFondo.h , cNegro, 200);

        procesarControles(objMenu, &askEvento, NULL);

        flipScr();
        salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
        (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

        if (objMenu->getObjByName("btnSiConfirma")->getTag().compare("selected") == 0){
            salir = true;
            salida = true;
            objMenu->getObjByName("btnSiConfirma")->setTag("");
            Traza::print("Detectado SI pulsado", W_DEBUG);
        } else if (objMenu->getObjByName("btnNoConfirma")->getTag().compare("selected") == 0){
            salir = true;
            salida = false;
            objMenu->getObjByName("btnNoConfirma")->setTag("");
            Traza::print("Detectado NO pulsado", W_DEBUG);
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);

    setSelMenu(menuInicial);
    return salida;
}

/**
*
*/
string Iofrontend::casoPANTALLAPREGUNTA(string titulo, string label){
    ignoreButtonRepeats = true;
    Traza::print("casoPANTALLAPREGUNTA: Inicio", W_PARANOIC);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    string salida = "";
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAPREGUNTA];
    objMenu->getObjByName("valor")->setLabel(label);
    objMenu->getObjByName("borde")->setLabel(titulo);
    long delay = 0;
    unsigned long before = 0;


    do{
        before = SDL_GetTicks();
        askEvento = WaitForKey();
        clearScr(cBgScreen);
        procesarControles(objMenu, &askEvento, NULL);

        flipScr();
        salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
        (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

        UIInput *input = (UIInput *)objMenu->getObjByName("valor");

        if (input->getSize() > 0 &&
            ( (askEvento.isKey && askEvento.key == SDLK_RETURN) || (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_A))) ){
            salida = input->getText();
            salir = true;
        }

        delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
        if(delay > 0) SDL_Delay(delay);
    } while (!salir);

    return salida;
}


/**
*
*/
int Iofrontend::accionesGotoPantalla(tEvento *evento){
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *object = objsMenu->getObjByPos(objsMenu->getFocus());

    int posBoton = findEventPos(object->getName());  //Buscamos la posicion del elemento en el array de punteros a funcion
    if (posBoton >= 0){ //Si hemos encontrado una funcion
        int posMenu = Constant::strToTipo<int>(propertiesPt2Func.parms[posBoton]);
        if (posMenu >= 0){
            this->setSelMenu(posMenu);
            this->ObjectsMenu[posMenu]->findNextFocus();
            comprobarUnicode(posMenu);
            return posMenu;
        }
    }
    return false;
}

/**
* En este metodo se comprueba si tenemos que activar unicode para que
* las teclas del teclado se traduzcan sin problemas de dependencia de layouts
*/
void Iofrontend::comprobarUnicode(int menu){

    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    int i=0;
    bool found = false;

    while (i < objsMenu->getSize() && !found){

        if (objsMenu->getObjByPos(i) != NULL)
            if (objsMenu->getObjByPos(i)->getObjectType() == GUIINPUTWIDE)
                found = true;
        i++;
    }

    Traza::print("comprobarUnicode: " + Constant::TipoToStr(menu) + ((found == true) ? " UNICODE=S":" UNICODE=N"), W_DEBUG);
    SDL_EnableUNICODE(found);
}

/**
* Establece el nombre del fichero o directorio seleccionado por el explorador de
* ficheros, en el contenido un campo especificado por parámetro
*/
void Iofrontend::setTextFromExplorador(tEvento *evento, UIInput *objCampoEdit){
    try{
        Dirutil dir;
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        //Si el objeto ya tiene datos, comprobamos si existe el directorio que supuestamente contiene
        string uri = dir.getFolder(objCampoEdit->getText());
        if (dir.existe(uri)){
            //Si resulta que existe, hacemos un cambio de directorio para que se muestre
            //el contenido del directorio nada mas pulsar en el boton
            dir.changeDirAbsolute(uri.c_str());
        }
        //Abrimos el explorador de archivos y esperamos a que el usuario seleccione un fichero
        //o directorio
        string fichName = showExplorador(evento);
        //Si se ha seleccionado algo, establecemos el texto en el objeto que hemos recibido por parametro
        if (!fichName.empty()){
            objCampoEdit->setText(fichName);
            objMenu->setFocus(objCampoEdit->getName());
        }
    } catch (Excepcion &e){
        Traza::print("setTextFromExplorador: " + string(e.getMessage()), W_ERROR);
    }
}

/**
*
*/
string Iofrontend::showExplorador(tEvento *evento){
    Traza::print("showExplorador: Inicio", W_INFO);
    bool salir = false;
    tEvento askEvento;
    clearEvento(&askEvento);
    int menuInicio = this->getSelMenu();
    this->setSelMenu(PANTALLABROWSER2);
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLABROWSER2];
    UIList *obj = NULL;
    ignoreButtonRepeats = true;
    string fileUri = "";
    string fileTempSelec = "";
    Dirutil dir;
    static string lastDirOpened;

    try{
        loadComboUnidades("comboBrowser", PANTALLABROWSER2, -1);
        obj = (UIList *)objMenu->getObjByName(OBJLISTABROWSER2);
        obj->setFocus(true);
        obj->setTag("");
        obj->setPosActualLista(-1);
        //Forzamos a que se actualicen todos los elementos
        objMenu->resetElements();
        //Seleccionamos a la lista que esta en primer lugar
        //objMenu->findNextFocus();
        ObjectsMenu[PANTALLABROWSER2]->setFocus(OBJLISTABROWSER2);

        if (!lastDirOpened.empty()){
            dir.changeDirAbsolute(dir.getFolder(lastDirOpened).c_str());
        }

        long delay = 0;
        unsigned long before = 0;
        this->accionesListaExplorador(evento);

        do{
            before = SDL_GetTicks();
            askEvento = WaitForKey();
            clearScr(cBlanco);
            int pos = obj->getPosActualLista();
            //Carga de imagenes de fondo en la pantalla del explorador de ficheros
            if (pos >= 0){
                fileTempSelec = obj->getListNames()->get(pos);
                string ruta = dir.getDirActual() +  Constant::getFileSep() + fileTempSelec;
                UIPicture *objPict = (UIPicture *)objMenu->getObjByName("ImgFondo");

                if (objPict->getImgGestor()->getRuta().compare(ruta) != 0){
                    if (dir.findIcon(fileTempSelec.c_str()) == page_white_picture){
                        objPict->loadImgFromFile(ruta);
                        objPict->getImgGestor()->setBestfit(false);
                    } else {
                        if (objPict->getImgGestor()->clearFile())
                            objPict->setImgDrawed(false);
                    }
                    obj->setImgDrawed(false);
                    objMenu->getObjByName(BTNACEPTARBROWSER)->setImgDrawed(false);
                    objMenu->getObjByName(BTNCANCELARBROWSER)->setImgDrawed(false);
                    objPict->getImgGestor()->setRuta(ruta);
                }
            }

            procesarControles(objMenu, &askEvento, NULL);
            if (objMenu->getFocus() < 0){
                objMenu->findNextFocus();
            }

            //fps();
            flipScr();
            salir = (askEvento.isJoy && askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_B)) ||
            ( ((askEvento.isKey && askEvento.key == SDLK_ESCAPE) || !obj->getTag().empty())
             || objMenu->getObjByName(BTNACEPTARBROWSER)->getTag().compare("selected") == 0
             || objMenu->getObjByName(BTNCANCELARBROWSER)->getTag().compare("selected") == 0);


            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            if(delay > 0) SDL_Delay(delay);
        } while (!salir);

    } catch (Excepcion &e){
        Traza::print("Error en showExplorador: " + string(e.getMessage()), W_ERROR);
    }

    this->setSelMenu(menuInicio);
    string fileSelec;
    string diractual = dir.getDirActual();

    if (obj != NULL){
        // En el caso de que se haya pulsado el boton aceptar, obtenemos el elemento seleccionado
        if (objMenu->getObjByName(BTNACEPTARBROWSER)->getTag().compare("selected") == 0){
            int pos = obj->getPosActualLista();
            if (pos >= 0){
                fileSelec = obj->getListNames()->get(pos);
                bool tieneFileSep = diractual.substr(diractual.length()-1).compare(Constant::getFileSep()) == 0;
                obj->setTag(diractual + (!tieneFileSep ? Constant::getFileSep() : "") + fileSelec);
            }
        }
        fileUri = obj->getTag();
    }

    try{
        objMenu->getObjByName(BTNCANCELARBROWSER)->setTag("");
        objMenu->getObjByName(BTNACEPTARBROWSER)->setTag("");
    } catch (Excepcion &e){
        Traza::print("showExplorador: reseteando botones: " + string(e.getMessage()), W_ERROR);
    }

    //No queremos que se seleccionen directorios incorrectos
    if (fileSelec.compare("..") == 0){
//        obj->setTag("");
//        showMessage("Directorio no valido.", 2000);
//        fileUri = showExplorador(evento);
        fileUri = fileUri.substr(0, fileUri.find_last_of(tempFileSep));
    }
    lastDirOpened = fileUri;
    return fileUri;
}

/**
*
*/
int Iofrontend::accionesListaExplorador(tEvento *evento){

    string fileSelec = "";

    try{
        Traza::print("Iofrontend::accionesListaExplorador", W_INFO);
        tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLABROWSER2];
        UIList * obj = (UIList *)objMenu->getObjByName(OBJLISTABROWSER2);
        Dirutil dir;
        bool dirChanged = false;
        string diractual;
        int pos = obj->getPosActualLista();

        if (evento == NULL){
            pos = 0;
            dirChanged = true;
        } else if (pos >= 0){
            string fileSelec = obj->getListNames()->get(pos);
            string valorSelec = obj->getListValues()->get(pos);
            Traza::print("cambiando al directorio: " + fileSelec, W_DEBUG);
            if (Constant::strToTipo<int>(valorSelec) == TIPODIRECTORIO){
                dirChanged = dir.changeDirRelative(fileSelec.c_str());
            } else {
                diractual = dir.getDirActual();
                obj->setTag(diractual + tempFileSep + fileSelec);
            }
        }
        //Reseteamos la posicion del explorador para el siguiente directorio
        obj->setPosActualLista(0);
        obj->calcularScrPos();
        //Actualizamos la barra principal con la ruta actual
        diractual = dir.getDirActual();

        ((UIArt *)objMenu->getObjByName(ARTDIRBROWSER))->setLabel(diractual);

        if (dirChanged || pos < 0){
            //Obtenemos el directorio actual y sus elementos
            listaSimple<FileProps> *filelist = new listaSimple<FileProps>();
            unsigned int numFiles = dir.listarDir(diractual.c_str(), filelist);
            Traza::print("Ficheros: " + Constant::TipoToStr(numFiles), W_DEBUG);

            if (filelist != NULL && numFiles > 0){
                //Hacemos espacio en la lista para que la asignacion sea rapida
                obj->resizeLista(numFiles);
                //Recorremos la lista de ficheros y lo incluimos en el objeto de lista para mostrar los datos
                for (unsigned int i = 0; i < numFiles; i++){
                    obj->addElemLista(filelist->get(i).filename , Constant::TipoToStr(filelist->get(i).filetype), filelist->get(i).ico );
                }
            } else {
               obj->resizeLista(1);
               obj->addElemLista(".." , Constant::TipoToStr(TIPODIRECTORIO), folder);
            }
            delete filelist;
        }


    }catch (Excepcion &e){
        Traza::print("accionesListaExplorador: " + string (e.getMessage()), W_ERROR);
    }

    ObjectsMenu[PANTALLABROWSER2]->setFocus(OBJLISTABROWSER2);
    return true;
}

/**
*
*/
void Iofrontend::loadComboUnidades(string objName, int pantalla, int types){
    Traza::print("Iofrontend::loadComboUnidades", W_INFO);
    UIList *combo = (UIList *)ObjectsMenu[pantalla]->getObjByName(objName);
    combo->clearLista();
    combo->setPosActualLista(0);
    vector<t_drive *> drives;

    Dirutil dir;
    dir.getDrives(&drives);
    int actualDrive = 0;
    string actualDir = dir.getDirActual();

    for (int i=0; i < drives.size(); i++){
        if (types == -1 || types == drives.at(i)->driveType){
            combo->addElemLista(drives.at(i)->drive.substr(0,2)
                                + " (" + drives.at(i)->driveTypeString + ") "
                                + drives.at(i)->label, drives.at(i)->drive, drives.at(i)->ico);
            if (actualDir.find(drives.at(i)->drive) != string::npos){
                actualDrive = i;
            }
        }
    }
    combo->setPosActualLista(actualDrive);
    combo->calcularScrPos();
}

/**
*
*/
int Iofrontend::accionCombo(tEvento *evento){
    Traza::print("Iofrontend::accionCombo", W_INFO);
    UIComboBox *combo = (UIComboBox *)ObjectsMenu[PANTALLABROWSER2]->getObjByName("comboBrowser");
    string unidad = combo->getValue(combo->getPosActualLista());
    Traza::print("Iofrontend::accionCombo. Drive: " + unidad, W_DEBUG);
    Dirutil dir;
    bool cambioDir = dir.changeDirAbsolute(unidad.c_str());

    if (cambioDir){
        Traza::print("Iofrontend::accionCombo. EXITO Drive: " + unidad, W_DEBUG);
    } else {
        Traza::print("Iofrontend::accionCombo. ERROR Drive: " + unidad, W_ERROR);
    }

    clearEvento(evento);
    this->accionesListaExplorador(NULL);
    ObjectsMenu[PANTALLABROWSER2]->setFocus(OBJLISTABROWSER2);
    return 0;
}

/**
*
*/
void Iofrontend::ComprobarPopupsCerrados(tmenu_gestor_objects *objMenu, tEvento *evento){
    //Comprobacion para evitar problemas con popups
    Object *object = objMenu->getObjByPos(objMenu->getFocus());
    if (object != NULL){
        if (object->isPopup() == false && object->isFocus() == false && evento
            && objMenu->getObjByName(object->getName())->getObjectType() == GUIPOPUPMENU)
        {
            if (evento->isMouse && (evento->mouse == MOUSE_BUTTON_RIGHT || evento->mouse == MOUSE_BUTTON_LEFT)
                && evento->mouse_state == SDL_PRESSED)
            {
                UIPopupMenu *objPopup = (UIPopupMenu *)objMenu->getObjByName(object->getName());
                objMenu->setFocus(objPopup->getCallerPopup()->getName());
                Traza::print("ComprobarPopupsCerrados. Foco a: " + objPopup->getCallerPopup()->getName(), W_DEBUG);
            }
        }
    }
}



/***************************************************************************************************************/
/**                                ACCIONES DE LOS CAMPOS DE LA APLICACION                                     */
/***************************************************************************************************************/


/**
* Carga el resultado de la seleccion del explorador de archivos en un campo de texto
* - El objeto boton que llama a este metodo debe tener el campo tag rellenado con el campo de destino
*   En el que se quiere que se guarde el resultado
*/
int Iofrontend::loadDirFromExplorer(tEvento *evento){

    try{
        //Obtenemos los objetos del menu actual
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        //Obtenemos el objeto que ha sido seleccionado y que tiene el foco
        Object *obj = objMenu->getObjByPos(objMenu->getFocus());
        //Obtenemos el tag del elemento que indica en que campo deberemos dar valor a la seleccion
        //que hagamos del explorador de archivos
        string tag = obj->getTag();
        if (!tag.empty()){
            setTextFromExplorador(evento, (UIInput *)objMenu->getObjByName(tag));
        } else {
            Traza::print("loadDirFromExplorer. El objeto: " + obj->getLabel() + " no tiene tag asociado " +
                         "que indique donde cargar el texto del explorador", W_ERROR);
        }
    } catch (Excepcion &e){
        Traza::print("loadDirFromExplorer: " + string(e.getMessage()), W_ERROR);
    }
    return 0;
}

/**
* Lanza el emulador para configurarlo manualmente
*/
int Iofrontend::launchEmuForConfig(tEvento *evento){
    try{
        Traza::print("Lanzando EMU", W_DEBUG);
        //Obtenemos los objetos del menu actual
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        //Obtenemos el objeto que ha sido seleccionado y que tiene el foco
        Object *obj = objMenu->getObjByPos(objMenu->getFocus());
        //Obtenemos la ruta del emulador que estamos intentando lanzar para configurarlo
        string rutaEmu = ((UIInput *)objMenu->getObjByName("EmuRuta"))->getText();
        Dirutil *dirio = new Dirutil();

//        if (dirio->existe(rutaEmu)){
            //Una vez obtenida la ruta, lanzamos el programa
            Launcher *launcher = new Launcher();
            FileLaunch emulInfo;
            killSDL();
            emulInfo.rutaexe = dirio->getFolder(rutaEmu);
            emulInfo.fileexe = dirio->getFileName(rutaEmu);
            emulInfo.rutaroms = "";
            emulInfo.nombrerom = "";
            emulInfo.parmsexe = "";
            emulInfo.descomprimir = false;
            Traza::print("Antes de ejecutar programa", W_DEBUG);
            launcher->lanzarProgramaUNIXFork(&emulInfo);
            this->clearLastEvento();
            Traza::print("Iniciando SDL", W_DEBUG);
            initSDL(false);
            Traza::print("SDL Iniciado con exito", W_DEBUG);
            delete launcher;

//        } else {
//            Traza::print("launchEmuForConfig: No existe el programa en la ruta indicada: " + rutaEmu, W_ERROR);
//            showMessage("No existe el programa en la ruta indicada", 3000);
//        }
        delete dirio;


    } catch (Excepcion &e){
        Traza::print("launchEmuForConfig: " + string(e.getMessage()), W_ERROR);
    }
    return 0;
}


/**
*
*/
int Iofrontend::accionesBtnEliminarEmu(tEvento *evento){

    string pregunta = "¿Está seguro de que desea eliminar el programa?";
    bool confirmed = casoPANTALLACONFIRMAR("Confirmacion", pregunta);
    if (confirmed){
        tmenu_gestor_objects *objMenu = ObjectsMenu[this->getSelMenu()];
        Object *obj = objMenu->getObjByName("btnEliminarEmu");
        gestorRoms->deleteEmulador(Constant::strToTipo<int>(obj->getTag()));
        this->setSelMenu(PANTALLAEDITAREMU);
        cargarListaEmuladores(PANTALLAEDITAREMU, PANTALLAOPCIONRUTAS, "listaEditarEmus");
    }
    return 0;
}

/**
*
*/
int Iofrontend::accionesBtnVolverConfigEmu(tEvento *evento){
    tmenu_gestor_objects *objsMenu = ObjectsMenu[this->getSelMenu()];
    string tag = objsMenu->getObjByName("btnCancelarEmu")->getTag();

    if (tag.compare("FROMEMULIST") == 0){
        objsMenu->getObjByName("btnCancelarEmu")->setTag("");
        this->setSelMenu(MENUJUEGOS);
        ObjectsMenu[MENUJUEGOS]->setFirstFocus();
    } else if (tag.compare("FROMEMUGROUPLIST") == 0){
        objsMenu->getObjByName("btnCancelarEmu")->setTag("");
        this->setSelMenu(PANTALLAGROUPLIST);
        ObjectsMenu[PANTALLAGROUPLIST]->setFirstFocus();
    } else {
        this->setSelMenu(PANTALLAEDITAREMU);
        objsMenu = ObjectsMenu[this->getSelMenu()];
        objsMenu->findNextFocus();
        cargarListaEmuladores(PANTALLAEDITAREMU, PANTALLAOPCIONRUTAS, "listaEditarEmus");
    }
    //Seleccionamos a que se seleccione el primer elemento del menu
    return 0;
}

/**
* Acciones del boton aceptar de la alta o modificacion del emulador
* si el tag del boton de aceptar viene informado, significa que estamos modificando
* y debemos realizar un update en la bbdd. En caso contrario realizamos un insert
*/
int Iofrontend::accionesBtnAceptarOpciones(tEvento *evento){

    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAOPCIONRUTAS];
    string tagBotonAceptar = objMenu->getObjByName("btnAceptarEmu")->getTag();

    try{
        Emuinfo emuProps;
        emuProps.emuProperties.nombreEmu  = ((UIInput *)objMenu->getObjByName("EmuName"))->getText();
        emuProps.emuProperties.rutaEmu    = ((UIInput *)objMenu->getObjByName("EmuRuta"))->getText();
        emuProps.emuProperties.parmsEmu   = ((UIInput *)objMenu->getObjByName("EmuParms"))->getText();
        emuProps.emuProperties.rutaRoms   = ((UIInput *)objMenu->getObjByName("EmuRutaRom"))->getText();
        emuProps.emuProperties.emuRomExt  = ((UIInput *)objMenu->getObjByName("EmuRomExtension"))->getText();
        emuProps.emuProperties.descomp    = objMenu->getObjByName("CheckDescomprimir")->isChecked();
        emuProps.emuProperties.titleRom   = objMenu->getObjByName("CheckFindTitleRom")->isChecked();
        emuProps.emuProperties.idEmu      = tagBotonAceptar;
        emuProps.emuProperties.rutaImg    = ((UIInput *)objMenu->getObjByName("EmuImg"))->getText();
        //Obtenemos el valor seleccionado en el combo
        emuProps.emuProperties.fixOption  = objMenu->getObjByName("CheckComboFixes")->isChecked();
        emuProps.emuProperties.platform  = ((UIComboBox *)objMenu->getObjByName("ComboPlatform"))->getSelectedValue();

        if (emuProps.emuProperties.nombreEmu.empty() || emuProps.emuProperties.rutaEmu.empty() || emuProps.emuProperties.rutaRoms.empty()){
            showMessage("Falta rellenar alguno de los campos", 2000);
        } else {
            if (emuProps.emuProperties.idEmu.compare("") == 0){
                if (gestorRoms->insertEmulador(&emuProps.emuProperties)){
                    showMessage("Emulador insertado correctamente", 2000);
                    clearEmuFields();
                }
            } else {
                if (gestorRoms->updateEmulador(&emuProps.emuProperties)){
                    showMessage("Emulador modificado correctamente", 2000);
                }
            }
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion accionesBtnAceptarOpciones" + string(e.getMessage()), W_ERROR);
    }
    return 0;
}

/**
*
*/
void Iofrontend::clearEmuFields(){
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAOPCIONRUTAS];
    ((UIInput *)objMenu->getObjByName("EmuName"))->setText("");
    ((UIInput *)objMenu->getObjByName("EmuRuta"))->setText("");
    ((UIInput *)objMenu->getObjByName("EmuParms"))->setText("");
    ((UIInput *)objMenu->getObjByName("EmuRutaRom"))->setText("");
    ((UIInput *)objMenu->getObjByName("EmuRomExtension"))->setText("");
    ((UIInput *)objMenu->getObjByName("EmuImg"))->setText("");
    objMenu->getObjByName("CheckDescomprimir")->setChecked(false);
    objMenu->getObjByName("CheckFindTitleRom")->setChecked(false);
    objMenu->getObjByName("CheckComboFixes")->setChecked(false);
    ((UIComboBox *)objMenu->getObjByName("ComboPlatform"))->setPosActualLista(0);
    objMenu->setFocus("EmuName");
}

/**
*
*/
bool Iofrontend::cargarDatosEmulador(string idprog){
    gestorRoms->getDb()->prepareStatement("selectEmulador");
    gestorRoms->getDb()->setString(0,idprog);
    vector<vector<string> > result = gestorRoms->getDb()->executeQuery();

    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAOPCIONRUTAS];
    for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
        vector<string> row = *it;
        ((UIInput *)objMenu->getObjByName("EmuName"))->setText(row.at(1));
        ((UIInput *)objMenu->getObjByName("EmuRuta"))->setText(row.at(2));
        ((UIInput *)objMenu->getObjByName("EmuParms"))->setText(row.at(3));
        ((UIInput *)objMenu->getObjByName("EmuRutaRom"))->setText(row.at(4));
        ((UIInput *)objMenu->getObjByName("EmuRomExtension"))->setText(row.at(5));
        objMenu->getObjByName("CheckDescomprimir")->setChecked((row.at(6).compare("S") == 0 ? true : false));
        ((UIInput *)objMenu->getObjByName("EmuImg"))->setText(row.at(7));
        objMenu->getObjByName("CheckFindTitleRom")->setChecked((row.at(8).compare("S") == 0 ? true : false));
        objMenu->getObjByName("CheckComboFixes")->setChecked((row.at(9).compare("S") == 0 ? true : false));

        if (row.at(10).empty()){
            ((UIComboBox *)objMenu->getObjByName("ComboPlatform"))->setPosActualLista(0);
        } else {
            ((UIComboBox *)objMenu->getObjByName("ComboPlatform"))->selectValueInList(row.at(10));
        }


    }
    return false;
}

/**
*
*/
bool Iofrontend::cargarListaEmuladores(int menu, int destino, string lista){
    try{
        tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
        UIList *obj = (UIList *)objMenu->getObjByName(lista);
        Traza::print("Cargando lista de emuladores de: " + dirInicial, W_DEBUG);
        gestorRoms->fillMenuByQuery(obj, "selectListaEmuladores",NULL, destino);

        if (lista.compare("listaEditarEmus") == 0){
            obj->addElemLista("Configuración", "", bullet_go, MENUOPCIONES);
        } else {
            obj->addElemLista("Configuración", "", bullet_go, MENUINICIAL);
        }

    } catch (Excepcion &e) {
         Traza::print("Excepcion cargarListaEmuladores" + string(e.getMessage()), W_ERROR);
    }
    return false;
}

/**
*
*/
bool Iofrontend::cargarListaRoms(int menu, string idprog, string lista){
    try{
        tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
        //Obtenemos el nombre del emulador que seleccionamos en la pantalla anterior
        UIList *objEmu = (UIList *)ObjectsMenu[MENUJUEGOS]->getObjByName("ListaMenuJuegos");
        //Cargamos la imagen de fondo del emulador
        string imgruta = gestorRoms->getImgEmulador(Constant::strToTipo<int>(idprog));
        Traza::print("cargarListaRoms. imgruta: " + imgruta, W_PARANOIC);

        UIPicture * objImg = (UIPicture *) objMenu->getObjByName("ImgEmulador");
        if (!imgruta.empty())
            objImg->loadImgFromFile(imgruta.c_str());
        else
            objImg->getImgGestor()->clearFile();

        objImg->setImgDrawed(false);
        //Realizamos la query para obtener las roms que se mostraran en la pantalla de seleccion de roms
        std::vector<string> parms;
        parms.push_back(idprog);

        UIListCommon *objLista = (UIListCommon *)objMenu->getObjByName(lista);
        gestorRoms->fillMenuByQuery(objLista, "selectListaRoms", &parms, LANZARROM);
        Object *titulo = objMenu->getObjByName(TITLESCREEN);
        titulo->setLabel(objEmu->getListNames()->get(objEmu->getPosActualLista()) + " - " + Constant::TipoToStr(objLista->getSize()) + " juegos");

        if (objLista->getObjectType() == GUILISTBOX){
            ((UIList *)objLista)->addElemLista("Volver", "idprog=" + idprog, bullet_go, MENUJUEGOS);
        } else if (objLista->getObjectType() == GUILISTGROUPBOX){
            vector <ListGroupCol *> miFila;
            miFila.push_back(new ListGroupCol("Volver", "idprog=" + idprog, bullet_go, MENUJUEGOS));
            miFila.push_back(new ListGroupCol("", "", bullet_go, MENUJUEGOS));
            ((UIListGroup *)objLista)->addElemLista(miFila);
        }
        setInfoRomValues();


    } catch (Excepcion &e) {
         Traza::print("Excepcion cargarListaRoms" + string(e.getMessage()), W_ERROR);
    }
    return false;
}

/**
*
*/
bool Iofrontend::actualizarRoms(){
    try{
        ignoreButtonRepeats = true;
        //tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
        showMessage("Actualizando roms. Espere un momento...", 1);

        Thread<Gestorroms> *thread = new Thread<Gestorroms>(gestorRoms, &Gestorroms::actualizarRoms);
        if (thread->start())
            std::cout << "Thread started with id: " << thread->getThreadID() << std::endl;
        //thread->join();
        //pintarIconoProcesando(thread);
        pintarIconoProcesando(true);
        while(thread->isRunning()){
            pintarIconoProcesando(false);
        }
        clearScr();
        delete thread;

    } catch (Excepcion &e) {
         Traza::print("Excepcion actualizarRoms" + string(e.getMessage()), W_ERROR);
    }
    return false;
}



/**
*
*/
bool Iofrontend::importRetroarchConfig(){
    ignoreButtonRepeats = true;
    showMessage("Importando configuracion. Espere un momento...", 1);
    Thread<Gestorroms> *thread = new Thread<Gestorroms>(gestorRoms, &Gestorroms::importRetroarchConfig);
    if (thread->start())
        std::cout << "Thread started with id: " << thread->getThreadID() << std::endl;
    //thread->join();
    pintarIconoProcesando(true);
    while(thread->isRunning()){
        pintarIconoProcesando(false);
    }

    clearScr();
    actualizarRoms();

    delete thread;
    return false;
}

/**
*
*/
bool Iofrontend::lanzarPrograma(string claves){
    ignoreButtonRepeats = true;
    string sqlwhere = "";

    if (claves.find(",SCRAPPED=") != string::npos){
        claves = claves.substr(0, claves.find(",SCRAPPED="));
    }

    sqlwhere = gestorRoms->parserSQLWhere(claves, "r");

    string sql = "select e.RUTAEMU, e.PARMSEMU, r.NOMBRE, e.RUTAROMS || ru.RUTA, e.DESCOMPRIMIR, e.SEPARARUTAPARM, e.FIXOPTION";
    sql.append(" from ROMS r, EMULADOR e, RUTAS ru ");
    sql.append(" where r.IDPROG = e.IDPROG and r.IDPROG = ru.IDPROG  and r.IDRUTA = ru.IDRUTA");
    sql.append(sqlwhere);

    Traza::print("Ejecutando query: " + sql, W_DEBUG);
    vector<vector<string> > result = gestorRoms->getRowQuery(sql);

    FileLaunch emulInfo;
    Dirutil *dirio = new Dirutil();
    Launcher *launcher = new Launcher();
    killSDL();

    if (result.size() > 0){
        vector<string> row = result.at(0);
        emulInfo.rutaexe = dirio->getFolder(row.at(0));
        emulInfo.fileexe = dirio->getFileName(row.at(0));
        emulInfo.rutaroms = row.at(3);
        emulInfo.nombrerom = row.at(2);
        emulInfo.parmsexe = row.at(1);
        emulInfo.descomprimir = (row.at(4) == "S" ? true : false );
        emulInfo.fixoption = (row.at(6) == "S" ? true : false );
        //Realizamos comprobaciones para lanzar el programa. Util para copiar dll's
        //que activan directdraw en windows 8
        comprobarFixesSO(&emulInfo);
        //Lanzamos el programa
        Traza::print("Antes de ejecutar programa", W_DEBUG);
        launcher->lanzarProgramaUNIXFork(&emulInfo);
        this->clearLastEvento();
    }
    Traza::print("Iniciando SDL", W_DEBUG);
    initSDL(false);
    //Repintamos la imagen de fondo de la pantalla de lanzamiento de las roms
    ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("ImgEmulador")->setImgDrawed(false);
    drawUIPicture(ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("ImgEmulador"));

    Traza::print("SDL Iniciado con exito", W_DEBUG);
    return false;
}

/**
*
*/
void Iofrontend::comprobarFixesSO(FileLaunch *emulInfo){
    Traza::print("Aplicando fixes de SO: " + emulInfo->fixoption, W_DEBUG);
    Dirutil dir;

    //Copiaremos la dll correspondiente al directorio del emulador
    string dllOrigen = dirInicial + Constant::getFileSep() + "fixesOS" + Constant::getFileSep();
    string dllDestino = emulInfo->rutaexe + Constant::getFileSep() + "ddraw.dll";
    string fixddraw = gestorRoms->getParameter("FIXDDRAW");

    if (fixddraw == "1"){
        //Se aplica fix para windows 8
        dllOrigen = dllOrigen + "ddraw8.dll";
    } else if (fixddraw == "2"){
        //Se aplica fix para windows 8.1
        dllOrigen = dllOrigen + "ddraw8_1.dll";
    }

    if (emulInfo->fixoption && dir.existe(dllOrigen) && !dir.isDir(dllOrigen)){
        Traza::print("Copiando dll desde: " + dllOrigen + " a: " + dllDestino, W_DEBUG);
        //Copiamos la dll al directorio del emulador
        dir.copyFile(dllOrigen, dllDestino);
    } else if (dir.existe(dllDestino) && !dir.isDir(dllDestino)){
        Traza::print("Borrando dll de: " + dllDestino, W_DEBUG);
        //No hay fix, por lo que deberiamos eliminar el fichero
        dir.borrarArchivo(dllDestino);
    }
}


/**
*
*/
string Iofrontend::casoJOYBUTTONS(tEvento *evento){
    ignoreButtonRepeats = true;
    configButtonsJOY(evento);
    return "";
}

/**
*
*/
int Iofrontend::accionesMediaAvanzar(tEvento *evento){
    player.salto10PercentForward();
    return false;
}

/**
*
*/
int Iofrontend::accionesMediaRetroceder(tEvento *evento){
    player.salto10PercentBackwards();
    return false;
}

/**
*
*/
int Iofrontend::accionesMediaPause(tEvento *evento){
    player.pause();
    return false;
}

/**
*
*/
int Iofrontend::accionesMediaStop(tEvento *evento){
    simularEscape(evento);
    return false;
}

/**
*
*/
int Iofrontend::mediaClicked(tEvento *evento){
    UIProgressBar * objProg = (UIProgressBar *)ObjectsMenu[PANTALLAREPRODUCTOR]->getObjByName("progressBarMedia");

    Traza::print("Pos pulsada media: ", objProg->getProgressPos(), W_DEBUG);
    float pos = 0;
    if (objProg->getProgressMax() > 0)
        pos = objProg->getProgressPos() / (float)objProg->getProgressMax();

    player.setPositionPercent(pos);

    return false;
}

/**
*
*/
void Iofrontend::setPanelMediaVisible(bool var){
    try{
        tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
        obj->getObjByName("panelMedia")->setVisible(var);
        obj->getObjByName("btnBackward")->setVisible(var);
        obj->getObjByName("btnPlay")->setVisible(var);
        obj->getObjByName("btnStop")->setVisible(var);
        obj->getObjByName("btnForward")->setVisible(var);
        obj->getObjByName("progressBarMedia")->setVisible(var);
        obj->getObjByName("mediaTimerTotal")->setVisible(var);
        obj->getObjByName("mediaTimer")->setVisible(var);
        if (!var) {
            t_region regionPantalla = {0 , calculaPosPanelMedia(), getWidth(), getHeight() - calculaPosPanelMedia()};
            clearScr(cNegro, regionPantalla);
        }

    } catch (Excepcion &e){
        Traza::print("setPanelMediaVisible: " + string(e.getMessage()), W_ERROR);
    }
}
/**
*
*/
bool Iofrontend::bucleReproductor(string file){

    bool salir = false;
    long delay = 0;
    unsigned long before = 0;
    unsigned long timer1s = 0;
    unsigned long timerPanelMedia = 0;
    ignoreButtonRepeats = true;
    tEvento askEvento;
    long max_ = 0;
    tmenu_gestor_objects *obj = ObjectsMenu[PANTALLAREPRODUCTOR];
    bool panelMediaVisible = false;

    //El reproductor tarda un tiempo en saber el tamanyo del fichero a reproducir
    //con lo que tenemos que esperarnos hasta que devuelva el total en segundos
    max_ = waitMedia();
    //De primeras no tenemos ni idea del tamanyo del video, pero despues de esperar
    //con la funcion anterior lo obtenemos, lo que nos obliga a parar el video y volver a redimensionar
    //los surfaces.
    //Recalculamos el tamanyo que deberia tener el video para caber por pantalla
    player.setSizeToVideo();
    //Una vez que hemos obtenido el tamanyo del video, paramos el reproductor y lo volvemos a iniciar
    player.stop();
    player.destroy();
    player.setObjsMenuVideo(obj);
    player.play(file);
    waitMedia();
    //Una vez sabemos el maximo de tiempo, damos valor a la barra de progreso con el maximo de segundos
    //y al label para mostrar el total de tiempo de la pelicula
    UIProgressBar *objProg = (UIProgressBar *)obj->getObjByName("progressBarMedia");
    objProg->setProgressMax(max_);
    obj->getObjByName("mediaTimerTotal")->setLabel(player.getTimeTotal());

    clearEvento(&askEvento);
    timer1s = SDL_GetTicks();
    timerPanelMedia = timer1s;
    setPanelMediaVisible(false);

    tscreenobj screenEvents;
    screenEvents.drawComponents = false;
    screenEvents.execFunctions = true;

    do{
            //Dibujamos el video por pantalla
            player.blitVideo();
            before = SDL_GetTicks();

            //Procesamos los controles de la aplicacion
            askEvento = WaitForKey();

            //Actualizamos el indicador de la barra de progreso y del tiempo actual
            if (before - timer1s > 500 && panelMediaVisible){
                obj->getObjByName("mediaTimer")->setLabel(player.getTimePos());
                objProg->setProgressPos(max_ * player.getPositionPercent());
                timer1s = before;
            }
            //Si debemos mostrar el menu de la barra de progreso
            if (before - timerPanelMedia > 8000){
                //Si no ha habido evento, deshabilitamos la barra
                if (panelMediaVisible){
                    panelMediaVisible = false;
                    setPanelMediaVisible(false);
                }
            } else {
                //Si ha habido evento, habilitamos la barra
                if (!panelMediaVisible){
                  panelMediaVisible = true;
                  setPanelMediaVisible(true);
                }
            }

            //Procesamos los eventos para cada elemento que pintamos por pantalla
            procesarControles(obj, &askEvento, &screenEvents);
            //Si pulsamos escape, paramos la ejecucion
            salir = (askEvento.isKey && askEvento.key == SDLK_ESCAPE);

            if ((askEvento.isKey && askEvento.key == SDLK_SPACE) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_START)){
                player.pause();
                timerPanelMedia = SDL_GetTicks();
            } else if ((askEvento.isKey && askEvento.key == SDLK_RIGHT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_RIGHT)){
                player.salto10PercentForward();
                objProg->setProgressPos(max_ * player.getPositionPercent());
                timerPanelMedia = SDL_GetTicks();
            } else if ((askEvento.isKey && askEvento.key == SDLK_LEFT) || askEvento.joy == JoyMapper::getJoyMapper(JOY_BUTTON_LEFT)){
                player.salto10PercentBackwards();
                objProg->setProgressPos(max_ * player.getPositionPercent());
                timerPanelMedia = SDL_GetTicks();
            } else if (askEvento.resize){
                float pos = player.getPositionPercent();
                player.stop();
                player.destroy();
                player.setScreen(screen);
                clearScr(cNegro);
                player.play(file);
                waitMedia();
                player.setPositionPercent(pos);
                setDinamicSizeObjects();
            } else if (askEvento.isMouseMove){
                if (askEvento.mouse_y > calculaPosPanelMedia()){
                    timerPanelMedia = SDL_GetTicks();
                }
            } else if (askEvento.quit){
                salir = true;
            }

            flipScr();
            delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;

            if(delay > 0) SDL_Delay(delay);
        } while (!salir);

        //Cerramos el reproductor multimedia y reseteamos la barra de progreso
        player.stop();
        player.destroy();
        objProg->setProgressMax(0);
        objProg->setProgressPos(0);

    return true;
}

/**
*
*/
int Iofrontend::calculaPosPanelMedia(){
    int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
    return (bottom - PROGRESSSEPBOTTOM - SEPPANELMEDIA);
}

/**
*
*/
UIPopupMenu * Iofrontend::addPopup(int pantalla, string popupName, string callerName){
    UIPopupMenu * popup1 = NULL;

    try{
        tmenu_gestor_objects *objMenu = ObjectsMenu[pantalla];
        objMenu->getObjByName(callerName)->setPopupName(popupName);
        ObjectsMenu[pantalla]->add(popupName, GUIPOPUPMENU, 0, 0, 100, 100, popupName, false)->setVisible(false);
        popup1 = (UIPopupMenu *) objMenu->getObjByName(popupName);
        popup1->setFont(getFont());
        popup1->setAutosize(true);
    } catch (Excepcion &e){
        Traza::print("addPopup: " + string(e.getMessage()), W_ERROR);
    }

    return popup1;
}

/**
* Desde este menu podremos mostrar por pantalla la informacion de las roms de cada emulador
*/
bool Iofrontend::procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento){

    static unsigned long lastClick = 0;
    static unsigned long lastTitleUpdate = 0;
    static int pos = -1;


    try{
        if (SDL_GetTicks() - lastTitleUpdate > 1000){
            UIArt *titulo = (UIArt *) ObjectsMenu[this->getSelMenu()]->getObjByName(TITLESCREEN);

            size_t posTextScrapping = titulo->getLabel().find(" | ");
            string label = titulo->getLabel();
            if (posTextScrapping != string::npos){
                label = label.substr(0, posTextScrapping);
            }

            if (gestorRoms->isScrappingNow()){
                titulo->setLabel(label + " | " + "Scrapping: " + gestorRoms->getPlatform() + " - " + gestorRoms->getProgress()) ;
            } else {
                titulo->setLabel(label);
            }
            lastTitleUpdate = SDL_GetTicks();
        }
    } catch (Excepcion &e){
        //No realizamos ninguna accion en el catch porque no es necesario
    }



    if (this->getSelMenu() == PANTALLAGROUPLIST){
        //Implementamos un retraso para cargar la informacion de los emuladores, sino se intentaria cargar
        //para cada pulsacion del teclado, lo que podria sobrecargar el sistema

        if (SDL_GetTicks() - lastClick > 200 && SDL_GetTicks() - lastClick < 2000){
            UIListGroup * listaGrupo = (UIListGroup *) ObjectsMenu[PANTALLAGROUPLIST]->getObjByName("listaGrupoRoms");
            if (pos != listaGrupo->getPosActualLista()){
                setInfoRomValues();
                pos = listaGrupo->getPosActualLista();
            }
        }
        //Procesamos los eventos del teclado
        if (evento->isKey || evento->isJoy || (evento->isMouse && evento->mouse_state == SDL_PRESSED &&
                                               (evento->mouse == MOUSE_BUTTON_WHEELDOWN || evento->mouse == MOUSE_BUTTON_WHEELUP
                                                || evento->mouse == MOUSE_BUTTON_LEFT)) ){
            if (evento->key == SDLK_UP || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_UP) || (evento->mouse == MOUSE_BUTTON_WHEELUP)
                       || evento->key == SDLK_DOWN || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_DOWN) || evento->mouse == MOUSE_BUTTON_WHEELDOWN
                       || evento->key == SDLK_PAGEDOWN || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_R) || evento->key == SDLK_PAGEUP
                       || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_L) || evento->key == SDLK_END || evento->key == SDLK_HOME
                       || (evento->isMouse && evento->mouse == MOUSE_BUTTON_LEFT && evento->mouse_state == SDL_PRESSED)){
                //Guardamos el momento en el que hemos hecho click para procesar la accion posteriormente. VER LLAMADA a setInfoRomValues();
                //de mas arriba
                lastClick = SDL_GetTicks();
            } else if (evento->key == SDLK_F3 && evento->keyjoydown || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_SELECT)){
                tmenu_gestor_objects *objMenu = ObjectsMenu[getSelMenu()];
                UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
                string rom = listaGrupo->getValue(listaGrupo->getSize() - 1);

                showMenuEmergente(PANTALLAOPCIONESROM, "ImgFondoRoms");
                objMenu = ObjectsMenu[PANTALLAOPCIONESROM];
                objMenu->getObjByName("btnScrapGame")->setTag(rom);
            }
        }
    }
}

/**
* Muestra el menu pasado por parametro como si fuera emergente, dejando el menu anterior
* difuminado sobre el fondo
*/
void Iofrontend::showMenuEmergente(int menu, string objImagenFondo){
    try{
        //Procesamos el menu actual para que se vuelva a repintar
        procesarControles(ObjectsMenu[getSelMenu()], new tEvento(), NULL);
        //Seleccionamos el menu que queremos mostrar como si fuese emergente
        tmenu_gestor_objects *objMenu = ObjectsMenu[menu];
        //Realizamos una captura de pantalla en el objeto imagen de fondo del menu de destino
        UIPicture * fondoImg = (UIPicture *) objMenu->getObjByName(objImagenFondo);
        //Le damos un toque mas oscuro a la imagen
        fondoImg->setAlpha(150);
        //Obtenemos una captura de la pantalla entera
        takeScreenShot(&fondoImg->getImgGestor()->surface);
        //Seleccionamos el menu de destino y procesamos sus controles para que se pinten
        setSelMenu(menu);
        tEvento askEvento;
        clearEvento(&askEvento);
        procesarControles(objMenu, &askEvento, NULL);
    } catch (Excepcion &e) {
         Traza::print("Excepcion showMenuEmergente: " + string(e.getMessage()), W_ERROR);
    }

}

/**
*
*/
DWORD Iofrontend::setInfoRomValues(){
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAGROUPLIST];
    UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
    UITextElementsArea *textElems = (UITextElementsArea *)objMenu->getObjByName("textosBox");
    textElems->setOffsetDesplazamiento(0);
    textElems->setImgDrawed(false);
    int pos = listaGrupo->getPosActualLista();
    if (pos >= 0 && pos < listaGrupo->getSize()-1 && listaGrupo->isShowLetraPopup() == false ){
        string value = listaGrupo->getValue(pos);
        Traza::print(value, W_PARANOIC);
        vector <string> infoRom = Constant::split(value,",");

        string idprog = Constant::split(infoRom.at(0), "=").at(1);
        string idrom = Constant::split(infoRom.at(1), "=").at(1);

        Traza::print("idrom: " + idrom + ";" + "idprog: " + idprog, W_PARANOIC);
        gestorRoms->getDb()->prepareStatement("selectInfoRom");
        gestorRoms->getDb()->setString(0,idprog);
        gestorRoms->getDb()->setString(1,idrom);

        vector<vector<string> > result = gestorRoms->getDb()->executeQuery();


        if (result.size() > 0){
            vector<string> row = result.at(0);
            textElems->setFieldText("txtPlayers", row.at(0));
            textElems->setFieldText("txtGenre", row.at(2));
            textElems->setFieldText("txtReleased", row.at(3));
            textElems->setFieldText("txtLastPlayed", "0");
            textElems->setFieldText("txtTimesPlayed", "0");
            textElems->setFieldText("txtDescripcion", row.at(5));
            textElems->setFieldText("txtPublisher", row.at(6));
            textElems->setFieldText("txtDeveloper", row.at(7));
            textElems->setFieldText("txtRating", row.at(8));

            string directory = Constant::getAppDir()
            + FILE_SEPARATOR + DIR_IMG_ROMS_IMGS;

            if (!row.at(4).empty() && row.at(4).at(0) == FILE_SEPARATOR){
                directory.append(row.at(4));
            } else {
                directory.append(FILE_SEPARATOR + idprog);
                directory.append(FILE_SEPARATOR + row.at(4));
            }

            UIPicture* pict = ((UIPicture*) objMenu->getObjByName("ImgBoxArt"));
            pict->clearImg();
            pict->loadImgFromFile(directory);
            pict->setCentrado(true);
            pict->setBestfit(true);

        } else {
            textElems->setFieldText("txtPlayers", "");
            textElems->setFieldText("txtGenre", "");
            textElems->setFieldText("txtReleased", "");
            textElems->setFieldText("txtLastPlayed", "0");
            textElems->setFieldText("txtTimesPlayed", "0");
            textElems->setFieldText("txtDescripcion", "");
            textElems->setFieldText("txtPublisher", "");
            textElems->setFieldText("txtDeveloper", "");
            textElems->setFieldText("txtRating", "");
            ((UIPicture*) objMenu->getObjByName("ImgBoxArt"))->clearImg();
        }
    }
    return 1;
}

/**
*
*/
bool Iofrontend::procesarPopups(tmenu_gestor_objects *objMenu, tEvento *evento){
        Object *object = objMenu->getObjByPos(objMenu->getFocus());
        if (object != NULL){
            try{
                //Comprobamos si el elemento que estamos pintando deberia mostrar su menu de popup
//                Traza::print("object->isPopup()",object->isPopup(), W_DEBUG);
//                Traza::print("object->isFocus()",object->isFocus(), W_DEBUG);
//                Traza::print("object->getName(): " + object->getName(), W_DEBUG);

                if (object->isPopup() && object->isFocus()){
                    Traza::print("procesarPopups", W_DEBUG);
                    //Obtenemos el objeto popup
                    UIPopupMenu *objPopup = (UIPopupMenu *)objMenu->getObjByName(object->getPopupName());
                    //Mostramos el popup
                    objPopup->setVisible(true);
                    //Seteamos la posicion del menu popup
                    if (evento->mouse_x > 0 && evento->mouse_y > 0){
                        //Si no tenemos espacio horizontal por la derecha, intentamos mostrar el popup por
                        //la izquierda del lugar clickado
                        if (evento->mouse_x + objPopup->getW() > this->getWidth()){
                            objPopup->setX(evento->mouse_x - objPopup->getW());
                        } else {
                            objPopup->setX(evento->mouse_x);
                        }
                        //Si no tenemos espacio vertical por debajo, intentamos mostrar el popup por
                        //encima del lugar clickado
                        if (evento->mouse_y + objPopup->getH() > this->getHeight()){
                            objPopup->setY(evento->mouse_y - objPopup->getH());
                        } else {
                            objPopup->setY(evento->mouse_y);
                        }
                    }
                    //Asignamos el elemento que ha llamado al popup
                    objPopup->setCallerPopup(objMenu->getObjByPos(objMenu->getFocus()));
                    //Damos el foco al popup
                    objMenu->setFocus(object->getPopupName());
                    //Evitamos que el elemento procese cualquier evento mientras aparezca el popup
                    object->setEnabled(false);
                    //Forzamos a que se refresque el elemento padre que lanzo el popup
                    object->setImgDrawed(false);
                }
            } catch (Excepcion &e) {
                Traza::print("Excepcion en popup: " + object->getPopupName() + ". " + string(e.getMessage()), W_ERROR);
            }
        }

    return true;
}



/**
* Copia el texto seleccionado desde un popup al elemento que lo llama. Por ahora solo lo hace
* en campos input.
*/
int Iofrontend::accionCopiarTextoPopup(tEvento *evento){
    Traza::print("accionCopiarTextoPopup", W_DEBUG);
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        string selected = objPopup->getListValues()->get(objPopup->getPosActualLista());
        if (objPopup->getCallerPopup() != NULL){
            //Obtenemos el objeto llamador
            if (objPopup->getCallerPopup()->getObjectType() == GUIINPUTWIDE){
                UIInput *objInput = (UIInput *)objPopup->getCallerPopup();
                //Anyadimos el texto al input
                objInput->setText(objInput->getText() + selected);
                //Devolvemos el foco al elemento que llamo al popup
                objsMenu->setFocus(objPopup->getCallerPopup()->getName());
            }
        }
    }
    return 0;
}

/**
* Configura el emulador
*/
int Iofrontend::accionConfigEmusPopup(tEvento *evento){
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    //Comprobamos que efectivamente, el elemento es un popup
    if (obj->getObjectType() == GUIPOPUPMENU){
        UIPopupMenu *objPopup = (UIPopupMenu *)obj;
        //Obtenemos el valor del elemento seleccionado en el popup
        int selElemPopup = objPopup->getPosActualLista();
        if (objPopup->getCallerPopup() != NULL){
            UIListCommon *objList = (UIListCommon *)objPopup->getCallerPopup();
            string codEmu = "0";
            string origen = "";

            if (objPopup->getCallerPopup()->getObjectType() == GUILISTBOX){
                codEmu = objList->getValue(objList->getPosActualLista());
                origen = "FROMEMULIST";
            } else if (objPopup->getCallerPopup()->getObjectType() == GUILISTGROUPBOX){
                string elemSel = objList->getValue(objList->getPosActualLista());
                int posIni = elemSel.find("IDPROG=") + strlen("IDPROG=");
                int posFin = elemSel.find(",",posIni);
                codEmu = elemSel.substr(posIni, posFin - posIni);
                origen = "FROMEMUGROUPLIST";
            }

            if (selElemPopup == 0){
                cargaMenu(PANTALLAOPCIONRUTAS, codEmu, NULL);
                tmenu_gestor_objects *objsMenu = ObjectsMenu[PANTALLAOPCIONRUTAS];
                objsMenu->getObjByName("btnCancelarEmu")->setTag(origen);
            } else if (selElemPopup == 1){
                if (objPopup->getCallerPopup()->getObjectType() == GUILISTBOX){
                    scrapEmuRoms(codEmu);
                } else if (objPopup->getCallerPopup()->getObjectType() == GUILISTGROUPBOX){
                    scrapEmuRoms(codEmu);
                }
            }
        }
    }
    return 0;
}

/**
*
*/
void Iofrontend::crearComboSistemas(){
    tmenu_gestor_objects *objMenu  = ObjectsMenu[PANTALLAOPCIONRUTAS];
    UIComboBox * comboObj = (UIComboBox *) objMenu->getObjByName("ComboPlatform");
    comboObj->addElemLista("- Elige -","");
    comboObj->addElemLista("3DO","3DO");
    comboObj->addElemLista("Amiga","Amiga");
    comboObj->addElemLista("Amstrad CPC","Amstrad CPC");
    comboObj->addElemLista("Android","Android");
    comboObj->addElemLista("Arcade","Arcade");
    comboObj->addElemLista("Atari 2600","Atari 2600");
    comboObj->addElemLista("Atari 5200","Atari 5200");
    comboObj->addElemLista("Atari 7800","Atari 7800");
    comboObj->addElemLista("Atari Jaguar","Atari Jaguar");
    comboObj->addElemLista("Atari Jaguar CD","Atari Jaguar CD");
    comboObj->addElemLista("Atari Lynx","Atari Lynx");
    comboObj->addElemLista("Atari XE","Atari XE");
    comboObj->addElemLista("Colecovision","Colecovision");
    comboObj->addElemLista("Commodore 64","Commodore 64");
    comboObj->addElemLista("Fairchild Channel F","Fairchild Channel F");
    comboObj->addElemLista("Final Burn Alpha","fba");
    comboObj->addElemLista("Intellivision","Intellivision");
    comboObj->addElemLista("iOS","iOS");
    comboObj->addElemLista("Mac OS","Mac OS");
    comboObj->addElemLista("Magnavox Odyssey 2","Magnavox Odyssey 2");
    comboObj->addElemLista("Mame","mame");
    comboObj->addElemLista("Microsoft Xbox","Microsoft Xbox");
    comboObj->addElemLista("Microsoft Xbox 360","Microsoft Xbox 360");
    comboObj->addElemLista("Microsoft Xbox One","Microsoft Xbox One");
    comboObj->addElemLista("MSX","MSX");
    comboObj->addElemLista("Neo Geo Pocket","Neo Geo Pocket");
    comboObj->addElemLista("Neo Geo Pocket Color","Neo Geo Pocket Color");
    comboObj->addElemLista("NeoGeo","NeoGeo");
    comboObj->addElemLista("Nintendo 3DS","Nintendo 3DS");
    comboObj->addElemLista("Nintendo 64","Nintendo 64");
    comboObj->addElemLista("Nintendo DS","Nintendo DS");
    comboObj->addElemLista("Nintendo (NES)","Nintendo Entertainment System (NES)");
    comboObj->addElemLista("Nintendo Game Boy","Nintendo Game Boy");
    comboObj->addElemLista("Nintendo Game Boy Advance","Nintendo Game Boy Advance");
    comboObj->addElemLista("Nintendo Game Boy Color","Nintendo Game Boy Color");
    comboObj->addElemLista("Nintendo GameCube","Nintendo GameCube");
    comboObj->addElemLista("Nintendo Virtual Boy","Nintendo Virtual Boy");
    comboObj->addElemLista("Nintendo Wii","Nintendo Wii");
    comboObj->addElemLista("Nintendo Wii U","Nintendo Wii U");
    comboObj->addElemLista("Ouya","Ouya");
    comboObj->addElemLista("PC","PC");
    comboObj->addElemLista("Philips CD-i","Philips CD-i");
    comboObj->addElemLista("Sega 32X","Sega 32X");
    comboObj->addElemLista("Sega CD","Sega CD");
    comboObj->addElemLista("Sega Dreamcast","Sega Dreamcast");
    comboObj->addElemLista("Sega Game Gear","Sega Game Gear");
    comboObj->addElemLista("Sega Genesis","Sega Genesis");
    comboObj->addElemLista("Sega Master System","Sega Master System");
    comboObj->addElemLista("Sega Mega Drive","Sega Mega Drive");
    comboObj->addElemLista("Sega Saturn","Sega Saturn");
    comboObj->addElemLista("Sinclair ZX Spectrum","Sinclair ZX Spectrum");
    comboObj->addElemLista("Sony Playstation","Sony Playstation");
    comboObj->addElemLista("Sony Playstation 2","Sony Playstation 2");
    comboObj->addElemLista("Sony Playstation 3","Sony Playstation 3");
    comboObj->addElemLista("Sony Playstation 4","Sony Playstation 4");
    comboObj->addElemLista("Sony Playstation Vita","Sony Playstation Vita");
    comboObj->addElemLista("Sony PSP","Sony PSP");
    comboObj->addElemLista("Super Nintendo (SNES)","Super Nintendo (SNES)");
    comboObj->addElemLista("TurboGrafx 16","TurboGrafx 16");
    comboObj->addElemLista("WonderSwan","WonderSwan");
    comboObj->addElemLista("WonderSwan Color","WonderSwan Color");
}

/**
*
*/
int Iofrontend::accionesBtnAceptarOpcionesGenerales(tEvento *evento){
    tmenu_gestor_objects *objMenu = ObjectsMenu[MENUOPCIONESGENERALES];
    try{
        string valueFixes  = ((UIComboBox *)objMenu->getObjByName("ComboFixesGeneral"))->getSelectedValue();
        if (gestorRoms->updateParameter("FIXDDRAW", valueFixes)){
            showMessage("Opciones modificadas", 2000);
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion accionesBtnAceptarOpcionesGenerales" + string(e.getMessage()), W_ERROR);
    }
    return 0;
}

/**
*
*/
bool Iofrontend::cargarOpcionesGenerales(){
    tmenu_gestor_objects *objMenu = ObjectsMenu[MENUOPCIONESGENERALES];
    ((UIComboBox *)objMenu->getObjByName("ComboFixesGeneral"))->selectValueInList(gestorRoms->getParameter("FIXDDRAW"));
    return true;
}

/**
*
*/
int Iofrontend::buscarInfoRoms(tEvento *evento){
    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAOPCIONESROM];
    string rom = objMenu->getObjByName("btnScrapGame")->getTag();

    vector<string> tmp = Constant::split(rom, ",");
    if (tmp.size() >= 1){
        string idprog = Constant::split(tmp.at(0),"=").at(1);
        if (!idprog.empty()){
            Traza::print("SCRAPPING: " + idprog, W_DEBUG);
            gestorRoms->setThEmuID(idprog);
            Thread<Gestorroms> *thread = new Thread<Gestorroms>(gestorRoms, &Gestorroms::thScrapSystem);
            thread->start();
            setSelMenu(PANTALLAGROUPLIST);
            cargarListaRoms(PANTALLAGROUPLIST, idprog, "listaGrupoRoms");
        }
    }
}

/**
*
*/
int Iofrontend::volverInfoRoms(tEvento *evento){

    tmenu_gestor_objects *objMenu = ObjectsMenu[PANTALLAOPCIONESROM];
    string rom = objMenu->getObjByName("btnScrapGame")->getTag();

    vector<string> tmp = Constant::split(rom, ",");
    if (tmp.size() >= 1){
        if (tmp.at(0).find("=") != string::npos){
            string idprog = Constant::split(tmp.at(0),"=").at(1);
            if (!idprog.empty()){
                setSelMenu(PANTALLAGROUPLIST);
                cargarListaRoms(PANTALLAGROUPLIST, idprog, "listaGrupoRoms");
            }
        }
    }
}

/**
*
*/
void Iofrontend::scrapAllRoms(){
    Traza::print("scrapAllRoms", W_DEBUG);

    if (Gestorroms::mutex != NULL){
        SDL_DestroyMutex(Gestorroms::mutex);
    }
    Gestorroms::mutex = SDL_CreateMutex();
    gestorRoms->setThDirInicial(dirInicial);

    Thread<Gestorroms> *thread = new Thread<Gestorroms>(gestorRoms, &Gestorroms::thRefreshAllArtWorkOptim);
    thread->start();
}

/**
*
*/
void Iofrontend::scrapEmuRoms(string codEmu){
    Traza::print("scrapEmuRoms", W_DEBUG);

    if (Gestorroms::mutex != NULL){
        SDL_DestroyMutex(Gestorroms::mutex);
    }
    Gestorroms::mutex = SDL_CreateMutex();
    gestorRoms->setThEmuID(codEmu);
    gestorRoms->setThDirInicial(dirInicial);

    Thread<Gestorroms> *thread = new Thread<Gestorroms>(gestorRoms, &Gestorroms::thRefreshArtWorkOptim);
    thread->start();
}

/**
*
*/
int Iofrontend::ImgEmuladorClicked(tEvento *evento){

    int menu = this->getSelMenu();
    tmenu_gestor_objects *objsMenu = ObjectsMenu[menu];
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());

    if (obj->getObjectType() == GUIPICTURE){
        Traza::print("evento picture clicked", W_DEBUG);
    }


    return false;
}
