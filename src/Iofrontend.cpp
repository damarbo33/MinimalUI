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
    initUIObjs();
    setSelMenu(MENUJUEGOS);
    gestorRoms = new Gestorroms(dirInicial);
    convColor = new Colorutil();
    imgGestor = new ImagenGestor();
    comprobarUnidad(dirInicial);
    cargaMenu(getSelMenu(), "", NULL);
    Traza::print("Fin Constructor de IoFrontend", W_PARANOIC);
}



/**
* Destructor
*/
Iofrontend::~Iofrontend(){
    Traza::print("Destructor de IoFrontend", W_DEBUG);
    delete convColor;
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
        + unidadRoms + ". %C2%BFDeseas actualizar?";
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
    
    tmenu_gestor_objects *objMenu = createMenu(MENUINICIAL);
    objMenu->add("ListaMenuInicial", GUILISTBOX, 0, 0, 0, 0, "ListaMenuInicial", true)->setVerContenedor(true);
    UIList * listaObj = (UIList *) objMenu->getObjByName("ListaMenuInicial");
    listaObj->addElemLista("Juegos", "", controller, MENUJUEGOS);
    listaObj->addElemLista("Videos", "", film, PANTALLAOPENMEDIA);
    listaObj->addElemLista("Opciones", "", bullet_wrench, MENUOPCIONES);
    listaObj->addElemLista("Reiniciar","reboot", arrow_refresh);
    listaObj->addElemLista("Apagar","shutdown", door_out);
    listaObj->addElemLista("Salir","salir", application_xp_terminal);
    objMenu->findNextFocus();
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Lanzador", false)->setEnabled(false);

    //Este menu no tiene nada. Es para hacer de enlace con el browser de directorios
    objMenu = createMenu(PANTALLAOPENMEDIA);
    objMenu = createMenu(LANZARROM);
    objMenu = createMenu("PACO");
    
    objMenu = createMenu(MENUJUEGOS);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Emuladores disponibles", false)->setEnabled(false);
    objMenu->add("ImgFondo", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondo", false)->setEnabled(false);
    objMenu->add("ListaMenuJuegos", GUILISTBOX, 0, 0, 0, 0, "ListaMenuJuegos", true)->setVerContenedor(false);
    ((UIPicture*) objMenu->getObjByName("ImgFondo"))->loadImgFromFile(dirInicial +  Constant::getFileSep()
        + "emuImgs" + Constant::getFileSep() + "54573da0d72cc.jpeg");
    objMenu->getObjByName("ImgFondo")->setAlpha(100);

    ((UIList *)objMenu->getObjByName("ListaMenuJuegos"))->setListScheme(SCHEMEICONS);
    ((UIList *)objMenu->getObjByName("ListaMenuJuegos"))->setSmoothDraw(true);

    UIPopupMenu * popupJuegos = addPopup(MENUJUEGOS, "popupEmusConfig", "ListaMenuJuegos");
    if (popupJuegos != NULL){
        popupJuegos->addElemLista("Config. Emu ", "ConfigEmu", controller);
        popupJuegos->addElemLista("Actualizar Información ", "ConfigEmu", arrow_refresh);
    }

    objMenu = createMenu(MENUOPCIONES);
    objMenu->add("ListaMenuOpciones", GUILISTBOX, 0, 0, 0, 0, "ListaMenuOpciones", true);
    UIList *listaObj2 = (UIList *) objMenu->getObjByName("ListaMenuOpciones");
    listaObj2->addElemLista("Alta emulador", "", add, PANTALLAOPCIONRUTAS);
    listaObj2->addElemLista("Modificar emulador","", application_form_edit, PANTALLAEDITAREMU);
    listaObj2->addElemLista("Escanear roms","scan", arrow_refresh, MENUOPCIONES);
    listaObj2->addElemLista(Constant::txtDisplay("Actualizar car%C3%a1tulas e info restante"),"scrapAll", folder_explore, MENUOPCIONES);
    listaObj2->addElemLista("Importar ajustes de Retroarch","importRetroarch", database_add, MENUOPCIONES);
    listaObj2->addElemLista("Asignar botones", "configJoy", controller, MENUOPCIONES);
    listaObj2->addElemLista("Opciones generales", "", page_white_gear, MENUOPCIONESGENERALES);
    listaObj2->addElemLista("Volver", "", bullet_go, MENUINICIAL);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones", false)->setEnabled(false);

    
    objMenu = createMenu(PANTALLAEDITAREMU);
    objMenu->add("listaEditarEmus", GUILISTBOX, 0, 0, 0, 0, "listaEditarEmus", false);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Modificar emulador", false)->setEnabled(false);

    /**Pantalla de opciones generales*/
    objMenu = createMenu(MENUOPCIONESGENERALES);
    int centroY = (-200 + ((zoomText > 1) ? -Constant::getMENUSPACE() : 0));
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones Generales", false)->setEnabled(false);
    objMenu->add("ComboFixesGeneral", GUICOMBOBOX, 0, centroY, 150, Constant::getCOMBOLISTHEIGHT(), "Corregir problemas en ddraw", true)->setVerContenedor(true);
    UIComboBox * comboGeneral = (UIComboBox *) objMenu->getObjByName("ComboFixesGeneral");
    comboGeneral->addElemLista("Sin Fix", "0");
    comboGeneral->addElemLista("Windows 8", "1");
    comboGeneral->addElemLista("Windows 8.1", "2");

    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("btnAceptarGeneralOpt",  GUIBUTTON, -(BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Aceptar" , true)->setIcon(tick);
    objMenu->add("btnCancelarGeneralOpt", GUIBUTTON, (BUTTONW + 5) , centroY, BUTTONW -15,BUTTONH, "Cancelar", true)->setIcon(cross);

    /**Pantalla de opciones de cada emulador*/
    objMenu = createMenu(PANTALLAOPCIONRUTAS);
    centroY = (-200 + ((zoomText > 1) ? -Constant::getMENUSPACE() : 0));
    objMenu->add("EmuName", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Nombre emulador:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("EmuRuta", GUIINPUTWIDE, - 15 , centroY, INPUTW - 30, Constant::getINPUTH(), "Ruta emulador:", true)->setColor(cNegro);
    objMenu->add("btnEmuRuta", GUIBUTTON, INPUTW/2 * zoomText - 10, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuRuta");
    objMenu->add("btnLaunchEmu", GUIBUTTON, INPUTW/2 * zoomText + 20, centroY,MINIBUTTONH,MINIBUTTONH, "", true)->setIcon(bullet_wrench);;
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));

    objMenu->add("EmuParms", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Parametros emulador:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("EmuRutaRom", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Ruta roms:", true)->setColor(cNegro);
    objMenu->add("btnRomRuta", GUIBUTTON, INPUTW/2 + 20 * zoomText, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuRutaRom");
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("EmuRomExtension", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Extension:", true)->setColor(cNegro);
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("EmuImg", GUIINPUTWIDE, 0, centroY, INPUTW, Constant::getINPUTH(), "Imagen emulador:", true)->setColor(cNegro);
    objMenu->add("btnEmuImg", GUIBUTTON, INPUTW/2 + 20 * zoomText, centroY,MINIBUTTONH,MINIBUTTONH, "...", true)->setTag("EmuImg");
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("ComboPlatform", GUICOMBOBOX, 130, centroY, 280, Constant::getCOMBOLISTHEIGHT(), "Sistema para obtener info", true)->setVerContenedor(true);
    crearComboSistemas();
    centroY += (55 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));
    objMenu->add("CheckComboFixes", GUICHECK, -195, centroY - 60, 150, CHECKH, "Corregir problemas en ddraw", true);
    objMenu->add("CheckDescomprimir", GUICHECK, -195, centroY, 150, CHECKH, "Descomprimir roms", true);
    objMenu->add("CheckFindTitleRom", GUICHECK, -20, centroY, 150, CHECKH, "Buscar descripción de rom", true);
    centroY += (40 + ((zoomText > 1) ? Constant::getMENUSPACE() : 0));

    UIPopupMenu * popup1 = addPopup(PANTALLAOPCIONRUTAS, "popupParmsRom", "EmuParms");
    if (popup1 != NULL){
        popup1->addElemLista("Ruta de la rom", "%ROMPATH%", controller);
        popup1->addElemLista("Nombre de la Rom", "%ROMNAME%", controller);
        popup1->addElemLista("Nombre de la Rom con extensión", "%ROMNAMEXT%", controller);
        popup1->addElemLista("Ruta y nombre de Rom con extensión", "%ROMFULLPATH%", controller);
    }

    objMenu->add("btnAceptarEmu", GUIBUTTON, -(BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Aceptar", true)->setIcon(tick);
    objMenu->add("btnEliminarEmu", GUIBUTTON, 0, centroY, BUTTONW -15,BUTTONH, "Eliminar", true)->setIcon(deleteIco)->setVisible(false);
    objMenu->add("btnCancelarEmu", GUIBUTTON, (BUTTONW + 5), centroY, BUTTONW -15,BUTTONH, "Cancelar", true)->setIcon(cross);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Alta/Modificacion emulador", false)->setEnabled(false);
    
    objMenu = createMenu(PANTALLAROMS);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Roms", false)->setEnabled(false);
    objMenu->add("ImgEmulador", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgEmulador", true)->setEnabled(false);
    objMenu->add("ListaMenuRoms", GUILISTBOX, 0, 0, 0, 0, "ListaMenuRoms", true)->setVerContenedor(false);
    objMenu->getObjByName("ImgEmulador")->setAlpha(150);

    objMenu = createMenu(PANTALLAREPRODUCTOR);
    objMenu->add("panelMedia", GUIPANEL, 0,0,0,0, "", true)->setEnabled(false);
    objMenu->add("btnBackward", GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_rewind)->setVerContenedor(false);
    objMenu->add("btnPlay",     GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_play)->setVerContenedor(false);
    objMenu->add("btnStop",     GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_stop)->setVerContenedor(false);
    objMenu->add("btnForward",  GUIBUTTON, 0,0,0,0, "", true)->setIcon(control_fastforward)->setVerContenedor(false);
    objMenu->add("mediaTimerTotal",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);;
    objMenu->add("mediaTimer",  GUILABEL,  0,0,0,0, "0:00:00", false)->setEnabled(false);;
    objMenu->add("progressBarMedia", GUIPROGRESSBAR, 20, 20, 200, 20, "", true)->setShadow(false);
    objMenu->getObjByName("panelMedia")->setAlpha(150);

    
    objMenu = createMenu(PANTALLAOPCIONESROM);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Opciones Roms", false)->setEnabled(false);
    objMenu->add("ImgFondoRoms", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgFondoRoms", true)->setEnabled(false);
    objMenu->add("panelMedia", GUIPANEL, 0,0,500,350, "", true)->setEnabled(false);
    objMenu->add("btnScrapGame", GUIBUTTON, -(BUTTONW/2 + 5), -BUTTONH *2, BUTTONW,BUTTONH, "Buscar info", true)->setTag("");
    objMenu->add("btnCancelarOptRoms", GUIBUTTON, 0, 100, BUTTONW,BUTTONH, "Volver", true)->setIcon(cross);
    UIPanel * panel = (UIPanel *) objMenu->getObjByName("panelMedia");
    panel->setColor(cBlanco);

    
    objMenu = createMenu(PANTALLAGROUPLIST);
    objMenu->add(TITLESCREEN, GUIARTSURFACE, 0, 0, INPUTW, Constant::getINPUTH(), "Roms", false)->setEnabled(false);
    objMenu->add("ImgEmulador", GUIPICTURE, 0, Constant::getINPUTH(), 0, 0, "ImgEmulador", true)->setEnabled(false);
    objMenu->getObjByName("ImgEmulador")->setAlpha(200);
    objMenu->getObjByName("ImgEmulador")->getImgGestor()->setEnabledMoveImg(false);
    objMenu->getObjByName("ImgEmulador")->setVerContenedor(false);
    
    objMenu->add("listaGrupoRoms", GUILISTGROUPBOX, 0, 0, 0, 0, "", false)->setVerContenedor(false);
    objMenu->add("textosInfoBox", GUITEXTELEMENTSAREA, 0, 0, 0, 0, "", true)->setVerContenedor(false)->setEnabled(false);
    objMenu->add("textosDescBox", GUITEXTELEMENTSAREA, 0, 0, 0, 0, "", true)->setVerContenedor(false)->setEnabled(true);
    objMenu->add("ImgBoxArt", GUIPICTURE, 0, 0, Constant::getIMGBOXARTWIDTH(), Constant::getIMGBOXARTHEIGHT(), "", true)->setEnabled(true);
    objMenu->add("ImgBoxArtFull", GUIPICTURE, 0, 0, this->getWidth(), this->getHeight(), "ImgEmuladorFull", true)->setVisible(false);
    

    UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
    vector <ListGroupCol *> miCabecera;
    miCabecera.push_back(new ListGroupCol("Nombre Rom", ""));
    miCabecera.push_back(new ListGroupCol("Jugadores", ""));
    miCabecera.push_back(new ListGroupCol("Categoría", ""));
    miCabecera.push_back(new ListGroupCol("Calificación", ""));
    listaGrupo->setHeaderLista(miCabecera);
    listaGrupo->adjustToHeader(false);
    listaGrupo->addHeaderWith(500);
    listaGrupo->addHeaderWith(90);
    listaGrupo->addHeaderWith(120);
    listaGrupo->addHeaderWith(100);

    UIPicture* pict = ((UIPicture*) objMenu->getObjByName("ImgBoxArt"));
    //Para mejorar el rendimiento, las imagenes que se descargan de las caratulas ya estan redimensionadas
    pict->getImgGestor()->setResize(true);
    pict->getImgGestor()->setEnabledMoveImg(false);
    pict->getImgGestor()->setFillBackgroundColour(false);
    pict->getImgGestor()->setBestfit(true);

    int margenX = Constant::getIMGBOXARTWIDTH() + IMGBOXARTMARGIN * 2;
    //Anyadimos los textos en el area que digamos
    UITextElementsArea *infoTextRom = (UITextElementsArea *)objMenu->getObjByName("textosInfoBox");
    t_posicion pos(0, Constant::getMENUSPACE() + IMGBOXARTMARGIN,0,0);
    
    const char *elems[] = {"txtReleased", "RELEASED:",
    "txtPlayers","PLAYERS:",
    "txtPublisher","PUBLISHER:",
    "txtDeveloper","DEVELOPER:",
    "txtGenre","GENRE:",
    "txtRating","RATING:",
    "txtLastPlayed","LAST PLAYED:",
    "txtTimesPlayed","TIMES PLAYED:"};
    int len = (sizeof(elems) / sizeof(elems[0]));
    
    for (int i=0; i<len/2; i++){
        TextElement *textElem = new TextElement();
        textElem->setName(elems[i*2]);
        textElem->setLabel(elems[i*2+1]);
        textElem->setPos(pos);
        textElem->setUseMaxLabelMargin(true);
        pos.y += 20;
    }

    infoTextRom = (UITextElementsArea *)objMenu->getObjByName("textosDescBox");

    t_element_style stilo;

    stilo.pos = t_posicion(0,10,0,0);
    stilo.bold = true;
    stilo.fontSize = 16;
    TextElement *textElem = new TextElement();
    textElem->setName("txtFilePath");
    textElem->setStyle(stilo);
    textElem->setUseMaxLabelMargin(false);
    infoTextRom->addField(textElem);
    
    stilo.pos.y += 60;
    stilo.bold = false;
    TextElement *textElem2 = new TextElement();
    textElem2->setName("txtDescripcion");
    textElem2->setStyle(stilo);
    textElem2->setUseMaxLabelMargin(false);
    infoTextRom->addField(textElem2);

    UIPopupMenu * popupJuegosG = addPopup(PANTALLAGROUPLIST, "popupEmusConfig", "listaGrupoRoms");
    if (popupJuegosG != NULL){
        popupJuegosG->addElemLista("Config. Emu ", "ConfigEmu", controller);
        popupJuegosG->addElemLista("Actualizar Información ", "ConfigEmu", arrow_refresh);
    }

    //Establecemos los elementos que se redimensionan
    setDinamicSizeObjects();

    //Botones para la pantalla de video
    addEvent("btnPlay",  static_cast<typept2Func>(&Iofrontend::accionesMediaPause));
    addEvent("btnStop",  static_cast<typept2Func>(&Iofrontend::accionesMediaStop));
    addEvent("btnForward",  static_cast<typept2Func>(&Iofrontend::accionesMediaAvanzar));
    addEvent("btnBackward",  static_cast<typept2Func>(&Iofrontend::accionesMediaRetroceder));
    addEvent("progressBarMedia", static_cast<typept2Func>(&Iofrontend::mediaClicked));
    
    //Asignamos acciones a los elementos. NO INSERTAR BOTONES CON EL MISMO NOMBRE AUNQUE ESTEN EN MENUS DISTINTOS
    addEvent("ListaMenuInicial",  static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("ListaMenuOpciones",  static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("ListaMenuJuegos",  static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("ListaMenuRoms",  static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("listaEditarEmus", static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("popupParmsRom", static_cast<typept2Func>(&Iofrontend::accionCopiarTextoPopup));
    addEvent("popupEmusConfig", static_cast<typept2Func>(&Iofrontend::accionConfigEmusPopup));

    //Acciones del menu de las opciones
    addEvent("btnEmuRuta",  static_cast<typept2Func>(&Iofrontend::loadDirFromExplorer));
    addEvent("btnRomRuta",  static_cast<typept2Func>(&Iofrontend::loadDirFromExplorer));
    addEvent("btnEmuImg",  static_cast<typept2Func>(&Iofrontend::loadDirFromExplorer));
    addEvent("btnLaunchEmu", static_cast<typept2Func>(&Iofrontend::launchEmuForConfig));
    addEvent("btnAceptarEmu", static_cast<typept2Func>(&Iofrontend::accionesBtnAceptarOpciones));
    addEvent("btnEliminarEmu", static_cast<typept2Func>(&Iofrontend::accionesBtnEliminarEmu));
    addEvent("btnAceptarGeneralOpt", static_cast<typept2Func>(&Iofrontend::accionesBtnAceptarOpcionesGenerales));

    //Acciones por defecto de la lista para directorios
    addEvent("btnCancelarDir", static_cast<typept2Func>(&Iofrontend::accionesGotoPantalla), PANTALLAOPCIONRUTAS);
    addEvent("btnCancelarEmu", static_cast<typept2Func>(&Iofrontend::accionesBtnVolverConfigEmu), PANTALLAEDITAREMU);
    addEvent("btnCancelarGeneralOpt", static_cast<typept2Func>(&Iofrontend::accionesGotoPantalla), MENUOPCIONES);
    addEvent("btnScrapGame", static_cast<typept2Func>(&Iofrontend::buscarInfoRoms));
    addEvent("btnCancelarOptRoms", static_cast<typept2Func>(&Iofrontend::volverInfoRoms));
    addEvent("listaGrupoRoms", static_cast<typept2Func>(&Iofrontend::accionesMenu));
    addEvent("ImgBoxArt", static_cast<typept2Func>(&Iofrontend::ImgEmuladorClicked));
    addEvent("ImgBoxArtFull", static_cast<typept2Func>(&Iofrontend::ImgBoxArtFullClicked));
}

/**
 * 
 * @param menucarga
 * @param valorSelec
 * @param evento
 */
void Iofrontend::cargaMenu(string menucarga, string valorSelec, tEvento *evento){
    comprobarUnicode(menucarga);
    
    tmenu_gestor_objects *objsMenu = getMenu(menucarga);
    if (objsMenu == NULL){
        return;
    }

    this->setSelMenu(menucarga);
    //Damos el foco al primer elemento que haya en el menu
    objsMenu->setFirstFocus();
    
   if (menucarga.compare(MENUJUEGOS) == 0){
            cargarListaEmuladores(menucarga, PANTALLAGROUPLIST, "ListaMenuJuegos");
   } else if (menucarga.compare(MENUOPCIONES) == 0){
            if (valorSelec.compare("scan") == 0){
                actualizarRoms();
            } else if (valorSelec.compare("importRetroarch") == 0){
                importRetroarchConfig();
            } else if (valorSelec.compare("configJoy") == 0){
                casoJOYBUTTONS(evento);
            } else if (valorSelec.compare("scrapAll") == 0){
                scrapAllRoms();
            }
    } else if (menucarga.compare(PANTALLAROMS) == 0){
            cargarListaRoms(menucarga, valorSelec, "ListaMenuRoms");
    } else if (menucarga.compare(PANTALLAGROUPLIST) == 0){
            cargarListaRoms(menucarga, valorSelec, "listaGrupoRoms");
            objsMenu->setFocus("listaGrupoRoms");
    } else if (menucarga.compare(PANTALLAEDITAREMU) == 0){
            cargarListaEmuladores(menucarga, PANTALLAOPCIONRUTAS, "listaEditarEmus");
    } else if (menucarga.compare(PANTALLAOPCIONRUTAS) == 0){
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
    } else if (menucarga.compare(LANZARROM) == 0){
            UIListCommon *objTemp = (UIListCommon *)getMenu(PANTALLAGROUPLIST)->getObjByName("listaGrupoRoms");
            valorSelec = objTemp->getValue(objTemp->getPosActualLista());
            Traza::print("LANZANDO ROM con id: " + valorSelec, W_DEBUG);
            lanzarPrograma(valorSelec);
            this->setSelMenu(PANTALLAGROUPLIST);
    } else if (menucarga.compare(PANTALLAOPENMEDIA) == 0){
            playMedia(evento);
    } else if (menucarga.compare(MENUOPCIONESGENERALES) == 0){
            cargarOpcionesGenerales();
    }
}

/**
 * 
 * @return 
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
 * @param evento
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
void Iofrontend::setDinamicSizeObjects(){
    try{
        BaseFrontend::setDinamicSizeObjects();
        
        tmenu_gestor_objects *obj = getMenu(PANTALLAGROUPLIST);
        obj->getObjByName("listaGrupoRoms")->setTam(this->getWidth() / 2 , Constant::getINPUTH(), this->getWidth() / 2, this->getHeight()-Constant::getINPUTH());


        int widthForImgBox = (this->getWidth() / 4 > 250) ? 250 : this->getWidth() / 4;
        Constant::setIMGBOXARTWIDTH(widthForImgBox);
        Constant::setIMGBOXARTHEIGHT(widthForImgBox);

        obj->getObjByName("ImgBoxArt")->setTam(IMGBOXARTMARGIN,
                                            Constant::getINPUTH() + Constant::getMENUSPACE() + INPUTCONTENT,
                                            Constant::getIMGBOXARTWIDTH(), Constant::getIMGBOXARTHEIGHT());

        obj->getObjByName("ImgBoxArtFull")->setTam(0,0,this->getWidth(),this->getHeight());

        UITextElementsArea *infoTextRom = (UITextElementsArea *)obj->getObjByName("textosInfoBox");
        int margenX = Constant::getIMGBOXARTWIDTH() + IMGBOXARTMARGIN * 2;
        infoTextRom->setTam(margenX, Constant::getINPUTH(),
                            this->getWidth() / 2 - margenX,
                            this->getHeight() - Constant::getINPUTH());

        //int margenX = Constant::getIMGBOXARTWIDTH() + IMGBOXARTMARGIN + 3;
        t_posicion pos(0, Constant::getMENUSPACE() + IMGBOXARTMARGIN,0,0);
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

        infoTextRom = (UITextElementsArea *)obj->getObjByName("textosDescBox");
        infoTextRom->setTam(0, 30 + Constant::getIMGBOXARTHEIGHT() + IMGBOXARTMARGIN * 2,
                            this->getWidth() / 2, this->getHeight()- Constant::getINPUTH());

        t_posicion posDesc(0,10,0,0);
        infoTextRom->setPosition("txtFilePath",posDesc);
        posDesc.y+=60;
        infoTextRom->setPosition("txtDescripcion",posDesc);

        //Redimension para la pantalla de videos multimedia
        int desp = (this->getWidth() / 2) - FAMFAMICONW*2 - BUTTONW/2;
        int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
        obj = getMenu(PANTALLAREPRODUCTOR);
        obj->getObjByName("panelMedia")->setTam(0, calculaPosPanelMedia(), this->getWidth(), this->getHeight() - calculaPosPanelMedia());
        obj->getObjByName("btnBackward")->setTam(desp += (BUTTONW/2), bottom, FAMFAMICONW, FAMFAMICONH);
        obj->getObjByName("btnPlay")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        obj->getObjByName("btnStop")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        obj->getObjByName("btnForward")->setTam(desp += FAMFAMICONW, bottom, FAMFAMICONW, FAMFAMICONH);
        obj->getObjByName("progressBarMedia")->setTam( TIMEW + SEPTIMER, bottom - PROGRESSSEPBOTTOM, this->getWidth() - TIMEW*2 - SEPTIMER*2, PROGRESSHEIGHT);
        obj->getObjByName("mediaTimerTotal")->setTam(SEPTIMER, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
        obj->getObjByName("mediaTimer")->setTam(this->getWidth() - TIMEW, bottom - PROGRESSSEPBOTTOM, TIMEW, FAMFAMICONH);
    } catch (Excepcion &e){
        Traza::print("setDinamicSizeObjects: " + string(e.getMessage()), W_ERROR);
    }
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::launchEmuForConfig(tEvento *evento){
    try{
        Traza::print("Lanzando EMU", W_DEBUG);
        //Obtenemos los objetos del menu actual
        tmenu_gestor_objects *objMenu = getMenu(this->getSelMenu());
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
 * @param evento
 * @return 
 */
int Iofrontend::accionesBtnEliminarEmu(tEvento *evento){

    string pregunta = "%C2%BFEst%C3%A1 seguro de que desea eliminar el programa?";
    bool confirmed = casoPANTALLACONFIRMAR("Confirmacion",  pregunta);
    if (confirmed){
        tmenu_gestor_objects *objMenu = getMenu(this->getSelMenu());
        Object *obj = objMenu->getObjByName("btnEliminarEmu");
        gestorRoms->deleteEmulador(Constant::strToTipo<int>(obj->getTag()));
        obj->setTag("");

        string tag = objMenu->getObjByName("btnCancelarEmu")->getTag();
        //Si venimos de la pantalla de listado de emuladores o de la pantalla de listado de roms
        if (tag.compare("FROMEMULIST") == 0 || tag.compare("FROMEMUGROUPLIST") == 0){
            this->setSelMenu(MENUJUEGOS);
            getMenu(getSelMenu())->setFirstFocus();
            //ObjectsMenu[getSelMenu()]->getObjByName("ListaMenuJuegos")->setImgDrawed(false);
            cargarListaEmuladores(getSelMenu(), PANTALLAGROUPLIST, "ListaMenuJuegos");
        } else {
            //Si venimos de la pantalla de configuracion
            this->setSelMenu(PANTALLAEDITAREMU);
            cargarListaEmuladores(PANTALLAEDITAREMU, PANTALLAOPCIONRUTAS, "listaEditarEmus");
        }
        //Reseteamos el origen
        objMenu->getObjByName("btnCancelarEmu")->setTag("");
    }
    return 0;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::accionesBtnVolverConfigEmu(tEvento *evento){
    tmenu_gestor_objects *objsMenu = getMenu(this->getSelMenu());
    string tag = objsMenu->getObjByName("btnCancelarEmu")->getTag();

    if (tag.compare("FROMEMULIST") == 0){
        objsMenu->getObjByName("btnCancelarEmu")->setTag("");
        this->setSelMenu(MENUJUEGOS);
        getMenu(MENUJUEGOS)->setFirstFocus();
    } else if (tag.compare("FROMEMUGROUPLIST") == 0){
        objsMenu->getObjByName("btnCancelarEmu")->setTag("");
        this->setSelMenu(PANTALLAGROUPLIST);
        getMenu(PANTALLAGROUPLIST)->setFirstFocus();
    } else {
        this->setSelMenu(PANTALLAEDITAREMU);
        objsMenu = getMenu(this->getSelMenu());
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

    tmenu_gestor_objects *objMenu = getMenu(PANTALLAOPCIONRUTAS);
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


            showMessage("Actualizando datos...", 1);

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
    tmenu_gestor_objects *objMenu = getMenu(PANTALLAOPCIONRUTAS);
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

    tmenu_gestor_objects *objMenu = getMenu(PANTALLAOPCIONRUTAS);
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
bool Iofrontend::cargarListaEmuladores(string menu, string destino, string lista){
    try{
        tmenu_gestor_objects *objMenu = getMenu(menu);
        UIList *obj = (UIList *)objMenu->getObjByName(lista);
        Traza::print("Cargando lista de emuladores de: " + dirInicial, W_DEBUG);
        gestorRoms->fillMenuByQuery(obj, "selectListaEmuladores",NULL, destino);
        obj->addElemLista(Constant::txtDisplay("Configuraci%C3%B3n"), "", bullet_go, lista.compare("listaEditarEmus") == 0 ? MENUOPCIONES : MENUINICIAL);
    } catch (Excepcion &e) {
         Traza::print("Excepcion cargarListaEmuladores" + string(e.getMessage()), W_ERROR);
    }
    return false;
}

/**
*
*/
bool Iofrontend::cargarListaRoms(string menu, string idprog, string lista){
    try{
        tmenu_gestor_objects *objMenu = getMenu(menu);
        //Obtenemos el nombre del emulador que seleccionamos en la pantalla anterior
        UIList *objEmu = (UIList *)getMenu(MENUJUEGOS)->getObjByName("ListaMenuJuegos");
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
            miFila.push_back(new ListGroupCol("", "", bullet_go, MENUJUEGOS));
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
        showMessage("Actualizando roms. Espere un momento...", 1, false, {0,50,0,0});

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
    getMenu(PANTALLAGROUPLIST)->getObjByName("ImgEmulador")->setImgDrawed(false);
    drawUIPicture(getMenu(PANTALLAGROUPLIST)->getObjByName("ImgEmulador"));
    getMenu(PANTALLAGROUPLIST)->setFirstFocus();

    Traza::print("SDL Iniciado con exito", W_DEBUG);
    return false;
}

/**
 * 
 * @param emulInfo
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
 * @param evento
 * @return 
 */
int Iofrontend::accionesMediaAvanzar(tEvento *evento){
    player.salto10PercentForward();
    return false;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::accionesMediaRetroceder(tEvento *evento){
    player.salto10PercentBackwards();
    return false;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::accionesMediaPause(tEvento *evento){
    player.pause();
    return false;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::accionesMediaStop(tEvento *evento){
    simularEscape(evento);
    return false;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::mediaClicked(tEvento *evento){
    UIProgressBar * objProg = (UIProgressBar *)getMenu(PANTALLAREPRODUCTOR)->getObjByName("progressBarMedia");

    Traza::print("Pos pulsada media: ", objProg->getProgressPos(), W_DEBUG);
    float pos = 0;
    if (objProg->getProgressMax() > 0)
        pos = objProg->getProgressPos() / (float)objProg->getProgressMax();

    player.setPositionPercent(pos);

    return false;
}

/**
 * 
 * @param var
 */
void Iofrontend::setPanelMediaVisible(bool var){
    try{
        tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
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
 * @param file
 * @return 
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
    tmenu_gestor_objects *obj = getMenu(PANTALLAREPRODUCTOR);
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
            //delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
            //if(delay > 0) SDL_Delay(delay);
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
 * @return 
 */
int Iofrontend::calculaPosPanelMedia(){
    int bottom = this->getHeight() - FAMFAMICONH - ICOBOTTOMSPACE;
    return (bottom - PROGRESSSEPBOTTOM - SEPPANELMEDIA);
}

/**
 * Desde este menu podremos mostrar por pantalla la informacion de las roms de cada emulador
 * 
 * @param objMenu
 * @param evento
 * @return 
 */
bool Iofrontend::procesarMenuActual(tmenu_gestor_objects *objMenu, tEvento *evento){

    static unsigned long lastClick = 0;
    static unsigned long lastTitleUpdate = 0;
    static int pos = -1;


    try{
        if (SDL_GetTicks() - lastTitleUpdate > 1000){
            UIArt *titulo = (UIArt *) getMenu(getSelMenu())->getObjByName(TITLESCREEN);

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
            UIListGroup * listaGrupo = (UIListGroup *) getMenu(PANTALLAGROUPLIST)->getObjByName("listaGrupoRoms");
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
//            } else if (evento->key == SDLK_F3 && evento->keyjoydown){
//                tmenu_gestor_objects *objMenu = ObjectsMenu[getSelMenu()];
//                UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
//                string rom = listaGrupo->getValue(listaGrupo->getSize() - 1);
//
//                showMenuEmergente(PANTALLAOPCIONESROM, "ImgFondoRoms");
//                objMenu = ObjectsMenu[PANTALLAOPCIONESROM];
//                objMenu->getObjByName("btnScrapGame")->setTag(rom);
            } else if (evento->key == SDLK_ESCAPE && evento->keyjoydown || evento->joy == JoyMapper::getJoyMapper(JOY_BUTTON_SELECT)){
                tmenu_gestor_objects *objMenu = getMenu(getSelMenu());
                UIPicture * objFondo = (UIPicture *)objMenu->getObjByName("ImgBoxArtFull");
                if (objFondo->isVisible()){
                    objFondo->setVisible(false);
                    objMenu->setFocus("listaGrupoRoms");
                }

            }
        }
    }
}

/**
 * 
 * @return 
 */
DWORD Iofrontend::setInfoRomValues(){
    tmenu_gestor_objects *objMenu = getMenu(PANTALLAGROUPLIST);
    UIListGroup * listaGrupo = (UIListGroup *) objMenu->getObjByName("listaGrupoRoms");
    UITextElementsArea *textElemsInfo = (UITextElementsArea *)objMenu->getObjByName("textosInfoBox");
    UITextElementsArea *textElemsDesc = (UITextElementsArea *)objMenu->getObjByName("textosDescBox");

    textElemsInfo->setOffsetDesplazamiento(0);
    textElemsInfo->setImgDrawed(false);

    textElemsDesc->setOffsetDesplazamiento(0);
    textElemsDesc->setImgDrawed(false);

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
            textElemsInfo->setFieldText("txtPlayers", row.at(0));
            textElemsInfo->setFieldText("txtGenre", row.at(2));
            textElemsInfo->setFieldText("txtReleased", row.at(3));
            textElemsInfo->setFieldText("txtLastPlayed", "0");
            textElemsInfo->setFieldText("txtTimesPlayed", "0");
            textElemsDesc->setFieldText("txtDescripcion", row.at(5));
            textElemsInfo->setFieldText("txtPublisher", row.at(6));
            textElemsInfo->setFieldText("txtDeveloper", row.at(7));
            textElemsInfo->setFieldText("txtRating", row.at(8));
            textElemsDesc->setFieldText("txtFilePath", row.at(13));

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
            Dirutil dir;
            if (dir.existe(directory)){
                pict->loadImgFromFile(directory);
                pict->setCentrado(true);
                pict->setBestfit(false);
            }


        } else {
            textElemsInfo->setFieldText("txtPlayers", "");
            textElemsInfo->setFieldText("txtGenre", "");
            textElemsInfo->setFieldText("txtReleased", "");
            textElemsInfo->setFieldText("txtLastPlayed", "0");
            textElemsInfo->setFieldText("txtTimesPlayed", "0");
            textElemsDesc->setFieldText("txtDescripcion", "");
            textElemsInfo->setFieldText("txtPublisher", "");
            textElemsInfo->setFieldText("txtDeveloper", "");
            textElemsInfo->setFieldText("txtRating", "");
            textElemsDesc->setFieldText("txtFilePath", "");
            ((UIPicture*) objMenu->getObjByName("ImgBoxArt"))->clearImg();
        }
    }
    return 1;
}

/**
 * Configura el emulador
 * @param evento
 * @return 
 */
int Iofrontend::accionConfigEmusPopup(tEvento *evento){
    //Se obtiene el objeto menupopup que en principio esta seleccionado
    tmenu_gestor_objects *objsMenu = getMenu(this->getSelMenu());
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
                tmenu_gestor_objects *objsMenu = getMenu(PANTALLAOPCIONRUTAS);
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
    tmenu_gestor_objects *objMenu  = getMenu(PANTALLAOPCIONRUTAS);
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
 * @param evento
 * @return 
 */
int Iofrontend::accionesBtnAceptarOpcionesGenerales(tEvento *evento){
    tmenu_gestor_objects *objMenu = getMenu(MENUOPCIONESGENERALES);
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
 * @return 
 */
bool Iofrontend::cargarOpcionesGenerales(){
    tmenu_gestor_objects *objMenu = getMenu(MENUOPCIONESGENERALES);
    ((UIComboBox *)objMenu->getObjByName("ComboFixesGeneral"))->selectValueInList(gestorRoms->getParameter("FIXDDRAW"));
    return true;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::buscarInfoRoms(tEvento *evento){
    tmenu_gestor_objects *objMenu = getMenu(PANTALLAOPCIONESROM);
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
 * @param evento
 * @return 
 */
int Iofrontend::volverInfoRoms(tEvento *evento){

    tmenu_gestor_objects *objMenu = getMenu(PANTALLAOPCIONESROM);
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
 * @param codEmu
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
 * @param evento
 * @return 
 */
int Iofrontend::ImgEmuladorClicked(tEvento *evento){
    tmenu_gestor_objects *objsMenu = getMenu(this->getSelMenu());
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());

    if (obj->getObjectType() == GUIPICTURE){
        Traza::print("evento picture clicked", W_DEBUG);

        UIPicture * objImg = (UIPicture *)obj;
        UIPicture * objFondo = (UIPicture *)objsMenu->getObjByName("ImgBoxArtFull");

        string imgruta = objImg->getImgGestor()->getRuta();

        if (!imgruta.empty()){
            Dirutil dir;
            string origFileName = dir.getFolder(imgruta) + Constant::getFileSep() +
                dir.getFileNameNoExt(imgruta) + "_full" + dir.getExtension(imgruta);

            if (dir.existe(origFileName)){
                objFondo->setVisible(true);
                objFondo->clearImg();
                objFondo->loadImgFromFile(origFileName);
                objFondo->getImgGestor()->setBestfit(false);
                objFondo->getImgGestor()->setColorBackground(cNegro);
                objFondo->getImgGestor()->setFillBackgroundColour(true);
                objFondo->getImgGestor()->setCentrado(true);
                objFondo->setImgDrawed(false);
            }
        }
    }
    return false;
}

/**
 * 
 * @param evento
 * @return 
 */
int Iofrontend::ImgBoxArtFullClicked(tEvento *evento){
    tmenu_gestor_objects *objsMenu = getMenu(this->getSelMenu());
    Object *obj = objsMenu->getObjByPos(objsMenu->getFocus());
    if (obj->getObjectType() == GUIPICTURE){
        obj->setVisible(false);
    }
    return false;
}
