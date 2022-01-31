#include <iostream>
#include <vector>

#include "Cluster.h"
#include "Digi.h"

Cluster::Cluster(int slot, int oh, int eta, int first, int last) {
    fSlot = slot;
    fOh = oh;
    fEta = eta;
    fFirst = first;
    fLast = last;
}

bool Cluster::isNeighbour(int strip) {
    return (strip==fFirst-1) || (strip==fLast+1);
}

void Cluster::extend(int strip) {
    if (strip < fFirst) fFirst = strip;
    else if (strip > fLast) fLast = strip;
}

double Cluster::getCenter() {
    return 0.5*(fFirst + fLast);
}

double Cluster::getSize() {
    return fLast - fFirst + 1;
}

int Cluster::getSlot() {return fSlot;}
int Cluster::getOh() {return fOh;}
int Cluster::getEta() {return fEta;}
int Cluster::getFirst() {return fFirst;}
int Cluster::getLast() {return fLast;}

int Cluster::getChamber() {
    if (fSlot==0) {
        if (fOh>0) {
            return (fOh-2)*2 + (fEta-1)/2;
        } else {
            return 0;
        }
    } else return 5;
}

int Cluster::getDirection() {
    if (fOh>0) {
        return fEta % 2;
    } else {
        return 0;
    }
}

std::vector<Cluster> Cluster::fromDigis(std::vector<Digi> digis) {
    std::vector<Cluster> clusters;

    int slot;
    int oh;
    int eta;

    while (digis.size()>0) {
        Digi digi = digis[0];

        // Map OH, eta to chamber
        slot = digi.getSlot();
        oh = digi.getOh();
        eta = digi.getEta();

        // Use as seed for cluster:
        Cluster cluster = Cluster(slot, oh, eta, digi.getStrip(), digi.getStrip());
        digis.erase(digis.begin());

        bool clusterUpdated = true;
        while (clusterUpdated) {
            clusterUpdated = false;
            for (int j=0; j<digis.size();) {
                if ((digis[j].getSlot() != cluster.getSlot()) || (digis[j].getOh() != cluster.getOh()) || (digis[j].getEta() != cluster.getEta())) break;
                if (cluster.isNeighbour(digis[j].getStrip())) {
                    cluster.extend(digis[j].getStrip());
                    digis.erase(digis.begin() + j); // Don't increase j, list fill flow
                    // The cluster ends grew, so there might be more strips included
                    // Then scan the digi list again later:
                    clusterUpdated = true;
                } else j++;
            }
        }
        clusters.push_back(cluster);
    }
    return clusters;
}