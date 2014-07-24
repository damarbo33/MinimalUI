#include "estructuras.h"

Rominfo::Rominfo(){
    listaControls = new ListaIni<DataList>();
    nplayers = new ListaIni<Data>();
    infoRom = new ListaIni<Data>();
    vectorFileProps = new vector <FileProps>();
}

Rominfo::~Rominfo(){
    if (listaControls != NULL) {
        Traza::print("eliminando listaControls",listaControls->getSize(), W_DEBUG);
        delete listaControls;
        Traza::print("eliminando listaControls OK", W_DEBUG);
    }
    if (nplayers != NULL){
        Traza::print("eliminando nplayers", W_DEBUG);
        delete nplayers;
        Traza::print("eliminando nplayers OK", W_DEBUG);
    }
    if (infoRom != NULL){
        Traza::print("eliminando infoRom", W_DEBUG);
        delete infoRom;
        Traza::print("eliminando infoRom OK", W_DEBUG);
    }

    if (vectorFileProps != NULL){
        Traza::print("eliminando vectorFileProps", W_DEBUG);
        delete vectorFileProps;
        Traza::print("eliminando vectorFileProps OK", W_DEBUG);
    }


}

/**
*
*/
void Rominfo::loadFromIni(string rutaini){
    listaSimple<string> *controls = new listaSimple<string>();
    try{
        controls->loadStringsFromFile(rutaini);

        unsigned int i = 0;
        string str;

        while (i < controls->getSize()){
            str = controls->get(i);
            if (controls->get(i)[0] == '['){
                DataList listatemp;
                listatemp.initValores();
                listatemp.setKey(controls->get(i));
                i++;
                str = controls->get(i);
                while (i < controls->getSize() && controls->get(i)[0] != '['){
                    listatemp.addValue(controls->get(i));
                    i++;
                }
                i--;
                listaControls->add(listatemp);
            }
            i++;
        }
        listaControls->sort();
    } catch (Excepcion &e){
        Traza::print("Excepcion loadFromIni: " + string(e.getMessage()), W_ERROR);
    }

    delete controls;

}
