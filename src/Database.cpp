#include "Database.h"


/**
*/
Database::Database(const char* filename){
	database = NULL;
	listaQuerys = new listaSimple<DataList>();
	clauseWhere = true;

	if (open(filename)){
        Dirutil dir;
        //Cargamos las querys en la lista de consultas sql
        loadQuerys(dir.getFolder(filename) + FILE_SEPARATOR + "querys.xml");
        //Comprobacion de funcionamiento
//        try{
//            this->prepareStatement("emuladores");
//            this->setInt(0,1034);
//            this->executeQuery();
//
//            vector<vector<string> > result = this->executeQuery();
//            for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it){
//                vector<string> row = *it;
//                Traza::print("RESULTADO QUERY: " + row.at(0) + ";" + row.at(1), W_DEBUG);
//            }
//
//        } catch (Excepcion &e){
//            Traza::print("Database(): " + string(e.getMessage()), W_ERROR);
//        }
    }
}
/**
*/
Database::~Database(){
    delete listaQuerys;
}

/**
*
*/
bool Database::loadQuerys(string filename){

    TiXmlDocument doc(filename.c_str());
	bool loadOkay = doc.LoadFile();

	if (loadOkay){
		cout << filename << endl;
		loadQuerys( &doc , this->listaQuerys);
		int pos = this->listaQuerys->find("emuladores");
		if (pos >= 0){
            DataList lista = this->listaQuerys->get(pos);
            Data xmlSqlDatos = lista.getSubElem(0);
            Traza::print("Campo recuperado: " + xmlSqlDatos.getValue() + " de la etiqueta: " + xmlSqlDatos.getKey(), W_PARANOIC);
		}
		return true;
	}
	else{
		Traza::print("Ha ocurrido un error al abrir el fichero " + filename + ". Posible error de formato",W_DEBUG);
		return false;
	}
}


/**
*
*/
void Database::loadQuerys( TiXmlNode* pParent, listaSimple<DataList> *varElement)
{
	if ( !pParent ) return;
    TiXmlNode* pChild;
    TiXmlText* pText;
    int t = 0;
    int pos = 0;

    TiXmlElement* root = pParent->FirstChildElement();
    if(root == NULL){
		cerr << "Failed to load file: No root element." << endl;
		return;
	}

    for(TiXmlElement* elem = root->FirstChildElement(); elem != NULL; elem = elem->NextSiblingElement()){
		string elemName = elem->Value();
        Traza::print( "TINYXML_ELEMENT:" + elemName, W_PARANOIC );
        const char* attr;


		if(elemName == "QUERY"){
            DataList *queryData = new DataList();
			//Obtenemos el valor del atributo de la etiqueta query
			attr = elem->Attribute("id");
			if(attr != NULL){
                Traza::print( "Parm id:" + string(attr), W_PARANOIC );
                queryData->setKey(attr);
			}
			int nValues = 0;
			//Obtenemos la etiqueta SENTENCE que no contiene atributos
            TiXmlElement* e = elem->FirstChildElement("SENTENCE");
            if (e != NULL){
                //Obtenemos el contenido del elemento SENTENCE
                TiXmlText* text = e->FirstChild()->ToText();
                if(text != NULL){
                    queryData->addDataKeyValue("SENTENCE", text->Value());
                    nValues = Constant::countInStr(text->Value());

                    Traza::print( "SENTENCE text: " + string(text->Value()) + ", parms: " + Constant::TipoToStr(nValues), W_PARANOIC );


                    //Si obtuvimos una query, mostramos su where en el caso de que lo tenga
                    e = elem->FirstChildElement("WHERECLAUSE");
                    if (e != NULL){
                         //Obtenemos el contenido del elemento WHERECLAUSE
                        text = e->FirstChild()->ToText();
                        if(text != NULL){
                            queryData->addDataKeyValue("WHERECLAUSE", text->Value());
                            nValues += Constant::countInStr(text->Value());
                            Traza::print( "WHERECLAUSE text: " + string(text->Value()) + ", parms: " + Constant::TipoToStr(nValues), W_PARANOIC );
                        }
                    }
                    queryData->addDataKeyValue("sqlvalues", Constant::TipoToStr(nValues));
                }
            }
            varElement->add(*queryData);
		}
	}
	//Finalmente ordenamos para que la busqueda binaria funcione correctamente
	varElement->sort();
}

/**
*
*/
bool Database::open(const char* filename){
	if(sqlite3_open(filename, &database) == SQLITE_OK)
		return true;

	return false;
}

/**
*
*/
bool Database::execute(string query){
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);
    if (executeNoCommit(query)){
        sqlite3_exec(database, "COMMIT TRANSACTION", NULL, NULL, NULL);
        return true;
    }
    return false;
}

/**
*
*/
bool Database::executeNoCommit(string query){
    int rc = 0;
    char *zErrMsg = 0;
    const char * tmpQuery = query.c_str();

    Traza::print(tmpQuery, W_PARANOIC);
    rc = sqlite3_exec(database, tmpQuery, NULL, 0, &zErrMsg);
    Traza::print("Query realizada", W_PARANOIC);

    if (rc != SQLITE_OK){
        string serror = sqlite3_errmsg(database);
        string msgError = "SQLExcepcion: " + serror + "->" + query;
        Traza::print(msgError.c_str(), W_ERROR);
        return false;
    }
    return true;
}



/**
*
*/
vector<vector<string> > Database::query(string query){
	sqlite3_stmt *statement;
	vector<vector<string> > results;

    try{
        if(sqlite3_prepare_v2(database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
        {
            int cols = sqlite3_column_count(statement);
            int result = 0;
            do
            {
                result = sqlite3_step(statement);

                if(result == SQLITE_ROW)
                {
                    vector<string> values;
                    values.reserve(cols);

                    for(int col = 0; col < cols; col++)
                    {
                        if (sqlite3_column_text(statement, col) != NULL)
                            values.push_back((char*)sqlite3_column_text(statement, col));
                        else
                            values.push_back("");
                    }
                    results.push_back(values);
                }

            }while (result == SQLITE_ROW);

            sqlite3_finalize(statement);
        }

        string serror = sqlite3_errmsg(database);
        int ierror = sqlite3_errcode(database) ;
        if(ierror != SQLITE_OK){
            string myerror = "SQLExcepcion - Cod. error: " + Constant::TipoToStr(ierror) + " - Desc. error: " + serror + " -> " + query ;
            Traza::print(myerror.c_str(), W_ERROR);
    //        throw Excepcion(ierror, sW_ERROR);
        }

    } catch(...){
        Traza::print("Database::query::SQLExcepcion::", W_ERROR);
    }
	return results;
}

/**
* Evitamos los errores de las comillas simples ' = 0x27
*/
string Database::scapeStr(string dato){
     // set our locator equal to the first appearance of any character in replace
    //size_t i = dato.find_first_of("'");
    string salida = "";
    for (unsigned int i=0; i < dato.length(); i++){
        salida += dato[i];
        if (dato[i] == 0x27)
            salida.append("'");
    }
    return salida;
}

/**
*
*/
void Database::close()
{
	sqlite3_close(database);
}

/**
*
*/
void Database::prepareStatement(string statement){
    int pos = listaQuerys->find(statement);
    if (pos >= 0){
        this->statementDataList = listaQuerys->get(pos);
        statementValues.clear();
        this->clauseWhere = true;
        int pos = this->statementDataList.find("sqlvalues");
        if (pos >= 0){
            int numParms = Constant::strToTipo<int>(this->statementDataList.getSubElem(pos).getValue());
            statementValues.resize(numParms);
        }
    }
}

/**
*
*/
void Database::setString(int pos, string var){
    if (pos < statementValues.size()){
        statementValues.at(pos) =  "'" + var + "'";
    } else {
        throw(Excepcion(ERANGOSQL));
    }
}

/**
*
*/
void Database::setInt(int pos, int var){
    if (pos < statementValues.size()){
        statementValues.at(pos) = Constant::TipoToStr(var);
    } else {
        throw(Excepcion(ERANGOSQL));
    }
}

/**
*
*/
bool Database::executeNoCommit(){
    int rc = 0;
    char *zErrMsg = 0;
    string tmpQuery = "";

    if (generarQuery(&tmpQuery)){
        Traza::print("executeNoCommit: " + tmpQuery, W_PARANOIC);
        rc = sqlite3_exec(database, tmpQuery.c_str(), NULL, 0, &zErrMsg);
        Traza::print("Query realizada", W_PARANOIC);

        if (rc != SQLITE_OK){
            string serror = sqlite3_errmsg(database);
            string msgError = "SQLExcepcion: " + serror + "->" + tmpQuery;
            Traza::print(msgError.c_str(), W_ERROR);
            return false;
        }
        return true;
    } else {
        Traza::print("No hay suficientes parametros informados para la consulta: " + tmpQuery, W_ERROR);
        return false;
    }
}
/**
*
*/
bool Database::execute(){
    sqlite3_exec(database, "BEGIN TRANSACTION", NULL, NULL, NULL);
    if (executeNoCommit()){
        sqlite3_exec(database, "COMMIT TRANSACTION", NULL, NULL, NULL);
        return true;
    }
    return false;
}

/**
*
*/
vector<vector<string> > Database::executeQuery(){
	sqlite3_stmt *statement;
	vector<vector<string> > results;
    string query = "";

    bool ret = generarQuery(&query);

    if (ret){
        try{
            if(sqlite3_prepare_v2(database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
            {
                int cols = sqlite3_column_count(statement);
                int result = 0;
                do
                {
                    result = sqlite3_step(statement);

                    if(result == SQLITE_ROW)
                    {
                        vector<string> values;
                        values.reserve(cols);

                        for(int col = 0; col < cols; col++)
                        {
                            if (sqlite3_column_text(statement, col) != NULL)
                                values.push_back((char*)sqlite3_column_text(statement, col));
                            else
                                values.push_back("");
                        }
                        results.push_back(values);
                    }

                }while (result == SQLITE_ROW);

                sqlite3_finalize(statement);
            }

            string serror = sqlite3_errmsg(database);
            int ierror = sqlite3_errcode(database) ;
            if(ierror != SQLITE_OK){
                string myerror = "SQLExcepcion - Cod. error: " + Constant::TipoToStr(ierror) + " - Desc. error: " + serror + " -> " + query ;
                Traza::print(myerror.c_str(), W_ERROR);
            }

        } catch(...){
            Traza::print("Database::query::SQLExcepcion::", W_ERROR);
        }
    } else {
        Traza::print("Database::query::Error al lanzar la query::", W_ERROR);
    }
	return results;
}

/**
*
*/
bool Database::generarQuery(string *query){
    int pos = 0;


    Traza::print("Database::generarQuery::Buscando sentencia de: " + *query, W_DEBUG);
    //Obtenemos la query
    if ((pos = this->statementDataList.find("SENTENCE")) >= 0 ){
        *query = this->statementDataList.getSubElem(pos).getValue();
    }

    //Comprobamos que existe el campo sqlvalues
    pos = this->statementDataList.find("sqlvalues");
    if (pos >= 0){
        //Obtenemos el numero de parametros esperados de la query que debe coincidir con el
        //tamanyo del vector que contiene los parametros a setear
        int numParmsIN = Constant::strToTipo<int>(this->statementDataList.getSubElem(pos).getValue());

        if (this->clauseWhere && (pos = this->statementDataList.find("WHERECLAUSE")) >= 0){
           *query += " " + this->statementDataList.getSubElem(pos).getValue();
        }

        //Comprobamos que los tamanyos son correctos
        if (numParmsIN == statementValues.size()){
            for (int i=0; i<statementValues.size(); i++){
                *query = Constant::replace(*query, "?", statementValues.at(i));
            }
            Traza::print("****** GENERARQUERY ********: " + *query, W_DEBUG);
            return true;
        } else {
            Traza::print("Database::generarQuery::No coinciden los parametros informados " + Constant::TipoToStr(statementValues.size()) + " con la query: " + Constant::TipoToStr(numParmsIN), W_ERROR);
            return false;
        }
    } else {
        Traza::print("Database::generarQuery::Error grave. No se ha generado la query correctamente " + *query, W_ERROR);
        return false;
    }
}

/**
*
*/
string Database::getColQuery(vector<vector<string> > *result, int col){

    string resultado = "";
    int fila = 0;

    try{
        if (database != NULL) {
            if (result->size() > fila){
                if (result->at(fila).size() > col)
                    resultado = result->at(fila).at(col);
            }
        }
    } catch(Excepcion &e){
        Traza::print("Database::getColQuery" + string(e.getMessage()), W_ERROR);
    }
    return resultado;
}


