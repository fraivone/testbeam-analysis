#include <math.h>

#include "Hit.h"
#include "DetectorLarge.h"

Hit::Hit(double x, double y, double z, double errX, double errY, double errZ) {
    fGlobalPosition[0] = x;
    fGlobalPosition[1] = y;
    fGlobalPosition[2] = z;
    fErrPosition[0] = errX;
    fErrPosition[1] = errY;
    fErrPosition[2] = errZ;
}

Hit::Hit(DetectorGeometry *detector, double x, double y, double z, double errX, double errY, double errZ) :
    Hit(x, y, z, errX, errY, errZ) {
    setDetector(detector);
}

Hit Hit::fromLocal(DetectorGeometry *detector, double localX, double localY, double errX, double errY, double errZ) {
    return Hit(
        detector,
        localX+detector->getPositionX(), localY+detector->getPositionY(), detector->getPositionZ(),
        errX, errY, errZ 
    );
}

void Hit::setDetector(DetectorGeometry *detector) {
    fDetector = detector;
    // calculate local coordinates:
    fLocalPosition[0] = fGlobalPosition[0] - detector->getPositionX();
    fLocalPosition[1] = fGlobalPosition[1] - detector->getPositionY();
    fLocalR = sqrt( pow(fLocalPosition[0],2) + pow(fLocalPosition[1]-detector->getOriginY(),2) );
    fLocalPhi = atan( fLocalPosition[0]/(fLocalPosition[1]-detector->getOriginY()) );
}

int Hit::getEta() {
    // calculate eta partition of hit from detector geometry
    return ((DetectorLarge*)fDetector)->getNEta() - 
    (int)(getLocalY()/((DetectorLarge*)fDetector)->getEtaHeight());
}