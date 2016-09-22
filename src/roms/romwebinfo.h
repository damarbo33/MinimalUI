#ifndef ROMWEBINFO_H
#define ROMWEBINFO_H

#include "httputil.h"
#include "Constant.h"
#include "thegamesdb.h"
#include "Dirutil.h"
#include "image/uiimgdownloader.h"

class RomWebInfo
{
    public:
        /** Default constructor */
        RomWebInfo(int thPosRom, vector<vector<string> > *thListaRoms, string thPlatform);
        /** Default destructor */
        virtual ~RomWebInfo();
        /** Obtiene la informacion llamando al servicio http*/
        void updateGameInfo();
        /**Obtiene la respuesta parseada*/
        ResponseGame * getResponse(){return resp;}


    protected:
    private:
        //Parser que nos permite obtener la informacion de la web thegamesdb
        TheGamesDB gamesDB;
        //Listado de roms en las que hay que buscar informacion
        vector<vector<string> > *thListaRoms;
        //Plataforma en la que buscar informacion
        string thPlatform;
        //Posicion de la rom en la lista de la que intentamos recuperar información
        int thPosRom;
        //Respuesta obtenida de la web ya parseada
        ResponseGame *resp;
        //Obtiene el titulos mas parecido al que estamos buscando si hay varias opciones
        int findSimilarTitle(vector<ResponseGame *> *listaJuegos, string tituloBuscado);
        //Busca la imagen que representa a la caja del juego
        string findBoxartFront(ResponseQuery *response, string idprog);
        //Objeto para realizar llamadas http
        HttpUtil utilHttp;
        //Realiza un substring hasta el lugar definido por la ocurrencia de keyString
        void cutFileName(string *fileromname, string keyString);

};

#endif // ROMWEBINFO_H
