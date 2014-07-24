#include "Launcher.h"

#define LEER		0
#define ESCRIBIR	1
#define MAXTUBERIALEN 2000
/**
*
*/
Launcher::Launcher(){
    #ifdef UNIX
    desc_fich = open (RUTA_TRAZA_PROC, O_CREAT|O_APPEND|O_WRONLY, 0);
    dup2 (desc_fich, 1);		/* Redirige la salida normal */
    dup2 (desc_fich, 2);		/* Redirige la salida de error */
    #endif
}

/**
*
*/
Launcher::~Launcher(){
    log("Launcher::~Launcher");
    #ifdef UNIX
    log( "~Launcher(): Cerrando fichero de trazas");
    close (desc_fich);
    #endif
}

/**
* Descomprime el fichero .zip pasado por parametro y devuelve el nombre del fichero extraido
*/
string Launcher::descomprimirZIP(string filename){
    string filenameReturn = filename;
    log("descomprimirZIP");
    if (filename.find(".zip") != string::npos || filename.find(".ZIP") != string::npos){
        //Descomprimir el fichero
        UnzipTool *unzipTool = new UnzipTool();
        unzippedFileInfo salida = unzipTool->descomprimirZip(filename.c_str());
        delete unzipTool;
        filenameReturn = salida.rutaEscritura;
    }
    log("descomprimirZIP FIN");
    return filenameReturn;
}

/**
* Realiza un lanzamiento del programa que se encontro al parsearArgumentos
*/
bool Launcher::lanzarProgramaUNIXFork(FileLaunch *emulInfo){

    string ejecutable = emulInfo->rutaexe + (emulInfo->rutaexe.empty()==true ? "":Constant::TipoToStr(FILE_SEPARATOR)) + emulInfo->fileexe;
    string param = emulInfo->parmsexe;
    string rom = emulInfo->rutaroms + (emulInfo->rutaroms.empty()==true ? "":Constant::TipoToStr(FILE_SEPARATOR)) + emulInfo->nombrerom;

    #ifdef UNIX

        int extPos = rom.rfind(".");
        bool isZipped = false;
        if (extPos > 0){
            string extension = rom.substr(extPos);
            Constant::lowerCase(&extension);
            log( "extension: " + extension);
            if (extension.compare(".zip") == 0){
                isZipped = true;
            }
        }

        log( "*************Launcher::lanzarProgramaUNIX*************");
        string buffer = "";
        if (emulInfo->descomprimir){
            buffer = descomprimirZIP(rom);
        } else {
            buffer = rom;
        }
        string comando;

        if (ejecutable.find("%ROM%") != string::npos){
            log( "Antes de reemplazar: " + buffer);
            buffer = Constant::replaceMetaCaracters(buffer);
            log( "Despues de reemplazar: " + buffer);
            //Este comando es generado cuando se importan los emuladores desde retroarch
            comando = Constant::replaceAll(ejecutable, "%ROM%", buffer );
        } else {
            //Comando que se ejecuta al anyadir emuladores manualmente
            comando = "clear; " + ejecutable + " " + param + " " + "\"" + buffer +"\"" ;
        }


        log( "Vamos a lanzar el comando: " + comando);
        int retorno = system(comando.c_str());
        if(retorno != 0){
            log( "Error al lanzar el comando");
            return false;
        } else {
            if (emulInfo->descomprimir && isZipped){
                log( "Borrando el fichero: " + buffer);
                deleteUnzipedRom(buffer);
            }
            return true;
        }
    #elif WIN
        log( "*************Launcher::lanzarProgramaUNIX - En Windows*************");
        string buffer = "";

        int extPos = rom.rfind(".");
        bool isZipped = false;
        if (extPos > 0){
            string extension = rom.substr(extPos);
            Constant::lowerCase(&extension);
            log( "extension: " + extension);
            if (extension.compare(".zip") == 0){
                isZipped = true;
            }
        }

        if (emulInfo->descomprimir){
            buffer = descomprimirZIP(rom);
        } else {
            buffer = rom;
        }

//        string filerompath;
//        if (buffer.empty())
//            filerompath = "";
//        else
//            //filerompath = "\"" + buffer + "\"";
//            filerompath = buffer;

        Traza::print("Launcher::launch. Lanzando ejecutable: " + ejecutable, W_DEBUG);
        Traza::print("Launcher::launch. Lanzando parmsexe: " + param, W_DEBUG);
        Traza::print("Launcher::launch. Lanzando rom: " + buffer, W_DEBUG);

        string comando = rutaEspecial(ejecutable, param, buffer);

        /*
        P_OVERLAY 	Overlays parent process with child, which destroys the parent. This has the same effect as the exec* functions.
        P_WAIT 	Suspends parent process until the child process has finished executing (synchronous spawn).
        P_NOWAIT, P_NOWAITO 	Continues to execute calling process concurrently with new process (asynchronous spawn).
        P_DETACH 	the child is run in background without access to the console or keyboard. Calls to _cwait upon the new process will fail (asynchronous spawn)
        */
        //const char* argv[] = { ejecutable.c_str(), param.c_str(), filerompath.c_str(), 0 };
        //int pid = spawnv(P_WAIT, argv[0], argv);

//        string separador = " ";
//        if (!emulInfo->separaParmRuta){
//            separador = "";
//        }
//
//        string comando = ejecutable + " " + param + separador + filerompath ;
        Traza::print("Launcher::launch. comando: " + comando, W_DEBUG);
        Dirutil dir;
        dir.changeDirAbsolute(emulInfo->rutaexe.c_str());

        log( "Vamos a lanzar el comando: " + comando);
        int retorno = system(comando.c_str());
        if(retorno != 0){
            log( "Error al lanzar el comando");
            return false;
        } else {
            if (emulInfo->descomprimir && isZipped){
                log( "Borrando el fichero: " + buffer);
                deleteUnzipedRom(buffer);
            }
            return true;
        }

    #endif
    // executed only by parent
    log("lanzarProgramaUNIX Fin");
    return true;
}


//bool Launcher::lanzarProgramaUNIXFork(FileLaunch *emulInfo){
//
//    string ejecutable = emulInfo->rutaexe + FILE_SEPARATOR + emulInfo->fileexe;
//    string param = emulInfo->parmsexe;
//    string rom = emulInfo->rutaroms + FILE_SEPARATOR + emulInfo->nombrerom;
//
//    #ifdef UNIX
//        log( "*************Launcher::lanzarProgramaUNIX*************");
//        /* Buffer para escribir o leer caracteres de la tuberia */
//        string buffer = "";
//
//        if (emulInfo->descomprimir){
//            buffer = descomprimirZIP(rom);
//        } else {
//            buffer = rom;
//        }
//
//        char *cEjecutable = new char(ejecutable.length()+1);
//        char *cParam = new char(param.length()+1);
//        char *cRom = new char(buffer.length()+1);
//        strcpy(cEjecutable,ejecutable.c_str());
//        strcpy(cParam,param.c_str());
//        strcpy(cRom, buffer.c_str());
//
//
//        if (ejecutable.compare("") != 0){
//            int child_status;
//
//            /* Se crea el proceso hijo. En algún sitio dentro del fork(), nuestro
//             * programa se duplica en dos procesos. Cada proceso obtendrá una salida
//             * distinta. */
//            pid_t pID = vfork(); //vfork - create a child process and block parent
//
//            /* fork() devuelve 0 al proceso hijo.*/
//            if (pID == 0){
//                // Code only executed by child process
//                char* argv[] = { cEjecutable, cParam, cRom, NULL };
//                int execReturn = execv(argv[0], argv);
//
//                //Aqui no llegamos a no ser que vaya algo mal en la ejecucion
//                if(execReturn == -1){
//                    log(RUTA_TRAZA_HIJO, "ERROR al ejecutar execv: " + getErrMsg(errno));
//                     _exit(-1);
//                }
//                _exit(0); //_exit - - terminate the current process
//            }
//            /* Si fork() devuelve -1, es que hay un error y no se ha podido crear el
//             * proceso hijo. */
//            else if (pID < 0){            // failed to fork
//                cerr << "Failed to fork" << endl;
//                log( "lanzarProgramaUNIX Failed to fork");
//                exit(1);
//                 // Throw exception
//            }
//            /* fork() devuelve un número positivo al padre. Este número es el id del
//             * hijo. */
//            else {   // parent
//
//                /* This is run by the parent.  Wait for the child
//                to terminate. */
//                log("Padre: Esperando a la muerte del hijo");
//                waitpid(pID, &child_status, 0);
//
//                /* Comprueba la salida del hijo */
//                if (WIFEXITED(child_status) != 0){
//                    log("Padre: Proceso Hijo terminado. Recibido: " + buffer);
////                    deleteUnzipedRom(buffer);
//                } else {
//                    log("Padre: ERROR EN LA SALIDA DEL HIJO");
//                }
//            }
//        } else {
//             log( "No se ha encontrado ningun programa que lanzar");
//        }
//
//    #elif WIN
//        log( "*************Launcher::lanzarProgramaUNIX - En Windows*************");
//        string buffer = "";
//
//        if (emulInfo->descomprimir){
//            buffer = descomprimirZIP(rom);
//        } else {
//            buffer = rom;
//        }
//        string filerompath = "\"" + buffer + "\"";
//
//        Traza::print("Launcher::launch. Lanzando ejecutable: " + ejecutable, W_DEBUG);
//        Traza::print("Launcher::launch. Lanzando parmsexe: " + param, W_DEBUG);
//        Traza::print("Launcher::launch. Lanzando rom: " + filerompath, W_DEBUG);
//
//        /*
//        P_OVERLAY 	Overlays parent process with child, which destroys the parent. This has the same effect as the exec* functions.
//        P_WAIT 	Suspends parent process until the child process has finished executing (synchronous spawn).
//        P_NOWAIT, P_NOWAITO 	Continues to execute calling process concurrently with new process (asynchronous spawn).
//        P_DETACH 	the child is run in background without access to the console or keyboard. Calls to _cwait upon the new process will fail (asynchronous spawn)
//        */
//        const char* argv[] = { ejecutable.c_str(), param.c_str(), filerompath.c_str(), 0 };
//        int pid = spawnv(P_WAIT, argv[0], argv);
//        if (pid == -1) Traza::print("Launcher::launch. error" + string(strerror(errno)), W_DEBUG);
//    #endif
//    // executed only by parent
//    log("lanzarProgramaUNIX Fin");
//    return true;
//}

string Launcher::rutaEspecial(string ejecutable, string param, string filerompath){
    string comandoFinal = "";
//    string separador = " ";
//    if (!separaParmRuta){
//        separador = "";
//    }

    if (param.find("%") != std::string::npos){
        Dirutil dir;
        bool filenameinparms = false;

        if (param.find("%ROMNAME%") !=std::string::npos || param.find("%ROMNAMEXT%") !=std::string::npos
            || param.find("%ROMFULLPATH%") !=std::string::npos){
            filenameinparms = true;
        }

//        param = Constant::replaceAll(param, "%ROMPATH%", "\"" + dir.getFolder(filerompath) + "\"");
//        param = Constant::replaceAll(param, "%ROMNAME%", "\"" + dir.getFileNameNoExt(filerompath) + "\"");
//        param = Constant::replaceAll(param, "%ROMNAMEXT%", "\"" + dir.getFileName(filerompath) + "\"");
//        param = Constant::replaceAll(param, "%ROMFULLPATH%", "\"" + filerompath + "\"");

        param = Constant::replaceAll(param, "%ROMPATH%", dir.getFolder(filerompath));
        param = Constant::replaceAll(param, "%ROMNAME%", dir.getFileNameNoExt(filerompath));
        param = Constant::replaceAll(param, "%ROMNAMEXT%", dir.getFileName(filerompath));
        param = Constant::replaceAll(param, "%ROMFULLPATH%", filerompath);

        if (filenameinparms){
            comandoFinal = ejecutable + " " + param;
        } else {
            comandoFinal = ejecutable + " " + param + " " + filerompath;
        }


    } else {
        if (filerompath.empty())
            filerompath = "";
        else
            filerompath = "\"" + filerompath + "\"";

        comandoFinal = ejecutable + " " + param + " " + filerompath ;
    }
    Traza::print("Launcher::launch. comandoFinal: " + comandoFinal, W_DEBUG);
    return comandoFinal;
}

/**
*
*/
void Launcher::log(string msg){
    cout << Constant::fecha() << string(" - ") << msg << endl;
}

/**
*
*/
void Launcher::log(string ruta, string msg){
    ofstream myfile (ruta.c_str(), fstream::out | fstream::app);
    if (myfile.is_open()){
        myfile << Constant::fecha() << string(" - ") << msg << VBCRLF;
        myfile.close();
    }
}

/**
*
*/
void Launcher::deleteUnzipedRom(string romfile){
    if (romfile.compare("") != 0 && romfile.find(".zip") == string::npos && romfile.find(".ZIP") == string::npos){
        Dirutil *dirutil=new Dirutil();
        //dirutil->borrar(romfile);
        if (dirutil->borrarArchivo(romfile))
            log( "deleteUnzipedRom Fichero eliminado: " + romfile);
        else
            log( "No se ha encontrado el fichero a eliminar: " + romfile);

        delete dirutil;
    }
}

/**
*
*/
void Launcher::writeToFile(string ruta, string msg){
    FILE *fout = fopen(ruta.c_str(),"at+");
    msg = msg + VBCRLF;

    fwrite(msg.c_str(), sizeof(char) , msg.length(), fout);

    if (fout != NULL){
        fclose(fout);
        fout = NULL;
    }
}

/**
*
*/
std::string Launcher::getErrMsg(int errnum){

        switch ( errnum ) {

    #ifdef EACCES
            case EACCES :
            {
                return "EACCES Permission denied";
            }
    #endif

    #ifdef EPERM
            case EPERM :
            {
                return "EPERM Not super-user";
            }
    #endif

    #ifdef E2BIG
            case E2BIG :
            {
                return "E2BIG Arg list too long";
            }
    #endif

    #ifdef ENOEXEC
            case ENOEXEC :
            {
                return "ENOEXEC Exec format error";
            }
    #endif

    #ifdef EFAULT
            case EFAULT :
            {
                return "EFAULT Bad address";
            }
    #endif

    #ifdef ENAMETOOLONG
            case ENAMETOOLONG :
            {
                return "ENAMETOOLONG path name is too long     ";
            }
    #endif

    #ifdef ENOENT
            case ENOENT :
            {
                return "ENOENT No such file or directory";
            }
    #endif

    #ifdef ENOMEM
            case ENOMEM :
            {
                return "ENOMEM Not enough core";
            }
    #endif

    #ifdef ENOTDIR
            case ENOTDIR :
            {
                return "ENOTDIR Not a directory";
            }
    #endif

    #ifdef ELOOP
            case ELOOP :
            {
                return "ELOOP Too many symbolic links";
            }
    #endif

    #ifdef ETXTBSY
            case ETXTBSY :
            {
                return "ETXTBSY Text file busy";
            }
    #endif

    #ifdef EIO
            case EIO :
            {
                return "EIO I/O error";
            }
    #endif

    #ifdef ENFILE
            case ENFILE :
            {
                return "ENFILE Too many open files in system";
            }
    #endif

    #ifdef EINVAL
            case EINVAL :
            {
                return "EINVAL Invalid argument";
            }
    #endif

    #ifdef EISDIR
            case EISDIR :
            {
                return "EISDIR Is a directory";
            }
    #endif

    #ifdef ELIBBAD
            case ELIBBAD :
            {
                return "ELIBBAD Accessing a corrupted shared lib";
            }
    #endif

            default :
            {
                std::string errorMsg(strerror(errnum));
                if ( errnum ) return errorMsg;
                else return "";
            }
         }
}
