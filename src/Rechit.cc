#include <iostream>
#include <vector>
#include <math.h>

#include "Rechit.h"
#include "Cluster.h"
#include "Geometry.h"

// Rechit::Rechit(int chamber, int direction, Cluster cluster) {
//     fChamber = chamber;
//     fCenter = mappingStart[chamber][direction] + mappingScale[chamber][direction]*cluster.getCenter()*mappingPitch[chamber] - mappingCorrections[chamber][direction];
//     fClusterSize = cluster.getSize();
//     fError = fClusterSize*mappingPitchSqrt12[chamber];
// }

// Rechit::Rechit(int chamber, double center, int clusterSize) {
//     fChamber = chamber;
//     fCenter = center;
//     fClusterSize = clusterSize;
//     fError = fClusterSize*mappingPitchSqrt12[chamber];
// }

Rechit::Rechit(int chamber, double center, double error, int clusterSize) {
    fChamber = chamber;
    fCenter = center;
    fError = error;
    fClusterSize = clusterSize;
}

void Rechit::setGlobalPosition(double center, double z) {
    fGlobalCenter = center;
    fGlobalZ = z;
}

double Rechit::getCenter() {return fCenter; }
double Rechit::getError() {return fError; }
double Rechit::getClusterSize() {return fClusterSize; }
int Rechit::getChamber() {return fChamber;}