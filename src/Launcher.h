#ifndef LAUNCHER_H
#define LAUNCHER_H

#define IDPROGRAMALANZAR "-idprog"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#ifdef UNIX
    #include <stdio.h>
    #include <sys/wait.h>
#endif
#include <stdlib.h>
#include <vector>
#include "unzip/unziptool.h"
#include "Dirutil.h"
#include "filelaunch.h"

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

/*Redirección usando dup2 */
#include <fcntl.h>




using namespace std;

class Log
{

    public:
        Log(){};
        virtual ~Log(){};
        void print(string);
    protected:
    private:
};

class Launcher
{

    public:

        Launcher();
        virtual ~Launcher();
        bool parsearArgumentos(int, char **);
        bool lanzarProgramaUNIXFork(FileLaunch *);
    protected:
        string descomprimirZIP(string);
        std::string getErrMsg(int errnum);
        void deleteUnzipedRom(string);
        void writeToFile(string, string);
        void log(string);
        string crearArrayParametros(bool);
        void log(string, string);
        string rutaEspecial(string ejecutable, string param, string filerompath);
    private:
        int desc_fich;
};






// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

#endif // LAUNCHER_H
