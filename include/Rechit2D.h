#include "Cluster.h"

#ifndef DEF_RECHIT2D
#define DEF_RECHIT2D

class Rechit2D {

    public:

        double fCenterX, fCenterY;
        double fSizeX, fSizeY;
        int fChamber;

        Rechit2D() {}
        Rechit2D(int, Cluster, Cluster);
        //Rechit2D(int, int, int, int);

        double getCenterX();
        double getCenterY();
        double getSizeX();
        double getSizeY();
        int getChamber();

        void print() {
            std::cout << fChamber << "," << fCenterX << "," << fCenterY << "," << fSizeX << "," << fSizeY << std::endl;
        }
};

#endif