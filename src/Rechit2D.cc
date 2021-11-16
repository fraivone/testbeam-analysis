#include <iostream>
#include <vector>
#include <math.h>

#include "Rechit2D.h"
#include "Cluster.h"

Rechit2D::Rechit2D(int chamber, Cluster cluster1, Cluster cluster2) {
    fChamber = chamber;
    fCenterX = -44.75 + cluster1.getCenter()*0.25;
    fCenterY = 44.75 - cluster2.getCenter()*0.25;
    fClusterSizeX = cluster1.getSize();
    fClusterSizeY = cluster2.getSize();
    // calculate rechit error as pitch * sqrt(12) * sqrt(nstrips):
    fErrorX = sqrt(cluster1.getSize())*0.866;
    fErrorY = sqrt(cluster2.getSize())*0.866;
}

double Rechit2D::getCenterX() {return fCenterX; }
double Rechit2D::getCenterY() {return fCenterY; }
double Rechit2D::getErrorX() {return fErrorX; }
double Rechit2D::getErrorY() {return fErrorY; }
double Rechit2D::getClusterSizeX() {return fClusterSizeX; }
double Rechit2D::getClusterSizeY() {return fClusterSizeY; }

int Rechit2D::getChamber() {return fChamber;}