#include <iostream>

#ifndef DEF_DIGI
#define DEF_DIGI

class Digi{
    
    public:
        int fChamber, fEta, fStrip;

        Digi() {}
        Digi(int chamber, int eta, int strip);

        int getChamber();
        int getEta();
        int getStrip();

        void print() {
            std::cout << fChamber << "chamber." << fEta << "eta." << fStrip << "strip" << std::endl;
        }
};

#endif