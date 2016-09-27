#include "gestorroms.h"

Gestorroms::Gestorroms(string ruta){
    //ctor
    db = NULL;
    setRutaInicial(ruta);
    loadDBFromFile(ruta);
    scrappingNow = false;

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
            Emuinfo emuProps;
            for (unsigned int i=0; i < fileRetroarch->getSize(); i++){
                linea = fileRetroarch->get(i);
                if (linea.find("PATH") == 0){
                    emuProps.emuProperties.rutaRoms = Constant::split(linea, "=").at(1);
                } else if (linea.find("COMMAND") == 0){
                    emuProps.emuProperties.rutaEmu = Constant::split(linea, "=").at(1);
                } else if (linea.find("NAME") == 0){
                    emuProps.emuProperties.nombreEmu = Constant::split(linea, "=").at(1);
                } else if (linea.find("EXTENSION") == 0){
                    emuProps.emuProperties.emuRomExt = Constant::split(linea, "=").at(1);
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
    Traza::print("rutaCompleta: " + rutaCompleta, W_PARANOIC);

    if (!existe){
        try{

            db->query("CREATE TABLE CONFIG (PARAMETRO TEXT, VALOR TEXT);");
            db->query("CREATE TABLE EMULADOR (IDPROG INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , NOMBREEMU TEXT, RUTAEMU TEXT, PARMSEMU TEXT, RUTAROMS TEXT, EXTENSIONES TEXT, DESCOMPRIMIR TEXT DEFAULT N,  IMGRUTAFONDO TEXT, SEPARARUTAPARM TEXT DEFAULT S, SHOWTITLE TEXT DEFAULT N, FIXOPTION TEXT, PLATFORM TEXT );");
            db->query("CREATE TABLE ROMINFO (IDROM INTEGER NOT NULL , IDPROG INTEGER NOT NULL , NPLAYERS TEXT, TITLE TEXT, CATEG TEXT, YEAR TEXT, BOXART TEXT, DESCRIPTION TEXT, PUBLISHER TEXT, DEVELOPER TEXT, RATING TEXT, PLATFORM TEXT, SCRAPPED TEXT DEFAULT N, PRIMARY KEY (IDROM, IDPROG));");
            db->query("CREATE TABLE ROMS (IDPROG INTEGER NOT NULL , IDRUTA INTEGER NOT NULL , IDROM INTEGER NOT NULL , NOMBRE TEXT, PRIMARY KEY (IDPROG, IDRUTA, IDROM));");
            db->query("CREATE TABLE RUTAS (IDPROG INTEGER NOT NULL ,IDRUTA INTEGER NOT NULL ,RUTA TEXT DEFAULT (null), PRIMARY KEY (IDPROG, IDRUTA) );");
            db->query("CREATE TABLE TMPROMINFO (IDROM INTEGER NOT NULL , IDPROG INTEGER NOT NULL , NPLAYERS TEXT, TITLE TEXT, CATEG TEXT, YEAR TEXT, BOXART TEXT, DESCRIPTION TEXT, PUBLISHER TEXT, DEVELOPER TEXT, RATING TEXT, PLATFORM TEXT, SCRAPPED TEXT DEFAULT N,  PRIMARY KEY (IDROM, IDPROG));");
            db->query("INSERT INTO CONFIG (PARAMETRO, VALOR) VALUES ('VERSION', '4')");
            db->query("INSERT INTO CONFIG (PARAMETRO, VALOR) VALUES ('FIXDDRAW', '0')");
    //        db->query("CREATE TRIGGER \"insert_imgcateg_to_categoria\" BEFORE INSERT ON IMGCATEG BEGIN SELECT RAISE (ROLLBACK, 'SQLException: insert_imgcateg_to_categoria. Constraint: No existe la categoria especificada') WHERE (SELECT categid FROM CATEGORIA WHERE categid = NEW.categid) IS NULL; END;");
    //        db->query("CREATE TRIGGER \"duplicate_imgid\" BEFORE INSERT ON IMGCATEG BEGIN  SELECT RAISE (ROLLBACK, 'SQLException: duplicate_imgid. Constraint: El id de la imagen esta duplicado para esta categoria') WHERE (SELECT imgid FROM IMGCATEG WHERE imgid = NEW.imgid AND categid = NEW.categid) IS NOT NULL; END");
    //        db->query("CREATE TRIGGER \"duplicate_imgpos\" BEFORE INSERT ON IMGCATEG BEGIN SELECT RAISE (ROLLBACK, 'SQLException: duplicate_imgid. Constraint: La posición de la imagen esta duplicada para esta categoria') WHERE (SELECT imgpos FROM IMGCATEG WHERE imgpos = NEW.imgpos AND categid = NEW.categid) IS NOT NULL; END");
        } catch(Excepcion &e){
            Traza::print("Excepcion loadDBFromFile. Message" + string(e.getMessage()), W_ERROR);
        } catch (...){
            Traza::print("Excepcion Generica loadDBFromFile. ", W_ERROR);
        }
    } else {
        comprobarVersion();
    }
}

void Gestorroms::comprobarVersion(){
    string res = getColQuery("SELECT count(*) FROM sqlite_master WHERE TYPE='table' AND name='CONFIG'");
    int iRes = Constant::strToTipo<int>(res);

    if (iRes == 0){
        db->query("CREATE TABLE \"CONFIG\" (\"PARAMETRO\" TEXT, \"VALOR\" TEXT)");
        db->query("INSERT INTO CONFIG (PARAMETRO, VALOR) VALUES ('VERSION', '1')");
    }

    string strVer = getColQuery("SELECT VALOR FROM CONFIG WHERE PARAMETRO='VERSION'");

    int version = Constant::strToTipo<int>(strVer);

    if (version <= 1){
        db->query("ALTER TABLE EMULADOR ADD COLUMN PLATFORM TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN YEAR TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN BOXART TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN DESCRIPTION TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN PUBLISHER TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN DEVELOPER TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN RATING TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN PLATFORM TEXT");
        db->query("ALTER TABLE ROMINFO ADD COLUMN SCRAPPED TEXT DEFAULT N");
        db->query("UPDATE CONFIG set VALOR = '2' where PARAMETRO = 'VERSION'");
    }

    if (version <= 2){
        db->query("CREATE TABLE \"TMPROMINFO\" (\"IDROM\" INTEGER NOT NULL , \"IDPROG\" INTEGER NOT NULL , \"NPLAYERS\" TEXT, \"TITLE\" TEXT, \"CATEG\" TEXT, \"YEAR\" TEXT, \"BOXART\" TEXT, \"DESCRIPTION\" TEXT, \"PUBLISHER\" TEXT, \"DEVELOPER\" TEXT, \"RATING\" TEXT, \"PLATFORM\" TEXT, \"SCRAPPED\" TEXT DEFAULT N,  PRIMARY KEY (\"IDROM\", \"IDPROG\"));");
        db->query("UPDATE CONFIG set VALOR = '3' where PARAMETRO = 'VERSION'");
    }

    if (version <= 3){
       //Esto es necesario para cambiar el tipo de una columna
       db->query("CREATE TEMPORARY TABLE \"EMULADOR_BKP\" (\"IDPROG\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , \"NOMBREEMU\" TEXT, \"RUTAEMU\" TEXT, \"PARMSEMU\" TEXT, \"RUTAROMS\" TEXT, \"EXTENSIONES\" TEXT, \"DESCOMPRIMIR\" TEXT DEFAULT N,  \"IMGRUTAFONDO\" TEXT, \"SEPARARUTAPARM\" TEXT DEFAULT S, \"SHOWTITLE\" TEXT DEFAULT N, \"FIXOPTION\" INTEGER DEFAULT 0, \"PLATFORM\" TEXT )");
       db->query("INSERT INTO EMULADOR_BKP SELECT IDPROG,NOMBREEMU,RUTAEMU,PARMSEMU,RUTAROMS,EXTENSIONES,DESCOMPRIMIR,IMGRUTAFONDO,SEPARARUTAPARM,SHOWTITLE,FIXOPTION,PLATFORM FROM EMULADOR");
       db->query("DROP TABLE EMULADOR");
       db->query("CREATE TABLE \"EMULADOR\" (\"IDPROG\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL  UNIQUE , \"NOMBREEMU\" TEXT, \"RUTAEMU\" TEXT, \"PARMSEMU\" TEXT, \"RUTAROMS\" TEXT, \"EXTENSIONES\" TEXT, \"DESCOMPRIMIR\" TEXT DEFAULT N,  \"IMGRUTAFONDO\" TEXT, \"SEPARARUTAPARM\" TEXT DEFAULT S, \"SHOWTITLE\" TEXT DEFAULT N, \"FIXOPTION\" TEXT, \"PLATFORM\" TEXT )");
       db->query("INSERT INTO EMULADOR SELECT IDPROG,NOMBREEMU,RUTAEMU,PARMSEMU,RUTAROMS,EXTENSIONES,DESCOMPRIMIR,IMGRUTAFONDO,SEPARARUTAPARM,SHOWTITLE,CASE FIXOPTION WHEN 0 THEN 'N' ELSE 'S' END,PLATFORM FROM EMULADOR_BKP");
       db->query("DROP TABLE EMULADOR_BKP");
       //Finalmente actualizamos la version
       db->query("UPDATE CONFIG set VALOR = '4' where PARAMETRO = 'VERSION'");
       db->query("INSERT INTO CONFIG (PARAMETRO, VALOR) VALUES ('FIXDDRAW', '0')");
    }

     if (version <= 4){
        db->query("CREATE TABLE TMPROMS (IDPROG INTEGER NOT NULL , IDRUTA INTEGER NOT NULL , IDROM INTEGER NOT NULL , NOMBRE TEXT, PRIMARY KEY (IDPROG, IDRUTA, IDROM));");
        db->query("UPDATE CONFIG set VALOR = '5' where PARAMETRO = 'VERSION'");
     }

     if (version <= 5){
        db->query("CREATE INDEX index_roms_name on ROMS (IDPROG, NOMBRE);");
        db->query("REINDEX index_roms_name;");
        db->query("CREATE INDEX index_tmproms_name on TMPROMS (IDPROG, NOMBRE);");
        db->query("REINDEX index_tmproms_name;");
        db->query("UPDATE CONFIG set VALOR = '6' where PARAMETRO = 'VERSION'");
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
    db->setString(8,string((emuData->fixOption)?"S":"N"));
    db->setString(9,emuData->platform);
    db->setInt   (10,Constant::strToTipo<int>(emuData->idEmu));
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
    db->setString(8,string((emuData->fixOption)?"S":"N"));
    db->setString(9,emuData->platform);
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

string Gestorroms::getParameter(string parameter){
    db->prepareStatement("selectParameter");
    db->setString(0,parameter);
    vector<vector<string> > result = db->executeQuery();
    return db->getColQuery(&result, 0);
}

/**
*
*/
DWORD Gestorroms::actualizarRoms(int idEmulador){
    Traza::print("Creando querys y borrando de BBDD", W_DEBUG);
    vector<vector<string> > result = getDatosEmulador(idEmulador);


    //Borramos la tabla temporal
    db->prepareStatement("deletetmprominfo");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    db->prepareStatement("deletetmproms");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    //Antes de borrar rominfo hacemos una copia de seguridad
    db->prepareStatement("coyInTmpRomInfo");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    db->prepareStatement("copyInTmpRoms");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

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

    //Borramos para insertar nuevos registros
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

    //Para no perder datos, actualizamos todo lo nuevo que hemos encontrado
    //con la informacion que ya teniamos
    restaurarRomInfo(idEmulador);

    //Borramos la tabla temporal
    db->prepareStatement("deletetmprominfo");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

    db->prepareStatement("deletetmproms");
    if (idEmulador < 0)
        db->setClauseWhere(false);
    else
        db->setInt(0,idEmulador);
    db->execute();

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
    unsigned long now = SDL_GetTicks();

    Traza::print("Iniciando la carga de los ficheros", W_DEBUG);
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

                inforom = Constant::split(linea, ";");
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
    Traza::print("Index.ini cargado en", SDL_GetTicks() - now, W_DEBUG);
    now = SDL_GetTicks();

    try{
        infoDatos->nplayers->loadFromFile(getRutaInicial() + FILE_SEPARATOR + "nplayers.ini");
        infoDatos->nplayers->sort();
    } catch (Excepcion &e){
        Traza::print("cargarInfoRoms: No se puede cargar nplayers.ini. " + string(e.getMessage()), W_ERROR);
    }
    Traza::print("nplayers.ini cargado en", SDL_GetTicks() - now, W_DEBUG);
    now = SDL_GetTicks();

    mame.loadHistory(getRutaInicial() + FILE_SEPARATOR + "history.dat");
    Traza::print("history.dat cargado en", SDL_GetTicks() - now, W_DEBUG);
    now = SDL_GetTicks();

    delete info;
}







/**
* Genera el resto de la query a anyadir para generar la clausula where
* El formato de las claves será "clave1:valor1,clave2:valor2,clave3:valor3,...,claven:valorn"
* el string de resultado sera: and clave1=valor1 and clave2=valor2...
*/
string Gestorroms::parserSQLWhere(string claves){
    string sqlwhere = "";

    if (claves.find(",") != string::npos){
        vector<string> vectorClaves = Constant::split(claves, ",");
        vector<string> valorClaves;
        string clave = "";
        string valor = "";

        for (unsigned int i=0; i < vectorClaves.size(); i++){
            if (vectorClaves.at(i).find("=") != string::npos){
                valorClaves = Constant::split(vectorClaves.at(i), "=");
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
*
*/
int Gestorroms::fillMenuByQuery(Object *refMenu, string query, vector<string> *statementValues, int destino){
    int errorCode = 0;

    try{
        if (db != NULL) {
            db->prepareStatement(query);
            //Si hay parametros, generamos la query con parametros
            if (statementValues == NULL) db->setClauseWhere(false);
            else {
                for (int i=0; i<statementValues->size(); i++){
                    db->setString(i, statementValues->at(i));
                }
            }
            //Lanzamos la query
            vector<vector<string> > result = db->executeQuery();

            UIListCommon *refListaComun = (UIListCommon *)refMenu;
            //Redimensionamos la lista del menu antes de asignar elementos
            refListaComun->resizeLista(result.size());
            //Forzamos al repintado del menu
            refListaComun->setImgDrawed(false);

            if (refMenu->getObjectType() == GUILISTBOX){
                UIList *refMenuList = (UIList *)refMenu;
                //Anyadimos los elementos que iran dentro de la lista
                for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
                    vector<string> row = *it;
                        refMenuList->addElemLista(row.at(1), row.at(0), controller, row.at(2), destino);
                }
            } else if (refMenu->getObjectType() == GUILISTGROUPBOX){
                UIListGroup *refMenuList = (UIListGroup *)refMenu;
                //Anyadimos los elementos que iran dentro de la lista
                for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
                    vector<string> row = *it;
                        vector <ListGroupCol *> miFila;

                        if (row.size() > 1)
                            miFila.push_back(new ListGroupCol(row.at(1), row.at(0), controller, destino));

                        if (row.size() > 2)
                            miFila.push_back(new ListGroupCol(row.at(2), "", controller, destino));
                        refMenuList->addElemLista(miFila);
                }
            }
            //Recalculamos la posicion de la lista
            refListaComun->calcularScrPos();
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

    db->prepareStatement("updateRutaImgs");
    db->setString(0,unidadRoms);
    db->setString(1,unidadActual);
    db->setString(2,unidadRoms + "%");
    db->execute();
}

/**
* Obtiene la imagen del emulador
*/
string Gestorroms::getImgEmulador(int idEmu){
    db->prepareStatement("selectEmulador");
    db->setInt(0,idEmu);
    vector<vector<string> > result = db->executeQuery();
    return db->getColQuery(&result, 7);
}

/**
* Actualiza la informacion de todos los emuladores
*/
DWORD Gestorroms::thScrapAllSystem(){
    try{
        db->prepareStatement("selectListaEmuladores");
        Traza::print("Gestorroms::thScrapAllSystem. Obteniendo lista de sistemas", W_DEBUG);
        vector<vector<string> > result = db->executeQuery();
        for (int i=0; i < result.size(); i++){
            scrapsystem(result.at(i).at(0));
        }
    } catch (Excepcion &e) {
         Traza::print("Excepcion Gestorroms::thScrapAllSystem" + string(e.getMessage()), W_ERROR);
    }
    return 0;
}

/**
* Funcion para llamar desde el thread principal
*/
DWORD Gestorroms::thScrapSystem() {
    return scrapsystem(getThEmuID());
}

/**
* Obtiene la informacion de todas las roms del emulador que se le pasa por parametro
*/
DWORD Gestorroms::scrapsystem(string idEmu){
    int errorCode = 0;
    int i=0;

    try{
        scrappingNow = true;
        if (db != NULL) {
            Traza::print("Gestorroms::scrapsystem. Obteniendo datos de emu " + idEmu, W_DEBUG);
            vector<vector<string> > result = getDatosEmulador(Constant::strToTipo<int>(idEmu));
            if (result.size() > 0){
                platform = result.at(0).at(10);
                if (!platform.empty()){
                    db->prepareStatement("selectRomsForInfo");
                    //Si hay parametros, generamos la query con parametros
                    db->setClauseWhere(true);
                    db->setString(0, idEmu);
                    if (Constant::isUPDATE_MISSING()){
                        db->setRaw(1, " and ifnull(ri.SCRAPPED,'N') = 'N' ");
                    } else {
                        db->setRaw(1, "");
                    }

                    //Lanzamos la query
                    vector<vector<string> > listaRoms = db->executeQuery();
//                    vector<string> rom;
//                    rom.push_back("744");//IDPROG
//                    rom.push_back("12");//IDROM
//                    rom.push_back("Alex Kidd in Miracle World");//title
//                    rom.push_back("");//nplayers
//                    rom.push_back("");//scrapped
//                    vector<vector<string> > listaRoms;
//                    listaRoms.push_back(rom);
                    for ( i; i < listaRoms.size(); i++){
                        this->progress = Constant::TipoToStr((int)(i/(float)listaRoms.size() * 100)) + "%";
                        RomWebInfo *objRom = new RomWebInfo(i, &listaRoms, platform);
                        objRom->updateGameInfo();
                        addRomInfo(&listaRoms, i, objRom);
                    }
                }
            }
        }
    } catch(Excepcion &e){
        Traza::print("scrapsystem: " + string(e.getMessage()), W_ERROR);
        errorCode = e.getCode();
    }
    scrappingNow = false;

    return 0;
}

/**
*
*/
DWORD Gestorroms::addRomInfo(vector<vector<string> > *listaRoms, int posRomProcess, RomWebInfo *objRom){
    string scrapped = "";
    string idprog = "";
    string idrom = "";
    for (int j=0; j < Constant::getMAXTHREADSDOWN(); j++){
        ResponseGame *resp = objRom->getResponse();
        //Cuando haya terminado, debemos anaydirlos a la bbdd. Esta parte esta sincronizada
        //cuando todos los threads han terminado
        idprog = listaRoms->at(posRomProcess).at(0);
        idrom = listaRoms->at(posRomProcess).at(1);
        scrapped = listaRoms->at(posRomProcess).at(4);

        if (scrapped.empty()){
            db->prepareStatement("insertInfoRom");
        } else {
            db->prepareStatement("updateInfoRom");
            db->setClauseWhere(true);
        }

        if (resp != NULL){
            db->setString(0,resp->players);
            db->setString(1,Constant::replaceAll(resp->gameTitle,"'","''"));
            db->setString(2,resp->genres.size() > 0 ? resp->genres.at(0) : "");
            db->setString(3,resp->releaseDate);
            db->setString(4,resp->boxartFront);
            db->setString(5,Constant::replaceAll(resp->overview,"'","''"));
            db->setString(6,Constant::replaceAll(resp->publisher,"'","''"));
            db->setString(7,Constant::replaceAll(resp->developer,"'","''"));
            db->setString(8,resp->rating);
            db->setString(9,Constant::replaceAll(resp->platform,"'","''"));
            db->setString(10,"S");
            db->setString(11,idprog);
            db->setString(12,idrom);
            db->execute();

        } else {
            db->setString(0, "");
            db->setString(1, "");
            db->setString(2, "");
            db->setString(3, "");
            db->setString(4, "");
            db->setString(5, "");
            db->setString(6, "");
            db->setString(7, "");
            db->setString(8, "");
            db->setString(9, "");
            db->setString(10,"P");
            db->setString(11,idprog);
            db->setString(12,idrom);
            db->execute();
        }
        delete objRom;
    }
    return 0;
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
                bool isFindInfoRoms = (db->getColQuery(&result,8) == "S") ? true : false;
                vector<vector<string> > datosAntInfoRom;

                Traza::print("rominfo->filtroExtension: " + rominfo->filtroExtension, W_PARANOIC);

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

                            Traza::print("Para: " + propFile.filename + " tenemos ext: " + dirUtil.getExtension(propFile.filename), W_PARANOIC);
                            bool cumpleFiltro = false;
                            if (rominfo->filtroExtension.find(dirUtil.getExtension(propFile.filename)) != std::string::npos){
                                cumpleFiltro = true;
                            }

                            if (rominfo->filtroExtension.empty() ||
                                (!rominfo->filtroExtension.empty() && cumpleFiltro)){
                                //Insertamos la rom y creamos la informacion de la rom en la tabla ROMINFO
                                createRomInfo(rominfo, isFindInfoRoms, propFile.filename);
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
*
*/
void Gestorroms::createRomInfo(Rominfo *rominfo, bool isFindInfoRoms, string filename){
    int posTitle = -1;
    int posPlayers = -1;
    BDRomInfoOut infoRom;
    Dirutil dirUtil;
    map<string, string> romData;

    //Insertamos los datos importantes para poder ejecutar la rom
    string romname = Constant::replaceAll(filename,"'","''");
    db->prepareStatement("insertRoms");
    db->setInt(0,rominfo->romCounter);
    db->setInt(1,rominfo->emuladorID);
    db->setString(2,romname);
    db->setInt(3,rominfo->rutasCounter);
    db->executeNoCommit();
    Traza::print("Insertando rom: " + romname + "; " + Constant::TipoToStr(rominfo->romCounter)
                 + "; " + Constant::TipoToStr(rominfo->emuladorID)
                 + "; " + Constant::TipoToStr(rominfo->rutasCounter), W_PARANOIC);

    //Si hemos especificado buscar informacion de roms en el fichero
    if (isFindInfoRoms){
        string romnamenoext = dirUtil.getFileNameNoExt(filename);
        Traza::print("romname: " + romnamenoext, W_PARANOIC);
        posPlayers = rominfo->nplayers->find(romnamenoext);
        if (posPlayers != -1){
            infoRom.Setnplayers(rominfo->nplayers->get(posPlayers).getValue());
        }

        infoRom.Settitle("");
        posTitle = rominfo->infoRom->find(romnamenoext);
        if (posTitle != -1){
            infoRom.Settitle(Constant::replaceAll(rominfo->infoRom->get(posTitle).getValue(),"'","''"));
        }

        romData = mame.getRomData(romnamenoext);
        if (!romData.empty()){
            //Buscamos informacion de la rom del fichero history.dat
            infoRom.Setdescription(Constant::replaceAll(romData.at(HISTORY_DESC),"'","''"));
            infoRom.Setdeveloper(Constant::replaceAll(romData.at(HISTORY_DEVELOPER),"'","''"));
            infoRom.Setpublisher(Constant::replaceAll(romData.at(HISTORY_DEVELOPER),"'","''"));
//            infoRom.Settitle(Constant::replaceAll(romData.at(HISTORY_TITLE),"'","''"));
            infoRom.Setyear(Constant::replaceAll(romData.at(HISTORY_YEAR),"'","''"));
        }


        //Buscamos la imagen si existe guardada en el disco
        string fileImgSnap = Constant::getAppDir()
            + FILE_SEPARATOR + DIR_IMG_ROMS_IMGS
            + FILE_SEPARATOR + "snap"
            + FILE_SEPARATOR + romnamenoext + ".png";

        //Para los juegos de mame, comprobamos si existe su captura de pantalla en la carpeta snap del directorio de imagenes
        if (dirUtil.existe(fileImgSnap)){
            infoRom.Setboxart(Constant::TipoToStr(FILE_SEPARATOR) + "snap" + Constant::TipoToStr(FILE_SEPARATOR) + romnamenoext + ".png");
        }
    }

    bool someFound = !infoRom.Getdescription().empty() || !infoRom.Getnplayers().empty()
        || !infoRom.Gettitle().empty() || !infoRom.Getcateg().empty() || !infoRom.Getyear().empty()
        || !infoRom.Getboxart().empty() || !infoRom.Getpublisher().empty() || !infoRom.Getdeveloper().empty()
        || !infoRom.Getrating().empty() || !infoRom.Getplatform().empty() || !infoRom.Getscrapped().empty();

    db->prepareStatement("insertRomAllinfo");
    //especificamos todos los parametros
    db->setInt(   0,  rominfo->romCounter);
    db->setInt(   1,  rominfo->emuladorID);
    db->setString(2,  infoRom.Gettitle());
    db->setString(3,  infoRom.Getnplayers());
    db->setString(4,  ""); //CATEG
    db->setString(5,  infoRom.Getyear()); //YEAR
    db->setString(6,  infoRom.Getboxart()); //BOXART
    db->setString(7,  infoRom.Getdescription()); //DESCRIPTION
    db->setString(8,  infoRom.Getpublisher()); //PUBLISHER
    db->setString(9,  infoRom.Getdeveloper()); //DEVELOPER
    db->setString(10, ""); //RATING
    db->setString(11, ""); //PLATFORM
    db->setString(12, someFound ? "S" : "N"); //SCRAPPED
    db->executeNoCommit();

    //Aumentamos el contador
    rominfo->romCounter++;

}

/**
*
*/
bool Gestorroms::updateParameter(string parameter, string value){
    db->prepareStatement("updateparameter");
    db->setString(0,value);
    db->setString(1,parameter);
    return db->execute();
}

/**
*
*/
void Gestorroms::restaurarRomInfo(int idEmu){

    unsigned long now = SDL_GetTicks();
    Traza::print("Generando query para actualizar rominfo...", W_DEBUG);

    db->prepareStatement("getNewChanges");
    if (idEmu >= 0){
        db->setRaw(0, "tr.idprog = " + Constant::TipoToStr(idEmu) + " and ");
    } else {
        db->setRaw(0, "");
    }

    vector<vector<string> > listaRoms = db->executeQuery();
    Traza::print("Info Obtenida en", SDL_GetTicks() - now,  W_DEBUG);
    sqlite3 *database = db->getBbdd();
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);

    Traza::print("TOTAL",listaRoms.size(), W_DEBUG);
    for (int i=0; i<listaRoms.size(); i++){
        Traza::print("UPDATING: " + listaRoms.at(i).at(2) + "; " + listaRoms.at(i).at(0) , W_PARANOIC);
        db->prepareStatement("updateNewChanges");
        for (int j=0; j<11; j++){
            db->setString(0 + j, Constant::replaceAll(listaRoms.at(i).at(4 + j),"'","''"));
        }
        db->setString(11, listaRoms.at(i).at(2));  //Especificamos el emulador
        db->setString(12, listaRoms.at(i).at(0)); //Especificamos el nuevo idrom a actualizar
        db->executeNoCommit();
    }
    //Finalmente hacemos el commit
    sqlite3_exec(database, "COMMIT TRANSACTION", NULL, NULL, NULL);
    Traza::print("Informacion actualizada en: " + Constant::TipoToStr((SDL_GetTicks() - now)), W_DEBUG);
}
