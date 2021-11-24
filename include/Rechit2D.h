#include "Cluster.h"

#ifndef DEF_RECHIT2D
#define DEF_RECHIT2D

class Rechit2D {

    public:

        int fChamber;
        double fCenterX, fCenterY;
        double fErrorX, fErrorY;
        double fClusterSizeX, fClusterSizeY;

        double fCorrectionX[4] = { -0.367952, 0.218126, -0.384427, 0.0624537};
        double fCorrectionY[4] = { -1.98828, -0.250959, 0.458888, -0.115746 };

        Rechit2D() {}
        Rechit2D(int, Cluster, Cluster);
        //Rechit2D(int, int, int, int);

        double getCenterX();
        double getCenterY();
        double getErrorX();
        double getErrorY();
        double getClusterSizeX();
        double getClusterSizeY();
        int getChamber();

        void print() {
            std::cout << fChamber << "," << fCenterX << "," << fCenterY << "," << fErrorX << "," << fErrorY << std::endl;
        }
};

#endif