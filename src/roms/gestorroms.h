#ifndef GESTORROMS_H
#define GESTORROMS_H

#include "../Database.h"
#include "ListaSimple.h"
#include "Dirutil.h"
#include "Menuobject.h"
#include "estructuras.h"
#include "fileprops.h"
#include "EmuProperties.h"
#include "uilist.h"


class Gestorroms
{
    public:
        Gestorroms(string);
        virtual ~Gestorroms();
        void loadDBFromFile(string);
        void closeDB();
        int fillMenuByQuery(UIList *, string, vector<string> *statementValues, int);
        string getColQuery(const char*);
        vector<vector<string> > getRowQuery(string);
        bool executeQuery(string);
        DWORD actualizarRoms();

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

    protected:
    private:
        Database *db;
        unsigned int addRom(unsigned  int , unsigned int, string);
        void cargarInfoRoms(Rominfo *);
        DWORD actualizarRoms(int);
        string rutaInicial;
};

#endif // GESTORROMS_H
