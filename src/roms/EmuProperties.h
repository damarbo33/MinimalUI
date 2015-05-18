#ifndef ESTRUCTURASEMU_H
#define ESTRUCTURASEMU_H



class Emuinfo{
    public:
        Emuinfo(){
            clear();
        };
        ~Emuinfo(){};

        struct struct_emu {
            string idEmu, nombreEmu, rutaEmu, parmsEmu, rutaRoms, emuRomExt, rutaImg, fixOption;
            bool descomp, titleRom;
        } emuProperties;

        void clear(){
            emuProperties.descomp = false;
            emuProperties.titleRom = false;
            emuProperties.idEmu = "";
            emuProperties.nombreEmu = "";
            emuProperties.rutaEmu = "";
            emuProperties.parmsEmu = "";
            emuProperties.rutaRoms = "";
            emuProperties.rutaImg = "";
            emuProperties.fixOption = "0";
        }
};



#endif
