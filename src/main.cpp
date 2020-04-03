#include "main.h"
#include "Dirutil.h"
#include "image/uiimgdownloader.h"
#include "roms/mamehistoryparser.h"
// using ofstream constructors.
#include <iostream>
#include <fstream>

bool procesarTeclado(tEvento *evento, Iofrontend *ioFront){
    bool salir = false;
    static int mode = 0;
    
    if (evento->key == SDLK_RETURN && evento->keyMod & KMOD_LALT){
        ioFront->toggleFullScreen();
        evento->resize = true;
    } else if (evento->key == SDLK_f && evento->keyMod & KMOD_CTRL){
        mode++;
        if (ioFront->setFullscreenMode(mode) == -1)
            mode = 0;
    }
    return salir;
}

/**
*
*/
void Terminate(void)
{
#ifdef UNIX
//    chdir("/home/pi");
//    execl("/bin/sh", "/bin/sh", NULL);
#endif
}

/**
*
*/

int main(int argc, char *argv[]){
    #ifdef WIN
        string appDir = argv[0];
        int pos = appDir.rfind(Constant::getFileSep());
        if (pos == string::npos){
            FILE_SEPARATOR = FILE_SEPARATOR_UNIX;
            pos = appDir.rfind(FILE_SEPARATOR);
            tempFileSep[0] = FILE_SEPARATOR;
        }
        appDir = appDir.substr(0, pos);
        if (appDir[appDir.length()-1] == '.'){
            appDir.substr(0, appDir.rfind(Constant::getFileSep()));
        }
        Constant::setAppDir(appDir);
    #endif // WIN

    #ifdef UNIX
        Dirutil dir;
        Constant::setAppDir(dir.getDirActual());
    #endif // UNIX

    string rutaTraza = appDir + Constant::getFileSep() + "Traza.txt";

    Traza *traza = new Traza(rutaTraza.c_str());
    Iofrontend *ioFront = new Iofrontend();

    try{

        bool salir = false;
        long delay = 0;
        unsigned long before = 0;
        ioFront->setCanFlip(true);
        atexit (Terminate);

        while (!salir){

            tEvento evento = ioFront->WaitForKey();
            procesarTeclado(&evento, ioFront);

            before = SDL_GetTicks();
            salir = evento.quit;

            if (!salir){
                salir = ioFront->drawMenu(evento);

                if (LIMITFPS){
                //Calculamos el tiempo que deberia pasar de frame a frame en un lapso de 1 seg (TIMETOLIMITFRAME) y le restamos
                //el tiempo que realmente ha pasado (before - SDL_GetTicks()), para esperar el tiempo que indique
                //esta diferencia. No es el mejor metodo pero es sencillo de implementar
                    delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
                    if(delay > 0) SDL_Delay(delay);
                }
            }
        }
    } catch(Excepcion &e) {
        Traza::print("Error capturado en hilo principal:",e.getCode(), W_FATAL);
        //exit(e.getCode());
    }

    Traza::print("Eliminando iofront", W_DEBUG);
    delete ioFront;
    Traza::print("Eliminando traza", W_DEBUG);
    delete traza;
    Traza::print("Saliendo de la aplicacion", W_DEBUG);
    exit(0);
}


/**PARA CREAR ROMS FICTICIAS*/
/*
int main(int argc, char *argv[]){

   Database *db = new Database("C:\\Program Files (x86)\\Codeblocks\\Projects\\MinimalUI\\MinimalUI\\doc\\Recursos\\romgestor.sqlite");
   string directOut = "C:\\Program Files (x86)\\Codeblocks\\Projects\\MinimalUI\\MinimalUI\\doc\\roms\\mame2\\";
   string output;

    try{
        if (db != NULL) {
            vector<vector<string> > result = db->query("SELECT nombre FROM ROMS Where idprog = 759");
            for (int i=0; i < result.size(); i++){
                cout << result.at(i).at(0) << endl;
                output = directOut + result.at(i).at(0);
                std::ofstream outfile (output.c_str());
                outfile.close();
            }
        }
    } catch(Excepcion &e){
        Traza::print("gestorroms::fillMenuByQuery" + string(e.getMessage()), W_ERROR);
    }

    return 0;
}
*/
/*
int main(int argc, char *argv[]){
    MameHistoryParser *mame = new MameHistoryParser();
    mame->loadHistory("C:\\history.dat");
    cout << "HISTORY_TITLE: " << mame->getRomSection("joust", HISTORY_TITLE) << endl;
    cout << "HISTORY_YEAR: " << mame->getRomSection("joust", HISTORY_YEAR) << endl;
    cout << "HISTORY_DEVELOPER: " << mame->getRomSection("joust", HISTORY_DEVELOPER) << endl;
    cout << "HISTORY_PLAYERS: " << mame->getRomSection("joust", HISTORY_PLAYERS) << endl;
    cout << "HISTORY_DESC: " << mame->getRomSection("joust", HISTORY_DESC) << endl;
    cout << "HISTORY_TECH: " << mame->getRomSection("joust", HISTORY_TECH) << endl;
    cout << "HISTORY_TRIV: " << mame->getRomSection("joust", HISTORY_TRIV) << endl;
    cout << "HISTORY_SCOR: " << mame->getRomSection("joust", HISTORY_SCOR) << endl;
    cout << "HISTORY_TIPS: " << mame->getRomSection("joust", HISTORY_TIPS) << endl;
    delete mame;
    return 0;
}
*/

/**
void pruebaConexion(){
    RequestQuery queryGame;
    queryGame.id = "2684";
    //queryGame.name = "Alex Kidd";
    //queryGame.exactName = "Alex Kidd";
    TheGamesDB gamesDB;
    ResponseQuery response;

    int petOK = gamesDB.download2(&queryGame, &response);
    cout << "Recuperados: " << response.gameList.size() << endl;

    for (int i=0; i < response.gameList.size(); i++){
            cout << "El id es: " << response.gameList.at(i)->id << endl;
            cout << "El titulo es: " << response.gameList.at(i)->gameTitle << endl;
            cout << "El platformId es: " << response.gameList.at(i)->platformId << endl;
            cout << "El platform es: " << response.gameList.at(i)->platform << endl;
            cout << "El releaseDate es: " << response.gameList.at(i)->releaseDate << endl;
            cout << "El genres es: " << response.gameList.at(i)->genres.size() << endl;
            cout << "El players es: " << response.gameList.at(i)->players << endl;
            cout << "El publisher es: " << response.gameList.at(i)->publisher << endl;
            cout << "El developer es: " << response.gameList.at(i)->developer << endl;
            cout << "El rating es: " << response.gameList.at(i)->rating << endl;
            cout << "El baseImgUrl es: " << response.gameList.at(i)->baseImgUrl << endl;
            cout << "El boxart es: " << response.gameList.at(i)->boxart.size() << endl;
            cout << "El fanart es: " << response.gameList.at(i)->fanart.size() << endl;
            cout << "******************************" << endl << endl;
    }

    switch (petOK){
        case QUERYOK :
            cout << "Peticion OK" << endl;
            break;
        case GAMENOTFOUND:
            cout << "Juego no encontrado: " << response.error << endl;
            break;
        case CONNECTERROR:
            cout << "Problema de conexion" << endl;
            break;
        default:
            cout << "Error no determinado" << endl;
            break;
    }

}*/
/*
void generarQuery(){
    vector <string> parms;

    parms.push_back("420");
//    parms.push_back("'?�PRUEBA 2?'");
//    parms.push_back("'?�WHAT IS REALITY?'");

    string query = " SELECT IDPROG, NOMBREEMU, RUTAEMU,PARMSEMU, RUTAROMS, EXTENSIONES, DESCOMPRIMIR, IMGRUTAFONDO, SHOWTITLE, FIXOPTION, PLATFORM FROM EMULADOR WHERE IDPROG=?";

    int last = query.length();
    cout <<  "query.length(): "<< query.length() << endl;

    for (int i= parms.size() - 1; i >= 0 ; i--){
        last = query.find_last_of('?', last);
        cout << last << endl;
        query = Constant::replace(query, "?", parms.at(i), last);
    }

    cout << query << endl;


}

*/

void loadAndSave(){
    UIImgDownloader imgDownloader;
    //imgDownloader.download("http://fotos.pccomponentes.com/procesadores/procesadores_intel_socket_1150/intel_core_i7_4790k_4_0_ghz_box.jpg",
    //                       "C:\\corei7_res2.jpg", 50, 50);

//    imgDownloader.download("http://www.smalldog.com/SmallDogPriceList.txt",
//                           "C:\\Error.jpg", 50, 50);
}

/*
int main(int argc, char *argv[]){
    loadAndSave();
    return 0;
}
*/


/*
int main(int argc, char *argv[]){

    string appDir = argv[0];
    appDir = appDir.substr(0, appDir.rfind(Constant::getFileSep()));
    if (appDir[appDir.length()-1] == '.'){
        appDir.substr(0, appDir.rfind(Constant::getFileSep()));
    }
    Constant::setAppDir(appDir);

    Traza *traza = new Traza();
    Iofrontend *ioFront=new Iofrontend();

    try{

        bool salir = false;
        long delay = 0;
        unsigned long before = 0;
        ioFront->setCanFlip(true);
        atexit (Terminate);

        ioFront->setSelMenu(PANTALLAGROUPLIST);

        while (!salir){

            tEvento evento = ioFront->WaitForKey();
            procesarTeclado(&evento, ioFront);

            before = SDL_GetTicks();
            salir = evento.quit;

            if (!salir){
                 //Procesando el redimensionado de ventana
                if (evento.resize){
                    ioFront->resizeMenu();
                }

                salir = ioFront->drawMenu(evento);

                if (LIMITFPS){
                //Calculamos el tiempo que deberia pasar de frame a frame en un lapso de 1 seg (TIMETOLIMITFRAME) y le restamos
                //el tiempo que realmente ha pasado (before - SDL_GetTicks()), para esperar el tiempo que indique
                //esta diferencia
                    delay = before - SDL_GetTicks() + TIMETOLIMITFRAME;
                    if(delay > 0) SDL_Delay(delay);
                }
            }
        }
    } catch(Excepcion &e) {
        Traza::print("Error capturado en hilo principal:",e.getCode(), W_FATAL);
        //exit(e.getCode());
    }

    Traza::print("Eliminando iofront", W_DEBUG);
    if (ioFront != NULL) delete ioFront;
    Traza::print("Eliminando traza", W_DEBUG);
    delete traza;
    Traza::print("Saliendo de la aplicaci�n", W_DEBUG);
    exit(0);
}
*/

/*
int main(int argc, char *argv[]){
    cout << "Inicio"<<endl;
    string ss = "Estamos en la b %ROM% de la primera";
    Constant::lowerCase(&ss);
    char* METACHARS[] = {"*", "?", "[", "]", "'", """", "$", ";", "&", "(", ")", "|", "^", "<", ">"};
    cout << Constant::replaceAll(ss,"%ROM%","") << endl;
    cout << "metas: " << sizeof(METACHARS)/sizeof(METACHARS[0]) << endl;
    string ruta = "/mnt/roms/""AD&D.zip";
    cout << Constant::replaceMetaCaracters(ruta) << endl;
    return 0;
}
*/

/**
*
*/
/*
int main(int argc, char *argv[]){

    int id = 1;
    int rutas = 1;
    string ruta = "miruta";

    unsigned long now = SDL_GetTicks();
    string tmpQuery2 = "INSERT INTO RUTAS (IDPROG, IDRUTA, RUTA) VALUES (";
    tmpQuery2.append(Constant::TipoToStr(id));
    tmpQuery2.append(",");
    tmpQuery2.append(Constant::TipoToStr(rutas));
    tmpQuery2.append(",'");
    tmpQuery2.append(ruta);
    tmpQuery2.append("');");

    cout << "tiempo append=" << (unsigned long)(SDL_GetTicks() - now) << endl;

    now = SDL_GetTicks();
    string tmpQuery3 = "INSERT INTO RUTAS (IDPROG, IDRUTA, RUTA) VALUES (%1%, %2%, '%3%');";
    tmpQuery3 = Constant::replaceAll(tmpQuery3, "%1%", Constant::TipoToStr(id));
    tmpQuery3 = Constant::replaceAll(tmpQuery3, "%2%", Constant::TipoToStr(rutas));
    tmpQuery3 = Constant::replaceAll(tmpQuery3, "%3%", ruta);
    cout << "tiempo append=" << (unsigned long)(SDL_GetTicks() - now) << endl;

    return 0;
}*/

/**
PRUEBAS DE CARGA DE STRINGS
*/
//int main(int argc, char *argv[]){
//    Traza *traza = new Traza();
//    traza->print("Inicio pruebas", W_DEBUG);
//
//    listaSimple<string> *lista = new listaSimple<string>();
//
//    try{
//        lista->loadStringsFromFile("C:\\Users\\dmarcobo\\Desktop\\FiltroProview.txt");
//        string linea = "";
//        int inicio = -1;
//        int fin = -1;
//        Traza::print("size: " + Constant::TipoToStr(lista->getSize()), W_DEBUG);
//        for (int i= 0; i< lista->getSize(); i++){
//            linea = lista->get(i);
//            if (linea.length() > 0 && linea.at(0) != ';'){
//                inicio = linea.find(":");
//                fin = linea.find("=");
//                if (fin > inicio && inicio > 0){
//                    Traza::print(linea.substr(inicio+1, fin-inicio-1), W_DEBUG);
//                }
//            }
//
//            inicio = -1;
//            fin = -1;
//        }
//        delete lista;
//
//    } catch (Excepcion &e){
//        traza->print("Excepcion pruebas", W_ERROR);
//    }
//    return 0;
//}

//int main(int argc, char *argv[]){
//
//  CURL *curl;
//  CURLcode res;
//  Traza *traza = new Traza();
//
//  curl = curl_easy_init();
//  if(curl) {
//    curl_easy_setopt(curl, CURLOPT_URL, "http://www.google.com");
//    /* example.com is redirected, so we tell libcurl to follow redirection */
//    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
//
//    /* Perform the request, res will get the return code */
//    res = curl_easy_perform(curl);
//    /* Check for errors */
//    if(res != CURLE_OK)
//      fprintf(stderr, "curl_easy_perform() failed: %s\n",
//              curl_easy_strerror(res));
//
//    /* always cleanup */
//    curl_easy_cleanup(curl);
//  }
//
//  delete traza;
//  return 0;
//
//}
//
//#include <iostream>
//#include <ctime>
//
//int main(int argc, char *argv[]){
//    long input_seconds = 300;
//    cout << Constant::timeFormat(input_seconds) << endl;
//
//   return 0;
//
//}
