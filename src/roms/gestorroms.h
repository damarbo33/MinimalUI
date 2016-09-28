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

class Gestorroms
{
    public:
        Gestorroms(string);
        virtual ~Gestorroms();
        void loadDBFromFile(string);
        void closeDB();
        int fillMenuByQuery(Object *, string, vector<string> *statementValues, int);
        string getColQuery(const char*);
        vector<vector<string> > getRowQuery(string);
        bool executeQuery(string);
        DWORD actualizarRoms();
        DWORD actualizarRoms(int);
        bool insertEmulador(Emuinfo::struct_emu *);
        bool updateEmulador(Emuinfo::struct_emu *);
        bool deleteEmulador(int);
        vector<vector<string> > getDatosEmulador(int);


        void listarDirRecursivo(string dir,  Rominfo *);
        unsigned int listarDirSinOrden(const char *strdir, vector <FileProps> * tempFilelist, Rominfo *);
        string parserSQLWhere(string);
        DWORD importRetroarchConfig();
        void setRutaInicial(string var){rutaInicial = var;}
        string getRutaInicial(){return rutaInicial;}
        bool comprobarUnidad(string diractual, string *unidadRoms);
        void updateRutas(string unidadRoms, string unidadActual);
        string getImgEmulador(int idEmu);
        Database * getDb(){return db;}
        string getParameter(string parameter);
        bool updateParameter(string parameter, string value);
        //para threads
        DWORD thScrapSystem();
        DWORD thScrapAllSystem();
        void setThEmuID(string var){thEmuID = var;}
        string getThEmuID(){return thEmuID;}
        bool isScrappingNow(){return scrappingNow;}
        string getPlatform(){return platform;}
        void setPlatform(string var){platform = var;}
        string getProgress(){return progress;}

        void updateRom(vector<vector<string> > *listaRoms);
        DWORD updateRom();
        vector<vector<string> > * getThListaRoms(){return &thListaRoms;}

    protected:
    private:
        Database *db;
        unsigned int addRom(unsigned  int , unsigned int, string);
        void cargarInfoRoms(Rominfo *);
//        DWORD addRomInfo(vector<vector<string> > *listaRoms, HANDLE *myhandle, RomWebInfo **objRom);
        DWORD addRomInfo(vector<vector<string> > *listaRoms, int posRomProcess, RomWebInfo *objRom);
        DWORD scrapsystem(string idEmu);
        string rutaInicial;
        string thEmuID;
        string platform;
        string progress;
        bool scrappingNow;
        void restaurarRomInfo(int idEmu);


        void comprobarVersion();
        void createRomInfo(Rominfo *rominfo, bool isFindInfoRoms, string filename);
        MameHistoryParser mame;

        vector<vector<string> > thListaRoms;

};

#endif // GESTORROMS_H
