#ifndef BDROMINFOOUT_H
#define BDROMINFOOUT_H


class BDRomInfoOut
{
    public:
        /** Default constructor */
        BDRomInfoOut(){
            idprog="";
            idrom="";
            nplayers="";
            title="";
            categ="";
            year="";
            boxart="";
            description="";
            publisher="";
            developer="";
            rating="";
            platform="";
            scrapped="";
        }
        /** Default destructor */
        ~BDRomInfoOut(){};
        /** Access idprog
         * \return The current value of idprog
         */
        string Getidprog() { return idprog; }
        /** Set idprog
         * \param val New value to set
         */
        void Setidprog(string val) { idprog = val; }
        /** Access idrom
         * \return The current value of idrom
         */
        string Getidrom() { return idrom; }
        /** Set idrom
         * \param val New value to set
         */
        void Setidrom(string val) { idrom = val; }
        /** Access nplayers
         * \return The current value of nplayers
         */
        string Getnplayers() { return nplayers; }
        /** Set nplayers
         * \param val New value to set
         */
        void Setnplayers(string val) { nplayers = val; }
        /** Access title
         * \return The current value of title
         */
        string Gettitle() { return title; }
        /** Set title
         * \param val New value to set
         */
        void Settitle(string val) { title = val; }
        /** Access categ
         * \return The current value of categ
         */
        string Getcateg() { return categ; }
        /** Set categ
         * \param val New value to set
         */
        void Setcateg(string val) { categ = val; }
        /** Access year
         * \return The current value of year
         */
        string Getyear() { return year; }
        /** Set year
         * \param val New value to set
         */
        void Setyear(string val) { year = val; }
        /** Access boxart
         * \return The current value of boxart
         */
        string Getboxart() { return boxart; }
        /** Set boxart
         * \param val New value to set
         */
        void Setboxart(string val) { boxart = val; }
        /** Access description
         * \return The current value of description
         */
        string Getdescription() { return description; }
        /** Set description
         * \param val New value to set
         */
        void Setdescription(string val) { description = val; }
        /** Access publisher
         * \return The current value of publisher
         */
        string Getpublisher() { return publisher; }
        /** Set publisher
         * \param val New value to set
         */
        void Setpublisher(string val) { publisher = val; }
        /** Access developer
         * \return The current value of developer
         */
        string Getdeveloper() { return developer; }
        /** Set developer
         * \param val New value to set
         */
        void Setdeveloper(string val) { developer = val; }
        /** Access rating
         * \return The current value of rating
         */
        string Getrating() { return rating; }
        /** Set rating
         * \param val New value to set
         */
        void Setrating(string val) { rating = val; }
        /** Access platform
         * \return The current value of platform
         */
        string Getplatform() { return platform; }
        /** Set platform
         * \param val New value to set
         */
        void Setplatform(string val) { platform = val; }
        /** Access scrapped
         * \return The current value of scrapped
         */
        string Getscrapped() { return scrapped; }
        /** Set scrapped
         * \param val New value to set
         */
        void Setscrapped(string val) { scrapped = val; }
    protected:
    private:
        string idprog; //!< Member variable "idprog"
        string idrom; //!< Member variable "idrom"
        string nplayers; //!< Member variable "nplayers"
        string title; //!< Member variable "title"
        string categ; //!< Member variable "categ"
        string year; //!< Member variable "year"
        string boxart; //!< Member variable "boxart"
        string description; //!< Member variable "description"
        string publisher; //!< Member variable "publisher"
        string developer; //!< Member variable "developer"
        string rating; //!< Member variable "rating"
        string platform; //!< Member variable "platform"
        string scrapped; //!< Member variable "scrapped"
};

#endif // BDROMINFOOUT_H
