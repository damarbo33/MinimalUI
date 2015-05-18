#include "main.h"
#include "Dirutil.h"
//#include "httputil.h"

bool procesarTeclado(tEvento *evento, Iofrontend *ioFront){
    bool salir = false;
    if (evento->key == SDLK_RETURN && evento->keyMod & KMOD_LALT){
        ioFront->toggleFullScreen();
        evento->resize = true;
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
        appDir = appDir.substr(0, appDir.rfind(Constant::getFileSep()));
        if (appDir[appDir.length()-1] == '.'){
            appDir.substr(0, appDir.rfind(Constant::getFileSep()));
        }
        Constant::setAppDir(appDir);
    #endif // WIN

    #ifdef UNIX
        Dirutil dir;
        Constant::setAppDir(dir.getDirActual());
    #endif // UNIX

    Traza *traza = new Traza();
    Iofrontend *ioFront = new Iofrontend();

//    HttpUtil *util = new HttpUtil();
//    util->download("http://www.walpaper.es/images/wallpapers/espectro-de-colores-8349.jpeg");
//    util->writeToFile("C:\\espectro-de-colores-8349.jpeg");
//    delete util;

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
                //esta diferencia. No es el mejor método pero es sencillo de implementar
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
    Traza::print("Saliendo de la aplicación", W_DEBUG);
    exit(0);
}


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
    Traza::print("Saliendo de la aplicación", W_DEBUG);
    exit(0);
}
*/
/*
int main(int argc, char *argv[]){
    string ss = "";

//    ss +="--OBTENEMOS EL MAXIMO NUMERO DE OPERACIONES DE LOS CAJEROS \n";
//    ss +=" select A.deviceid as Cajero, substring(A.maximo,0,charindex(';',A.maximo)) as Operaciones from(\n";
//    ss +=" SELECT deviceid, max(substring(orgmessage,charindex('OP=',orgmessage)+3,9)) as maximo\n";
//    ss +=" FROM event WHERE messageno = 51001 and servertimestamp >= CONVERT(DATETIME,'2014/02/%1% 06:00:00 AM',20) and servertimestamp >= CONVERT(DATETIME,'2014/02/%2% 05:30:00 AM',20) group by deviceid) as A\n";
//    ss +=" ,basedata b, device c, deviceprofile d where A.deviceid = b.id and b.reference = 999995\n";
//    ss +=" and A.deviceid = c.deviceid\n";
//    ss +=" and c.devicetype = d.devicetype\n";
//    ss +=" and A.deviceid in (\n";
//    ss +=" --OBTENEMOS LOS CAJEROS QUE HAN REINICIADO EN EL MISMO DIA\n";
//    ss +=" select distinct Cajero from(\n";
//    ss +=" select A.deviceid as Cajero, substring(A.OP,0,charindex(';',A.OP)) as OP, A.FEC from(\n";
//    ss +=" SELECT deviceid, substring(orgmessage,charindex('OP=',orgmessage)+3,9) as OP, substring(orgmessage,charindex('FEC=',orgmessage)+4,19) as FEC, charindex('FEC=',orgmessage) as indice\n";
//    ss +=" FROM event WHERE messageno = 51001 and servertimestamp >= CONVERT(DATETIME,'2014/02/%1% 05:00:00 AM',20) and servertimestamp < CONVERT(DATETIME,'2014/02/%1% 08:00:00 AM',20)) \n";
//    ss +=" as A where indice > 0)  as B where OP = 0)\n";


  ss += " select Cajero, max(Convert(int, Operaciones)) as Operaciones from (\n";
   ss += " select A.deviceid as Cajero, substring(A.maximo,0,charindex(';',A.maximo)) as Operaciones, servertimestamp from(\n";
   ss += " SELECT deviceid, substring(orgmessage,charindex('OP=',orgmessage)+3,9) as maximo, servertimestamp\n";
   ss += " FROM event WHERE messageno = 51001 and servertimestamp >= CONVERT(DATETIME,'2014-02-%1% 06:00:00',120) and servertimestamp < CONVERT(DATETIME,'2014-02-%2% 05:30:00',120) )  as A\n";
   ss += " ,basedata b, device c, deviceprofile d where A.deviceid = b.id and b.reference = 999995\n";
   ss += " and A.deviceid = c.deviceid\n";
   ss += " and c.devicetype = d.devicetype\n";
   ss += " and A.deviceid in (\n";
   ss += "   --OBTENEMOS LOS CAJEROS QUE HAN REINICIADO EN EL MISMO DIA\n";
   ss += "   select distinct Cajero from(\n";
   ss += "  select A.deviceid as Cajero, substring(A.OP,0,charindex(';',A.OP)) as OP from(\n";
   ss += "   SELECT deviceid, substring(orgmessage,charindex('OP=',orgmessage)+3,9) as OP\n";
   ss += "   FROM event WHERE messageno = 51001 and servertimestamp >= CONVERT(DATETIME,'2014-02-%1% 05:00:00',120) and servertimestamp < CONVERT(DATETIME,'2014-02-%1% 08:00:00',120)) \n";
   ss += "   as A )  as B where OP in ('0','1'))\n";
   ss += " ) RESULTADO group by Cajero\n";

    string res  = "select Cajero, Operaciones, b.value as Localizacion, d.devicedescr as Descripcion  from (\n";
           res += "select Cajero, SUM(CAST(Operaciones AS INT)) as Operaciones from (\n";

    string temp = "";
    int maximo = 7;

    for (int i=1; i <= maximo; i++){
        temp = Constant::replaceAll(ss,"%1%",Constant::pad(Constant::TipoToStr(i),2,'0'));
        temp = Constant::replaceAll(temp,"%2%",Constant::pad(Constant::TipoToStr(i+1),2,'0'));
        res += temp;
        if (i < maximo) res += "union\n";
    }

    res += ") AS F group by Cajero) AS G\n";
    res += ", basedata b, device c, deviceprofile d where G.Cajero = b.id and b.reference = 999995\n";
    res += "and G.Cajero = c.deviceid\n";
    res += "and c.devicetype = d.devicetype\n";
    cout << res << endl;
    return 0;
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
