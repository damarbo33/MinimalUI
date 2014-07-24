#include "unziptool.h"

UnzipTool::UnzipTool()
{
    //ctor
}

UnzipTool::~UnzipTool()
{
    //dtor
}

unzippedFileInfo UnzipTool::descomprimirZip(const char *ruta)
{

    unzFile myZip = NULL;
    myZip = unzOpen(ruta);
    unz_global_info zip_global_info;
    int ret;

    unsigned long nFilesInZip = 0;
    unzippedFileInfo retorno;
    unzippedFileInfo tempRetorno;

    tempRetorno.errorCode = -1;
    tempRetorno.rutaEscritura = ruta;
    tempRetorno.romsize = 0;

    retorno.errorCode = -1;
    retorno.rutaEscritura = ruta;
    retorno.romsize = 0;


    if (myZip == NULL)
    {
        printf("Unable to open zip file.\n");
        return(retorno);
    }
    else
    {
        printf("File found.\n");
    }

    ret = unzGetGlobalInfo(myZip, &zip_global_info);
    if (ret != UNZ_OK)
    {
        printf("unzGetGlobalInfo() call failed.\n");
        return(retorno);
    }
    else
    {
//        printf("Found %d files in archive.\n", zip_global_info.number_entry);
//        printf("Comment length is %d bytes.\n", zip_global_info.size_comment);
        nFilesInZip = zip_global_info.number_entry;
        printf("Found %lu files in archive.\n", nFilesInZip);
    }

    ret = unzGoToFirstFile(myZip);

    if (ret != UNZ_OK)
    {
        printf("unzGoToFirstFile() call failed.\n");
        return(retorno);
    }


    while (ret == UNZ_OK){
        retorno = extraerFichero(myZip, ruta);
        if (retorno.romsize > tempRetorno.romsize){
            tempRetorno.errorCode = retorno.errorCode;
            tempRetorno.romsize = retorno.romsize;
            tempRetorno.rutaEscritura = retorno.rutaEscritura;
        }
        ret = unzGoToNextFile(myZip);
    }

    return tempRetorno;
}

/**
*
*/
unzippedFileInfo UnzipTool::extraerFichero(unzFile myZip, const char *ruta){

    int ret;
    unsigned long rom_size = 0;
     char filename[256];
    char *cartridge;
    unzippedFileInfo retorno;
    retorno.errorCode = -1;
    retorno.rutaEscritura = ruta;
    retorno.romsize = 0;
    unz_file_info zip_file_info;

    ret = unzGetCurrentFileInfo(myZip, &zip_file_info, filename, 256, NULL, 0, NULL, 0);

    if (ret == UNZ_OK)
    {
        //printf("Filename is %s.\n",filename);
        cout << "Uncompressed size is in bytes: " << zip_file_info.uncompressed_size << endl;
        rom_size = zip_file_info.uncompressed_size;
        retorno.romsize = rom_size;
    }

    ret = unzOpenCurrentFile(myZip);
    if (ret != UNZ_OK)
    {
        printf("Unable to open file from zip archive.\n");
        return(retorno);
    }

    cartridge = new char[rom_size];

    if (cartridge == NULL)
    {
        cerr << "Unable allocate memory for cartridge."<< endl;
        cerr << "Exiting." << endl;
        return(retorno);
    }

    // Load rom file.
    ret = unzReadCurrentFile(myZip, cartridge, rom_size);

    Fileio *fichero = new Fileio();
    Dirutil *dirUtil = new Dirutil();

    string filenameOut = dirUtil->getFolder(ruta) + FILE_SEPARATOR + filename;

    try{
        fichero->writeToFile(filenameOut.c_str(), cartridge, rom_size, false);
        retorno.rutaEscritura = filenameOut;
        retorno.errorCode = 0;
    } catch (Excepcion &e){
        Traza::print("Error UnzipTool::extraerFichero" + string(e.getMessage()), W_ERROR);
    }

    delete fichero;
    delete dirUtil;
    unzCloseCurrentFile(myZip);

    return retorno;
}


