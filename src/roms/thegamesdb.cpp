#include "thegamesdb.h"

TheGamesDB::TheGamesDB(){
    gameQueryUrl = "http://thegamesdb.net/api/GetGame.php";
}

TheGamesDB::~TheGamesDB(){
}

/**
* Wrapper para bajar informacion de juegos
*/
int TheGamesDB::download(RequestQuery* queryGame, ResponseQuery *response){
    HttpUtil util;
    util.setTimeout(5); //Un timeout de 5 segundos para cualquier query
    string url = gameQueryUrl + makeParms(queryGame);
    Traza::print("url: " + url, W_DEBUG);
    if (util.download(url)){
        Traza::print("DOWNLOADED!!!", W_DEBUG);
        GumboOutput* output = gumbo_parse(util.getData());
        getGameInfo(output->root, response);
        gumbo_destroy_output(&kGumboDefaultOptions, output);
        return QUERYOK;
        if (response->error.empty())
            return QUERYOK;
        else
            return GAMENOTFOUND;
    } else {
        return CONNECTERROR;
    }
}

/**
*
*/
int TheGamesDB::download2(RequestQuery* queryGame, ResponseQuery *response){
    HttpUtil util;
    util.setTimeout(5); //Un timeout de 5 segundos para cualquier query

    string url = gameQueryUrl + makeParms(queryGame);
    Traza::print("url: " + url, W_DEBUG);

    if (util.download(url)){
        TiXmlDocument doc;
        if (util.getDataLength() == 0){
            Traza::print("No se ha obtenido respuesta del servidor",W_ERROR);
            return PARSEERROR;
        } else if (doc.Parse((const char*)util.getData(), 0, TIXML_ENCODING_UTF8) != NULL){
            loadXmlGame( &doc , response);
            if (response->error.empty())
                return QUERYOK;
            else
                return GAMENOTFOUND;
        } else{
            Traza::print("Ha ocurrido un error al parsear la respuesta",W_DEBUG);
            return PARSEERROR;
        }
    } else {
        return CONNECTERROR;
    }
}



/**
*
*/
std::string TheGamesDB::getGameInfo(GumboNode* node, ResponseQuery *response) {

    if (node->type == GUMBO_NODE_TEXT) {
//        cout << node->v.text.text << endl;
        return std::string(node->v.text.text);
    } else if (node->type == GUMBO_NODE_ELEMENT) {
        string nombreTag = "";

        if (node->v.element.tag == GUMBO_TAG_UNKNOWN){
            nombreTag = getTagName(node->v.element.original_tag.data);
//            cout << nombreTag << ": ";
        }

        static bool similar = false;

        GumboVector* children = &node->v.element.children;
        for (unsigned int i = 0; i < children->length; ++i) {
            const std::string text = getGameInfo((GumboNode*) children->data[i], response);
            if (nombreTag.compare("GameTitle") == 0){
                response->gameList.back()->gameTitle = text;
            } else if (nombreTag.compare("baseImgUrl") == 0){
                response->baseImgUrl = text;
                //response->gameList.back()->baseImgUrl = text;
            } else if (!similar && nombreTag.compare("id") == 0){
                //Este es el primer campo que se obtiene, por lo que creamos el
                //objeto que almacena los datos del juego
                ResponseGame *newGame = new ResponseGame();
                newGame->id = text;
                response->gameList.push_back(newGame);
            } else if (!similar && nombreTag.compare("PlatformId") == 0){
                response->gameList.back()->platformId = text;
            } else if (nombreTag.compare("Platform") == 0){
                response->gameList.back()->platform = text;
            } else if (nombreTag.compare("ReleaseDate") == 0){
                response->gameList.back()->releaseDate = text;
            } else if (nombreTag.compare("Overview") == 0){
                response->gameList.back()->overview = text;
            } else if (nombreTag.compare("Players") == 0){
                response->gameList.back()->players = text;
            } else if (nombreTag.compare("Publisher") == 0){
                response->gameList.back()->publisher = text;
            } else if (nombreTag.compare("Developer") == 0){
                response->gameList.back()->developer = text;
            } else if (nombreTag.compare("Rating") == 0){
                response->gameList.back()->rating = text;
            } else if (nombreTag.compare("Similar") == 0){
                similar = true;
            } else if (nombreTag.compare("genre") == 0){
                response->gameList.back()->genres.push_back(text);
            } else if (nombreTag.compare("thumb") == 0){
                response->gameList.back()->fanart.push_back(text);
                similar = false;
            } else if (nombreTag.find("boxart") != string::npos){
                response->gameList.back()->boxart.push_back(text);
                similar = false;
            } else if (nombreTag.compare("Error") == 0){
                response->error = text;
            }
        }
    }
    return "";
}


/**
*
*/
std::string TheGamesDB::getTagName(const char* data) {

    int len = strlen(data);
    int i=0;
//    int ini = 0;
    int fin = 0;
    string tagName = "";
    char tmp[2] = {' ','\0'};

    while(i < len && fin == 0){
//        if (data[i] == '<')
//            ini = i;
//        else
        if (data[i] == '>')
            fin = i;
        else {
            tmp[0] = data[i];
            tagName.append(tmp);
        }
        i++;
    }
    return tagName;
}

/**
*
*/
string TheGamesDB::makeParms(RequestQuery* queryGame){
    string url = "";
    addParm(&url, "id", Constant::url_encode(queryGame->id));
    addParm(&url, "platform", Constant::url_encode(queryGame->platform));
    addParm(&url, "name", Constant::url_encode(queryGame->name));
    addParm(&url, "exactname", Constant::url_encode(queryGame->exactName));
    return url;
}

/**
*
*/
void TheGamesDB::addParm(string *url, string name, string value){
    if (!value.empty()){
        if (url->find("?") == string::npos)
            url->append("?");
        else
            url->append("&");

        url->append(name);
        url->append("=");
        url->append(value);
    }
}

/**
*
*/
void TheGamesDB::loadXmlGame( TiXmlNode* pParent, ResponseQuery *response)
{
	if ( !pParent ) return;

    TiXmlElement* root = pParent->FirstChildElement();
    if(root == NULL){
		cerr << "Failed to load file: No root element." << endl;
		return;
	}

    TiXmlText* text;
    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()){
		string elemName = elem->Value();
        if (elemName.compare("baseImgUrl") == 0){
            text = elem->FirstChild()->ToText();
            response->baseImgUrl = text->Value();
        } else if (elemName.compare("Game") == 0){
            ResponseGame *newGame = new ResponseGame();
            response->gameList.push_back(newGame);

            //Empezamos a procesar la informacion del juego
            for(TiXmlElement* elemChild = elem->FirstChildElement(); elemChild != NULL; elemChild = elemChild->NextSiblingElement()){
                elemName = elemChild->Value();
                if (elemName.compare("id") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->id = text->Value();
                } else if (elemName.compare("GameTitle") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->gameTitle = text->Value();
                } else if (elemName.compare("PlatformId") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->platformId = text->Value();
                } else if (elemName.compare("Platform") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->platform = text->Value();
                } else if (elemName.compare("ReleaseDate") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->releaseDate = text->Value();
                } else if (elemName.compare("Overview") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->overview = text->Value();
                } else if (elemName.compare("Players") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->players = text->Value();
                } else if (elemName.compare("Publisher") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->publisher = text->Value();
                } else if (elemName.compare("Developer") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->developer = text->Value();
                } else if (elemName.compare("Rating") == 0){
                    text = elemChild->FirstChild()->ToText();
                    response->gameList.back()->rating = text->Value();
                } else if (elemName.compare("Genres") == 0){
                    for(TiXmlElement* imagesChild = elemChild->FirstChildElement("genre"); imagesChild != NULL; imagesChild = imagesChild->NextSiblingElement()){
                        text = imagesChild->FirstChild()->ToText();
                        response->gameList.back()->genres.push_back(text->Value());
                    }
                } else if (elemName.compare("Images") == 0){
                    for(TiXmlElement* imagesChild = elemChild->FirstChildElement("boxart"); imagesChild != NULL; imagesChild = imagesChild->NextSiblingElement()){
                        text = imagesChild->FirstChild()->ToText();
                        const char* attr = imagesChild->Attribute("side");
                        if(attr != NULL && strcmp(attr,"front") == 0){
                            response->gameList.back()->boxart.push_back(text->Value());
                        }
                    }
                }
            }
        }

	}
}
