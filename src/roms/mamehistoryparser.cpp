#include "mamehistoryparser.h"

/**
*
*/
MameHistoryParser::MameHistoryParser(){
}

/**
*
*/
MameHistoryParser::~MameHistoryParser(){
}

/**
*
*/
string MameHistoryParser::getRomSection(string rom, string section){
  try {
    return mapHistoryRoms.at(rom).at(section);
  } catch (const std::out_of_range& oor) {
    return "";
  }
}

map<string, string> MameHistoryParser::getRomData(string rom){
  map<string, string> data;

  try {
    data = mapHistoryRoms.at(rom);
  } catch (const std::out_of_range& oor) {
    return data;
  }
  return data;
}

/**
*
*/
void MameHistoryParser::loadHistory(string ruta){
    int foundRoms = 0;
    string line;
    vector<string> splittedLine, splittedNameRom;
    string descRom;
    string nameRom;
    string titleRom;
    string year;
    string developer;
    bool continueReading = true;
    mapHistoryRoms.clear();
    size_t posTitle = 0;

    ifstream myfile (ruta.c_str());
    if (myfile.is_open()){
        while ( getline (myfile, line) ){
            if (line.find("$info") == 0){
                splittedLine = Constant::split(line, "=");
                //Si hemos encontrado un fichero, comprobamos que tenga descripcion, que debe estar en la
                //linea 5 despues de la $info
                titleRom = "";
                year = "";
                developer = "";
                for (int i=0; i < 5 && continueReading; i++){
                    continueReading = static_cast<bool>(getline (myfile, line));
                    //Cargamos el titulo, el anyo y el desarrollador del juego
                    if (!line.empty()){
                        posTitle = line.find("(c)");
                        if (posTitle != string::npos){
                            titleRom = line.substr(0, posTitle - 1);
                            year = line.substr(posTitle + 4, 4);
                            developer = line.substr(posTitle + 4 + 4, line.length());
                        }
                    }
                }

                descRom = "";
                size_t posSection = line.find("- ");
                while ( (posSection == string::npos || posSection > 0) && continueReading){
                    if (!line.empty()){
                        descRom.append(line + VBCRLF);
                    }
                    continueReading = static_cast<bool>(getline (myfile, line));
                    posSection = line.find("- ");
                }

                if (splittedLine.size() > 1 && !descRom.empty()){
                    nameRom = splittedLine.at(1);
                    splittedNameRom = Constant::split(nameRom, ",");
                    for (int j=0; j < splittedNameRom.size(); j++){
                    //for (int j=0; j < 1; j++){
                        if (!splittedNameRom.at(j).empty()){
//                            cout << "adding: " << splittedNameRom.at(j) << endl;
                            cout << "adding: " << splittedNameRom.at(j) << "; titleRom: " << titleRom << "; year: " << year << "; developer: " << developer << endl;
                            map<string, string> attrRom;
                            attrRom.insert( make_pair(HISTORY_DESC, descRom));
                            attrRom.insert( make_pair(HISTORY_TECH, extractSection(&myfile, &line, H_FIELD_TECHNICAL, &attrRom)));
                            attrRom.insert( make_pair(HISTORY_TRIV, extractSection(&myfile, &line, H_FIELD_TRIVIA, &attrRom)));
                            attrRom.insert( make_pair(HISTORY_SCOR, extractSection(&myfile, &line, H_FIELD_SCORING, &attrRom)));
                            attrRom.insert( make_pair(HISTORY_TIPS, extractSection(&myfile, &line, H_FIELD_TIPS, &attrRom)));

                            attrRom.insert( make_pair(HISTORY_TITLE,        titleRom));
                            attrRom.insert( make_pair(HISTORY_YEAR,         year));
                            attrRom.insert( make_pair(HISTORY_DEVELOPER,    developer));

    /**Prueba para comprimir los datos*/
    //                        comprLen = 10000*sizeof(int); /* don't overflow on MSDOS */
    //                        test_compress(compr, &comprLen, descRom.c_str());
    //                        attrRom.insert( make_pair("desc", (char *)compr));
    //                        comprLen = 10000*sizeof(int); /* don't overflow on MSDOS */
    //                        test_compress(compr, &comprLen, extractSection(file, &i, "TECHNICAL").c_str() );
    //                        attrRom.insert( make_pair("tech", (char *)compr));
                            mapHistoryRoms.insert(make_pair(splittedNameRom.at(j), attrRom));
                        }
                        foundRoms++;
                    }
                }
            }
        }
        myfile.close();
    }
}


/**
* Extrae el contenido de una seccion. Hay que tener en cuenta que siempre se tienen
* que informar las secciones en orden. Sino no las encontrara
*/
string MameHistoryParser::extractSection(ifstream *myfile, string *line, string section, map<string, string> *attrRom){
    bool continueReading = true;
    size_t posSection = line->find("- " + section + " -");

    size_t posEnd;
    string sectionValue = "";
    bool endReached = false;
//    cout << "buscando: " << section << " en: " << *line << endl;

    while (!endReached && continueReading){
        if (posSection == 0){
            continueReading = static_cast<bool>(getline (*myfile, *line));
            posSection = line->find("- ");
            posEnd = line->find("$end");
            while ( (posEnd == string::npos || posEnd > 0) && (posSection == string::npos || posSection > 0) && continueReading){
                if (!line->empty()){
                    sectionValue.append(*line + VBCRLF);
                }

//                if (section.compare(H_FIELD_TECHNICAL) == 0){
//                    size_t posNPlayers = line->find(H_FIELD_NPLAYERS);
//                    if (posNPlayers == 0){
//                        string nplayers = line->substr(H_FIELD_NPLAYERS.length() + 1, line->length());
//                        attrRom->insert( make_pair(HISTORY_PLAYERS, nplayers));
////                        cout << "nplayers: " << nplayers << endl;
//                    }
//                }

                continueReading = static_cast<bool>(getline (*myfile, *line));
                posSection = line->find("- ");
                posEnd = line->find("$end");
            }
            endReached = true;
//            cout << "seccion: " << section << " Value: " << sectionValue << endl;
        } else {
            posEnd = line->find("$end");
            if (posEnd == string::npos || posEnd > 0){
                continueReading = static_cast<bool>((getline (*myfile, *line)));
                posSection = line->find("- " + section + " -");
            } else {
                endReached = true;
            }
        }
    }
    return sectionValue;
}


/**
*
*/
bool MameHistoryParser::test_compress(Byte *compr, uLong *comprLen, const char *src){
    int err;
    uLong len = (uLong)strlen(src)+1;
    err = compress(compr, comprLen, (const Bytef*)src, len);
    return err == Z_OK;
}

/**
*
*/
bool MameHistoryParser::test_uncompress(Byte *uncompr, uLong *uncomprLen, Byte *compr, uLong comprLen){
    int err;
    strcpy((char*)uncompr, "garbage");
    err = uncompress(uncompr, uncomprLen, compr, comprLen);
    return err == Z_OK;
}


