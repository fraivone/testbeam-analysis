#include <iostream>
#include <vector>
#include <math.h>

#include "Rechit.h"
#include "Cluster.h"
#include "Corrections.h"

Rechit::Rechit(int chamber, int direction, Cluster cluster) {
    fChamber = chamber;
    fCenter = x0[direction] + cluster.getCenter()*0.25 - corrections[direction][chamber];
    fClusterSize = cluster.getSize();
    // calculate rechit error as pitch * sqrt(12) * sqrt(nstrips):
    fError = fClusterSize*0.07217;
}

double Rechit::getCenter() {return fCenter; }
double Rechit::getError() {return fError; }
double Rechit::getClusterSize() {return fClusterSize; }
int Rechit::getChamber() {return fChamber;}