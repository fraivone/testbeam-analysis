#include <iostream>

#ifndef DEF_DIGI
#define DEF_DIGI

class Digi{
    
    public:
        int fOh, fEta, fStrip;

        Digi() {}
        Digi(int oh, int eta, int strip);

        int getOh();
        int getEta();
        int getStrip();

        void print() {
            std::cout << fOh << "oh." << fEta << "eta." << fStrip << "strip" << std::endl;
        }
        
    //def __repr__(self):
    //    return f"Digi: {self.chamber},{self.xy},{self.strip}"
};

#endif