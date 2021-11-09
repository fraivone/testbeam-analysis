#include "Cluster.h"

#ifndef DEF_RECHIT2D
#define DEF_RECHIT2D

class Rechit2D {

    public:

        double fCenterX, fCenterY;
        double fErrorX, fErrorY;
        int fChamber;

        Rechit2D() {}
        Rechit2D(int, Cluster, Cluster);
        //Rechit2D(int, int, int, int);

        double getCenterX();
        double getCenterY();
        double getErrorX();
        double getErrorY();
        int getChamber();

        void print() {
            std::cout << fChamber << "," << fCenterX << "," << fCenterY << "," << fErrorX << "," << fErrorY << std::endl;
        }
};

#endif