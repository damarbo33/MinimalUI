#ifndef THEGAMESDB_H
#define THEGAMESDB_H

#include "httputil.h"
#include "Constant.h"
#include <gumbo.h>
#include "traza.h"
#define TIXML_USE_STL
#include "tinyxml/tinyxml.h"

typedef enum{
    QUERYOK,
    GAMENOTFOUND,
    CONNECTERROR,
    PARSEERROR
}errores;

class RequestQuery{
    public :
        string id;
        string platform;
        string name;
        string exactName;
};

class ResponseGame{
    public:
        string id;
        string gameTitle;
        string platformId;
        string platform;
        string releaseDate;
        string overview;
        vector <string> genres;
        string players;
        string publisher;
        string developer;
        string baseImgUrl;
        string rating;
        vector <string> boxart;
        vector <string> fanart;
        string boxartFront;
};

class ResponseQuery{
    public :
    ResponseQuery(){

    }
    //Vector con la lista de juegos obtenidos
    vector<ResponseGame *> gameList;
    //Tipo de error obtenido en la consulta
    string error;
    string baseImgUrl;
};



class TheGamesDB{
    public:
        /** Default constructor */
        TheGamesDB();
        /** Default destructor */
        virtual ~TheGamesDB();

        int download(RequestQuery* queryGame, ResponseQuery *response);
        int download2(RequestQuery* queryGame, ResponseQuery *response);

    protected:
        string gameQueryUrl;
        std::string getGameInfo(GumboNode* node, ResponseQuery *response);
        std::string getTagName(const char* data);
    private:
        void addParm(string *url, string name, string value);
        string makeParms(RequestQuery* queryGame);
        void loadXmlGame( TiXmlNode* pParent, ResponseQuery *response);
};

#endif // THEGAMESDB_H
