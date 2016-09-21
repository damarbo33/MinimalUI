#ifndef MAMEHISTORYPARSER_H
#define MAMEHISTORYPARSER_H

#include <map>
#include <stdexcept>      // std::out_of_range
#include <vector>
#include <fstream>
#include "Constant.h"
#include "zlib.h"
#include <iostream>

using namespace std;

static const string HISTORY_DESC = "HISTORY_DESC";
static const string HISTORY_TECH = "HISTORY_TECH";
static const string HISTORY_TRIV = "HISTORY_TRIV";
static const string HISTORY_SCOR = "HISTORY_SCOR";
static const string HISTORY_TIPS = "HISTORY_TIPS";
static const string HISTORY_PLAYERS = "HISTORY_PLAYERS";
static const string HISTORY_YEAR = "HISTORY_YEAR";
static const string HISTORY_DEVELOPER = "HISTORY_DEVELOPERS";
static const string HISTORY_TITLE = "HISTORY_TITLE";

static const string H_FIELD_TECHNICAL = "TECHNICAL";
static const string H_FIELD_TRIVIA = "TRIVIA";
static const string H_FIELD_SCORING = "SCORING";
static const string H_FIELD_TIPS = "TIPS AND TRICKS";
static const string H_FIELD_NPLAYERS = "Players :";


class MameHistoryParser
{
    public:
        /** Default constructor */
        MameHistoryParser();
        /** Default destructor */
        virtual ~MameHistoryParser();
        void loadHistory(string ruta);
        string getRomSection(string rom, string section);
        map<string, string> getRomData(string rom);
    protected:
    private:
        map<string, map<string, string> > mapHistoryRoms;
        bool test_uncompress(Byte *uncompr, uLong *uncomprLen, Byte *compr, uLong comprLen);
        bool test_compress(Byte *compr, uLong *comprLen, const char *src);
        string extractSection(ifstream *myfile, string *line, string section, map<string, string> *attrRom);
};

#endif // MAMEHISTORYPARSER_H
