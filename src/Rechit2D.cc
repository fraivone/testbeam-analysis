#include <iostream>
#include <vector>

#include "Rechit2D.h"
#include "Cluster.h"

Rechit2D::Rechit2D(int chamber, Cluster cluster1, Cluster cluster2) {
    fChamber = chamber;
    fCenterX = -44.75 + cluster1.getCenter()*0.25;
    fCenterY = 44.75 - cluster2.getCenter()*0.25;
    fSizeX = cluster1.getSize()*0.25;
    fSizeY = cluster2.getSize()*0.25;
}

double Rechit2D::getCenterX() {return fCenterX; }
double Rechit2D::getCenterY() {return fCenterY; }
double Rechit2D::getSizeX() {return fSizeX; }
double Rechit2D::getSizeY() {return fSizeY; }

int Rechit2D::getChamber() {return fChamber;}