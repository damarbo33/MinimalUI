#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include "uiobjects/fileprops.h"
#include "uiobjects/Traza.h"
#include "uiobjects/listaIni.h"


class Rominfo{
    public:
        Rominfo();
        ~Rominfo();

        ListaIni<DataList> *listaControls;
        ListaIni<Data> *nplayers;
        ListaIni<Data> *infoRom;
        vector <FileProps> * vectorFileProps;
        int emuladorID;
        string filtroExtension;
        void loadFromIni(string);

        int romCounter;
        int rutasCounter;
};
#endif // ESTRUCTURAS_H
