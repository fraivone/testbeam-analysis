#include "Cluster.h"
#include "Rechit.h"

#ifndef DEF_RECHIT2D
#define DEF_RECHIT2D

class Rechit2D {

    public:

        int fChamber;
        Rechit fRechitX, fRechitY;

        Rechit2D() {}
        // Rechit2D(int, Cluster, Cluster);
        Rechit2D(int, Rechit, Rechit);

        double getCenterX();
        double getCenterY();
        double getErrorX();
        double getErrorY();
        double getClusterSizeX();
        double getClusterSizeY();
        int getChamber();

        void setGlobalPosition(double x, double y, double z);
        double getGlobalX() { return fRechitX.getGlobalCenter(); }
        double getGlobalY() { return fRechitY.getGlobalCenter(); }
        double getGlobalZ() { return fRechitX.getGlobalZ(); }

        void print() {
            std::cout << fChamber << "," << getCenterX() << "," << getCenterY() << "," << getErrorX() << "," << getErrorY() << std::endl;
        }
};

#endif