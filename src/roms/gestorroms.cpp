#include "gestorroms.h"

Gestorroms::Gestorroms(string ruta){
    //ctor
    db = NULL;
    setRutaInicial(ruta);
    loadDBFromFile(ruta);

}

Gestorroms::~Gestorroms(){
    //dtor
    closeDB();
}

DWORD Gestorroms::importRetroarchConfig(){

    try{
        listaSimple<string> *fileRetroarch = new listaSimple<string>();
        unsigned int fileLen = fileRetroarch->loadStringsFromFile(getRutaInicial() + FILE_SEPARATOR + RUTA_RETROARCH_CFG);

        if (fileLen > 0){
            //Borramos lo que hubiese anteriormente
            db->prepareStatement("deleteroms");
            db->setClauseWhere(false);
            db->execute();
            db->prepareStatement("deleterutas");
            db->setClauseWhere(false);
            db->execute();
            db->prepareStatement("deleterominfo");
            db->setClauseWhere(false);
            db->execute();
            db->prepareStatement("deleteemulador");
            db->setClauseWhere(false);
            db->execute();

            string linea;
            //Gestorroms *gestor = new Gestorroms(dirInicial);
            //showMessage("Importando configuración. Espere un momento...", 1);

            Emuinfo emuProps;
            for (unsigned int i=0; i < fileRetroarch->getSize(); i++){
                linea = fileRetroarch->get(i);
                if (linea.find("PATH") == 0){
                    emuProps.emuProperties.rutaRoms = Constant::split(linea, '=').at(1);
                } else if (linea.find("COMMAND") == 0){
                    emuProps.emuProperties.rutaEmu = Constant::split(linea, '=').at(1);
                } else if (linea.find("NAME") == 0){
                    emuProps.emuProperties.nombreEmu = Constant::split(linea, '=').at(1);
                } else if (linea.find("EXTENSION") == 0){
                    emuProps.emuProperties.emuRomExt = Constant::split(linea, '=').at(1);
                }

                emuProps.emuProperties.parmsEmu = "";
                emuProps.emuProperties.descomp = true;

                if (!emuProps.emuProperties.rutaRoms.empty() && !emuProps.emuProperties.rutaEmu.empty()
                    && !emuProps.emuProperties.nombreEmu.empty()){
                    //Traza::print("command: " + command + " path: " + path + " name: " + name, W_DEBUG);
                    insertEmulador(&emuProps.emuProperties);
                    emuProps.clear();
                }
            }
        }

        delete fileRetroarch;
        return 0;

    } catch (Excepcion &e) {
         Traza::print("Excepcion importRetroarchConfig: " + string(e.getMessage()), W_ERROR);
         return 1;
    }

}

/**
*
*/
void Gestorroms::loadDBFromFile(string ruta){

    string rutaCompleta = ruta + FILE_SEPARATOR + FILEBBDD;
    Dirutil dir;
    bool existe = dir.existe(rutaCompleta);
    db = new Database(rutaCompleta.c_str());
    Traza::print("rutaCompleta: " + rutaCompleta, W_DEBUG);

    if (!existe){
        try{
            db->query("CREATE TABLE \"EMULADOR\" (\"IDPROG\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , \"NOMBREEMU\" TEXT, \"RUTAEMU\" TEXT, \"PARMSEMU\" TEXT, \"RUTAROMS\" TEXT, \"EXTENSIONES\" TEXT, \"DESCOMPRIMIR\" TEXT DEFAULT N,  \"IMGRUTAFONDO\" TEXT, \"SEPARARUTAPARM\" TEXT DEFAULT S, \"SHOWTITLE\" TEXT DEFAULT N)");
            db->query("CREATE TABLE \"ROMINFO\" (\"IDROM\" INTEGER NOT NULL , \"IDPROG\" INTEGER NOT NULL , \"NPLAYERS\" TEXT, \"TITLE\" TEXT, \"CATEG\" TEXT, PRIMARY KEY (\"IDROM\", \"IDPROG\"));");
            db->query("CREATE TABLE \"ROMS\" (\"IDPROG\" INTEGER NOT NULL , \"IDRUTA\" INTEGER NOT NULL , \"IDROM\" INTEGER NOT NULL , \"NOMBRE\" TEXT, PRIMARY KEY (\"IDPROG\", \"IDRUTA\", \"IDROM\"));");
            db->query("CREATE TABLE \"RUTAS\" (\"IDPROG\" INTEGER NOT NULL ,\"IDRUTA\" INTEGER NOT NULL ,\"RUTA\" TEXT DEFAULT (null), PRIMARY KEY (\"IDPROG\", \"IDRUTA\") );");
    //        db->query("CREATE TRIGGER \"insert_imgcateg_to_categoria\" BEFORE INSERT ON IMGCATEG BEGIN SELECT RAISE (ROLLBACK, 'SQLException: insert_imgcateg_to_categoria. Constraint: No existe la categoria especificada') WHERE (SELECT categid FROM CATEGORIA WHERE categid = NEW.categid) IS NULL; END;");
    //        db->query("CREATE TRIGGER \"duplicate_imgid\" BEFORE INSERT ON IMGCATEG BEGIN  SELECT RAISE (ROLLBACK, 'SQLException: duplicate_imgid. Constraint: El id de la imagen esta duplicado para esta categoria') WHERE (SELECT imgid FROM IMGCATEG WHERE imgid = NEW.imgid AND categid = NEW.categid) IS NOT NULL; END");
    //        db->query("CREATE TRIGGER \"duplicate_imgpos\" BEFORE INSERT ON IMGCATEG BEGIN SELECT RAISE (ROLLBACK, 'SQLException: duplicate_imgid. Constraint: La posición de la imagen esta duplicada para esta categoria') WHERE (SELECT imgpos FROM IMGCATEG WHERE imgpos = NEW.imgpos AND categid = NEW.categid) IS NOT NULL; END");
        } catch(Excepcion &e){
            Traza::print("Excepcion loadDBFromFile. Message" + string(e.getMessage()), W_ERROR);
        } catch (...){
            Traza::print("Excepcion Generica loadDBFromFile. ", W_ERROR);
        }
    }

}

/**
*
*/
void Gestorroms::closeDB(){
    if (db != NULL) db->close();
}

/**
* Se obtiene un solo resultado de la query
*/
string Gestorroms::getColQuery(const char* query){

    string resultado = "";

    try{
        if (db != NULL) {
            vector<vector<string> > result = db->query(query) ;

            if (result.size() > 0){
                resultado = result.at(0).at(0);
            }
        }
    } catch(Excepcion &e){
        Traza::print("gestorroms::fillMenuByQuery" + string(e.getMessage()), W_ERROR);
//        errorCode = e.getCode();
    }
    return resultado;
}


/**
* Se obtiene un solo resultado de la query
*/
vector<vector<string> > Gestorroms::getRowQuery(string query){

//    int errorCode = 0;
    vector<vector<string> > result;

    try{
        if (db != NULL) {
            result = db->query(query.c_str()) ;
        }
    } catch(Excepcion &e){
        Traza::print("gestorroms::fillMenuByQuery" + string(e.getMessage()), W_ERROR);
//        errorCode = e.getCode();
    }

    return result;
}

/**
*
*/
bool Gestorroms::executeQuery(string query){
    return db->execute(query);
}

/**
*
*/
bool Gestorroms::updateEmulador(Emuinfo::struct_emu *emuData){
    db->prepareStatement("updateEmulador");
    db->setString(0,emuData->nombreEmu);
    db->setString(1,emuData->rutaEmu);
    db->setString(2,emuData->parmsEmu);
    db->setString(3,emuData->rutaRoms);
    db->setString(4,string((emuData->descomp)?"S":"N"));
    db->setString(5,emuData->emuRomExt);
    db->setString(6,emuData->rutaImg);
    db->setString(7,string((emuData->titleRom)?"S":"N"));
    db->setInt   (8,Constant::strToTipo<int>(emuData->idEmu));

    bool res = db->execute();

    if (res){
        res = actualizarRoms(Constant::strToTipo<int>(emuData->idEmu)) == 1 ? true : false;
    }
    return res;
}

/**
*
*/
bool Gestorroms::insertEmulador(Emuinfo::struct_emu *emuData){
    Constant::lowerCase(&emuData->emuRomExt);
    db->prepareStatement("insertEmulador");
    db->setString(0,emuData->nombreEmu);
    db->setString(1,emuData->rutaEmu);
    db->setString(2,emuData->parmsEmu);
    db->setString(3,emuData->rutaRoms);
    db->setString(4,emuData->emuRomExt);
    db->setString(5,emuData->rutaImg);
    db->setString(6,string((emuData->descomp)?"S":"N"));
    db->setString(7,string((emuData->titleRom)?"S":"N"));
    return db->execute();
}

/**
*
*/
bool Gestorroms::deleteEmulador(int idEmulador){
    //Borramos lo que hubiese anteriormente
    db->prepareStatement("deleteemulador");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    return db->execute();
}

vector<vector<string> > Gestorroms::getDatosEmulador(int idEmulador){
    db->prepareStatement("selectEmulador");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);

    return db->executeQuery();
}

/**
*
*/
DWORD Gestorroms::actualizarRoms(int idEmulador){
    Traza::print("Creando querys y borrando de BBDD", W_DEBUG);
    vector<vector<string> > result = getDatosEmulador(idEmulador);

    //Borramos lo que hubiese anteriormente
    db->prepareStatement("deleteroms");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    db->prepareStatement("deleterutas");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    db->prepareStatement("deleterominfo");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    Traza::print("Rominfo creado", W_DEBUG);
    Rominfo *rominfo = new Rominfo();

    try{
        cargarInfoRoms(rominfo);
        //rominfo->loadFromIni("controls.ini");
        Traza::print("Info leida", W_DEBUG);
    } catch(Excepcion &e) {
        Traza::print("Error capturado al cargarInfoRoms: " + string(e.getMessage()), W_ERROR);
    }

    sqlite3 *database = db->getBbdd();
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);

    Traza::print("-----------------------------------------------------------", W_DEBUG);
    for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
        vector<string> row = *it;
        rominfo->emuladorID = Constant::strToTipo<int>(row.at(0));
        rominfo->filtroExtension = row.at(5);
        unsigned long now = SDL_GetTicks();
        try {
            rominfo->romCounter = 0;
            rominfo->rutasCounter = 0;
            listarDirRecursivo(row.at(4).c_str(), rominfo);
            Traza::print("------------------------ Insertado en: " + Constant::TipoToStr( (SDL_GetTicks()-now) / 1000) + " s -----------------------", W_DEBUG);
        } catch(Excepcion &e) {
            Traza::print("Error capturado al actualizarRoms: " + string(e.getMessage()), W_ERROR);
            return 0;
        }
    }
    sqlite3_exec(database, "COMMIT TRANSACTION", NULL, NULL, NULL);
    delete rominfo;
    Traza::print("rominfo borrado", W_DEBUG);
    return 1;
}


/**
* Metodo que se llama desde el menu de opciones para actualizar todos los
* emuladores
*/
DWORD Gestorroms::actualizarRoms(){
    return actualizarRoms(-1);
}

/**
*
*/
void Gestorroms::cargarInfoRoms(Rominfo *infoDatos){
    listaSimple<string> *info = new  listaSimple<string>();
    try{
        unsigned int nlines = info->loadStringsFromFile(getRutaInicial() + FILE_SEPARATOR + "Index.ini");
        string linea = "";
        infoDatos->infoRom = new ListaIni<Data>(nlines);
        int pos = 0;
        vector<string> inforom;

        for (unsigned int i=0; i < info->getSize(); i++){
            linea = info->get(i);
            pos = linea.find_first_of(";");
            if (pos >= 0){
                Data dato;
                dato.setKey(linea.substr(0, pos));

                inforom = Constant::split(linea, ';');
                //linea = inforom.size() > 3 ? inforom[3].substr(1) : "";
                if (inforom.size() > 3){
                    dato.setValue(inforom[3].substr(1));
                    infoDatos->infoRom->add(dato);
                }
            }
        }
        infoDatos->infoRom->sort();
    } catch (Excepcion &e){
        Traza::print("cargarInfoRoms: No se puede cargar Index.ini. " + string(e.getMessage()), W_ERROR);
    }

    try{
        infoDatos->nplayers->loadFromFile(getRutaInicial() + FILE_SEPARATOR + "nplayers.ini");
        infoDatos->nplayers->sort();
    } catch (Excepcion &e){
        Traza::print("cargarInfoRoms: No se puede cargar nplayers.ini. " + string(e.getMessage()), W_ERROR);
    }

    delete info;
}

/**
*
*/
void Gestorroms::listarDirRecursivo(string dir,  Rominfo *rominfo){

    vector <FileProps> *tempfilelist = new vector <FileProps>();
    unsigned int numDirs = 0;

    try{
        Traza::print("Escaneando: " + dir, W_DEBUG);
        numDirs = listarDirSinOrden(dir.c_str(), tempfilelist, rominfo);
        string tempDir = "";

        if (tempfilelist != NULL){
            for (unsigned int i=0; i < numDirs; i++){
                tempDir = dir + FILE_SEPARATOR + tempfilelist->at(i).filename;
                if (tempfilelist->at(i).filetype == TIPODIRECTORIO)
                    listarDirRecursivo(tempDir, rominfo);
            }
            delete tempfilelist;
       }
    } catch(Excepcion &e) {
        Traza::print("Error Dirutil::listarDirRecursivo - " +  string(e.getMessage()), W_ERROR);
        delete tempfilelist;
    }
}

/**
*
*/
unsigned int Gestorroms::listarDirSinOrden(const char *strdir, vector <FileProps> * tempFilelist, Rominfo *rominfo){
    DIR *dp;
    struct dirent *dirp;
    char * concatDir = NULL;
    const char  tempFileSep[2] = {FILE_SEPARATOR,'\0'};
    unsigned int lenDir = 0;
    unsigned int lenFiles = 0;
    Dirutil dirUtil;
    string tempDir = "";
    string tmpQuery = "";
    string tmpQuery2 = "";
    string romname = "";
    string romnamenoext = "";
    string title = "";

    try{
        //Miramos a ver si el directorio a explorar tiene una / al final
        if (strdir != NULL){

            int dirlen = sizeof(strdir);
            bool sepDir = false;
            if (dirlen > 0){
                if (strdir[dirlen-1] != FILE_SEPARATOR){
                    dirlen++;
                    sepDir = true;
                }
            }
            if((dp  = opendir(strdir)) == NULL) {
                Traza::print("Error al listar el directorio: " + string(strdir), W_ERROR);
                throw(Excepcion(EFIO));
            } else {


                vector<vector<string> > result = getDatosEmulador(rominfo->emuladorID);
                string rutaRomsEmu = db->getColQuery(&result,4);
                bool isShowTitle = (db->getColQuery(&result,8) == "S") ? true : false;

                //Recorremos el directorio
                while ((dirp = readdir(dp)) != NULL) {
                    concatDir = new char[dirlen+sizeof(dirp->d_name)+1];//Contamos +1 con el fin de cadena
                    strcpy(concatDir,strdir);
                    if (sepDir){
                        strcat(concatDir,tempFileSep);
                    }
                    strcat(concatDir,dirp->d_name);

                    if (strcmp(dirp->d_name,".") != 0 && strcmp(dirp->d_name,"..") != 0){
                        FileProps propFile;
                        propFile.dir = strdir;
                        propFile.filename = dirp->d_name;

                        if(dirUtil.isDir(concatDir)){
                            propFile.filetype = TIPODIRECTORIO;
                            propFile.ico = folder;
                            tempFilelist->push_back(propFile);
                            lenDir++;
                        } else {
                            propFile.filetype = TIPOFICHERO;
                            propFile.ico = page_white;

                             //Guardamos en una tabla intermedia el valor de las rutas de cada rom encontrada
                            if (propFile.dir.compare(tempDir) != 0){
                                 rominfo->rutasCounter++;
                                 tempDir = propFile.dir;
                                 string rutaSubdir = propFile.dir.substr(rutaRomsEmu.length());

                                 db->prepareStatement("insertRutas");
                                 db->setInt(0,rominfo->emuladorID);
                                 db->setInt(1,rominfo->rutasCounter);
                                 db->setString(2,rutaSubdir);
                                 db->executeNoCommit();
                            }

                            if (rominfo->filtroExtension.empty() ||
                                (!rominfo->filtroExtension.empty() && rominfo->filtroExtension.find(dirUtil.getExtension(propFile.filename)) != string::npos)){
                                //Insertamos los datos importantes para poder ejecutar la rom
                                romname = Constant::replaceAll(propFile.filename,"'","''");
                                db->prepareStatement("insertRoms");
                                db->setInt(0,rominfo->romCounter);
                                db->setInt(1,rominfo->emuladorID);
                                db->setString(2,romname);
                                db->setInt(3,rominfo->rutasCounter);
                                db->executeNoCommit();

                                if (isShowTitle){
                                    romnamenoext = dirUtil.getFileNameNoExt(propFile.filename);
                                    Traza::print("romname: " + romnamenoext, W_PARANOIC);
                                    int pos = rominfo->infoRom->find(romnamenoext);

                                    if (pos != -1){
                                        title = Constant::replaceAll(rominfo->infoRom->get(pos).getValue(),"'","''");
                                        db->prepareStatement("insertRominfo");
                                        db->setInt(0,rominfo->romCounter);
                                        db->setInt(1,rominfo->emuladorID);
                                        db->setString(2,title);
                                        db->executeNoCommit();
                                    }
                                }

                                rominfo->romCounter++;
                            }
                            lenFiles++;
                        }
                    }
                    delete concatDir;
                }
                Traza::print("Se han encontrado " + Constant::TipoToStr(lenDir) + " directorios y " + Constant::TipoToStr(lenFiles) + " ficheros", W_DEBUG);
                closedir(dp);
            }
        }
    } catch(Excepcion &e) {
        Traza::print("Error Dirutil::listarDir - " + string(e.getMessage()), W_ERROR);
        throw(e);
    }
    return lenDir;
}




/**
* Genera el resto de la query a anyadir para generar la clausula where
*
* El formato de las claves será "clave1:valor1,clave2:valor2,clave3:valor3,...,claven:valorn"
*
* el string de resultado sera: and clave1=valor1 and clave2=valor2...
*/
string Gestorroms::parserSQLWhere(string claves){
    string sqlwhere = "";

    if (claves.find(",") != string::npos){
        vector<string> vectorClaves = Constant::split(claves, ',');
        vector<string> valorClaves;
        string clave = "";
        string valor = "";

        for (unsigned int i=0; i < vectorClaves.size(); i++){
            if (vectorClaves.at(i).find("=") != string::npos){
                valorClaves = Constant::split(vectorClaves.at(i), '=');
                if (valorClaves.size() > 0){
                    clave = valorClaves.at(0);
                    valor = valorClaves.at(1);
                    sqlwhere.append(" and r." + clave + "=" + valor);
                    Traza::print("clave=" + clave + " valor=" + valor, W_DEBUG);
                }
            }
        }
    }

    return sqlwhere;

}


/**
*
*/
int Gestorroms::fillMenuByQuery(UIList *refMenu, string query, vector<string> *statementValues, int destino){
    int errorCode = 0;

    try{
        if (db != NULL) {
            db->prepareStatement("query");
            //Si hay parametros, generamos la query con parametros
            if (statementValues == NULL) db->setClauseWhere(false);
            else {
                for (int i=0; i<statementValues->size(); i++){
                    db->setString(i, statementValues->at(i));
                }
            }
            //Lanzamos la query
            vector<vector<string> > result = db->executeQuery();
            //Redimensionamos la lista del menu antes de asignar elementos
            refMenu->resizeLista(result.size());
            //Forzamos al repintado del menu
            refMenu->setImgDrawed(false);
            //Anyadimos los elementos que iran dentro de la lista
            for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
                vector<string> row = *it;
                    refMenu->addElemLista(row.at(1), row.at(0), controller, destino);
            }
            //Recalculamos la posicion de la lista
            refMenu->calcularScrPos();
            refMenu->setImgDrawed(false);
        }
    } catch(Excepcion &e){
        Traza::print("gestorroms::fillMenuByQuery: " + string(e.getMessage()), W_ERROR);
        errorCode = e.getCode();
    }
    return errorCode;
}

/**
*
*/
bool Gestorroms::comprobarUnidad(string diractual, string *unidadRoms){

    vector<vector<string> > result = getDatosEmulador(-1);
    string rutaRom = "";

    bool errorRutas = false;
    string unidadActual = "";

    if (diractual.length() >= 2){
        unidadActual = diractual.substr(0,2);
        Traza::print("Unidad actual= " + unidadActual, W_DEBUG);

        for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
            vector<string> row = *it;
            rutaRom = row.at(4);
            if (rutaRom.length() >= 2){
                if (rutaRom.substr(0,2).find(":") != string::npos){
                    *unidadRoms = rutaRom.substr(0,2);
                    if (unidadActual.compare(*unidadRoms) != 0){
                        errorRutas = true;
                    }
                }
            }
        }
    }
    return errorRutas;
}

/**
*
*/
void Gestorroms::updateRutas(string unidadRoms, string unidadActual){
    db->prepareStatement("updateRutaEmu");
    db->setString(0,unidadRoms);
    db->setString(1,unidadActual);
    db->setString(2,unidadRoms + "%");
    db->execute();

    db->prepareStatement("updateRutaRoms");
    db->setString(0,unidadRoms);
    db->setString(1,unidadActual);
    db->setString(2,unidadRoms + "%");
    db->execute();
}

string Gestorroms::getImgEmulador(int idEmu){
    db->prepareStatement("selectEmulador");
    db->setInt(0,idEmu);
    vector<vector<string> > result = db->executeQuery();
    return db->getColQuery(&result, 7);

}


/**
*
*/
//void Gestorroms::buscarControls(listaSimple<DataList> *listaFin){
//    listaSimple<string> *controls = new listaSimple<string>();
//    controls->loadFromFile("controls.ini");
//
//    DataList *listatemp;
//
//    unsigned int i = 0;
//
//    while (i < controls->getSize()){
//        if (controls->get(i)[0] == '['){
//            listatemp = new DataList();
//            listatemp->key = controls->get(i);
//            i++;
//            while (i < controls->getSize() && controls->get(i)[0] != '['){
//                listatemp->value->add(controls->get(i));
//                i++;
//            }
//            listaFin->add(*listatemp);
//        }
//        i++;
//    }
//    delete controls;
//}
