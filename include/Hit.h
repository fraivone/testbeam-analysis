#ifndef DEF_HIT
#define DEF_HIT

#include "DetectorGeometry.h"

class Hit {

    public:
        Hit() {}
        Hit(DetectorGeometry *detector, double x, double y, double z, double errX, double errY, double errZ);
        Hit(double x, double y, double z, double errX, double errY, double errZ);

        void setDetector(DetectorGeometry *detector);

        double getGlobalX() {return fGlobalPosition[0];}
        double getGlobalY() {return fGlobalPosition[1];}
        double getGlobalZ() {return fGlobalPosition[2];}
        double getErrX() {return fErrPosition[0];}
        double getErrY() {return fErrPosition[1];}
        double getErrZ() {return fErrPosition[2];}

        double getLocalX() {return fLocalPosition[0];}
        double getLocalY() {return fLocalPosition[1];}
        double getLocalR() {return fLocalR;}
        double getLocalPhi() {return fLocalPhi;}

        int getEta();

    private:
        double fGlobalPosition[3];
        double fLocalPosition[2];
        double fLocalR, fLocalPhi;
        double fErrPosition[3];
        DetectorGeometry *fDetector;
};

#endif