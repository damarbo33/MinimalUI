#ifndef GESTORROMS_H
#define GESTORROMS_H

#include "bbdd/Database.h"
#include "ListaSimple.h"
#include "Dirutil.h"
#include "Menuobject.h"
#include "estructuras.h"
#include "fileprops.h"
#include "EmuProperties.h"
#include "uilist.h"
#include "roms/romwebinfo.h"
#include "roms/thegamesdb.h"
#include "roms/bdrominfoout.h"
#include "roms/mamehistoryparser.h"
#include "thread.h"

class Gestorroms
{
    public:
        Gestorroms(string);
        virtual ~Gestorroms();
        void loadDBFromFile(string);
        void closeDB();
        int fillMenuByQuery(Object *, string, vector<string> *statementValues, string);
        string getColQuery(const char*);
        vector<vector<string> > getRowQuery(string);
        bool executeQuery(string);
        uint32_t actualizarRoms();
        uint32_t actualizarRoms(int);
        bool insertEmulador(Emuinfo::struct_emu *);
        bool updateEmulador(Emuinfo::struct_emu *);
        bool deleteEmulador(int);
        vector<vector<string> > getDatosEmulador(int);


        void listarDirRecursivo(string dir,  Rominfo *);
        unsigned int listarDirSinOrden(const char *strdir, vector <FileProps> * tempFilelist, Rominfo *);
        string parserSQLWhere(string, string);
        uint32_t importRetroarchConfig();
        void setRutaInicial(string var){rutaInicial = var;}
        string getRutaInicial(){return rutaInicial;}
        bool comprobarUnidad(string diractual, string *unidadRoms);
        void updateRutas(string unidadRoms, string unidadActual);
        string getImgEmulador(int idEmu);
        Database * getDb(){return db;}
        string getParameter(string parameter);
        bool updateParameter(string parameter, string value);
        //para threads
        uint32_t thScrapSystem();
        uint32_t thScrapAllSystem();
        void setThEmuID(string var){thEmuID = var;}
        string getThEmuID(){return thEmuID;}
        bool isScrappingNow(){return scrappingNow;}
        string getPlatform(){return platform;}
        void setPlatform(string var){platform = var;}
        string getProgress(){return progress;}

        void updateRom(vector<vector<string> > *listaRoms);
        uint32_t updateRom();
        vector<vector<string> > * getThListaRoms(){return &thListaRoms;}
        uint32_t scrapsystemMulti(string idEmu, int inicio, int fin);
        uint32_t thScrapSystemMulti();
        void setThScrapIni(int var){thScrapIni = var;}
        void setThScrapFin(int var){thScrapFin = var;}
        int getThScrapIni(){return thScrapIni;}
        int getThScrapFin(){return thScrapFin;}
        int getRomsNotScrapped(string idEmu);
        void setThScrapTotal(int var){thScrapTotal = var;}
        static SDL_mutex *mutex;
        vector<vector<string> > getAllEmus();
        void refreshArtWorkOptim(string codEmu, string dirInicial);
        uint32_t thRefreshArtWorkOptim();
        uint32_t thRefreshAllArtWorkOptim();
        void setThDirInicial(string var){thDirInicial = var;}

    protected:
    private:
        Database *db;
        unsigned int addRom(unsigned  int , unsigned int, string);
        void cargarInfoRoms(Rominfo *);
//        uint32_t addRomInfo(vector<vector<string> > *listaRoms, HANDLE *myhandle, RomWebInfo **objRom);
        uint32_t addRomInfo(vector<vector<string> > *listaRoms, int posRomProcess, RomWebInfo *objRom);
        uint32_t scrapsystem(string idEmu);
        string rutaInicial;
        string thEmuID;



        void restaurarRomInfo(int idEmu);


        void comprobarVersion();
        void createRomInfo(Rominfo *rominfo, bool isFindInfoRoms, string filename);
        MameHistoryParser mame;

        vector<vector<string> > thListaRoms;
        int thScrapIni;
        int thScrapFin;
        string thDirInicial;

        static string platform;
        static string progress;
        static int thScrapTotal;
        static int thScrapCount;
        static bool scrappingNow;


};

#endif // GESTORROMS_H
