#include "Cluster.h"

#ifndef DEF_RECHIT
#define DEF_RECHIT

// to calculate rechit error as pitch * sqrt(12) * nstrips:
const double PITCH_SQRT_12 = 0.07217;

class Rechit {

    public:

        int fChamber;
        double fCenter;
        double fError;
        double fClusterSize;

        // double fCorrectionX[4] = { -0.367952, 0.218126, -0.384427, 0.0624537};
        // double fCorrectionY[4] = { -1.98828, -0.250959, 0.458888, -0.115746 };

        // double fCorrectionX[4] = { -0.269035, +0.292939, -0.332708, +0.0905448 };
        // double fCorrectionY[4] = { -2.06244, -0.207079, +0.577936, +0.108215 };

        Rechit() {}
        Rechit(int, int, Cluster);
        Rechit(int, int, double, int);

        double getCenter();
        double getError();
        double getClusterSize();
        double getZ();
        int getChamber();

        void print() {
            std::cout << fChamber << "," << fCenter << "," << fError << std::endl;
        }
};

#endif