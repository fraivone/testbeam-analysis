#include <iostream>
#include <vector>
#include <math.h>

#include "Rechit.h"
#include "Cluster.h"
#include "Corrections.h"

Rechit::Rechit(int chamber, int direction, Cluster cluster) {
    fChamber = chamber;
    fCenter = mappingStart[direction] + mappingScale[direction]*cluster.getCenter()*0.25 - corrections[direction][chamber];
    fClusterSize = cluster.getSize();
    fError = fClusterSize*PITCH_SQRT_12;
}

Rechit::Rechit(int chamber, int direction, double center, int clusterSize) {
    fChamber = chamber;
    fCenter = center;
    fClusterSize = clusterSize;
    fError = fClusterSize*PITCH_SQRT_12;
}

double Rechit::getCenter() {return fCenter; }
double Rechit::getError() {return fError; }
double Rechit::getClusterSize() {return fClusterSize; }
double Rechit::getZ() {return zChamber[fChamber]; }
int Rechit::getChamber() {return fChamber;}