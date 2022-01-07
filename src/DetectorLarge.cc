#include "Cluster.h"
#include "Rechit.h"
#include "DetectorLarge.h"

DetectorLarge::DetectorLarge(int oh, int chamber, double baseNarrow, double baseWide, double height, int nEta, int nStrips) {
    fOh = oh;
    fChamber = chamber;
    fBaseNarrow = baseNarrow;
    fBaseWide = baseWide;
    fHeight = height;
    fNumberPartitions = nEta;
    fNumberStrips = nStrips;

    fPartitionYs.reserve(fNumberPartitions);
    fPartitionYTops.reserve(fNumberPartitions);
    fPartitionWidths.reserve(fNumberPartitions);
    fPartitionStripPitches.reserve(fNumberPartitions);
    fPartitionStripPitchesSqrt12.reserve(fNumberPartitions);
    for (int eta=0; eta<fNumberPartitions; eta++) {
        fPartitionYs[eta] = fHeight/fNumberPartitions*(0.5 + (double)(fNumberPartitions-eta));
        fPartitionYTops[eta] = fPartitionYs[eta] + 0.5*fHeight/fNumberPartitions;
        fPartitionWidths[eta] = fBaseNarrow + fPartitionYs[eta]*(fBaseWide-fBaseNarrow)/fHeight;
        fPartitionStripPitches[eta] = fPartitionWidths[eta] / fNumberStrips;
        fPartitionStripPitchesSqrt12[eta] = fPartitionStripPitches[eta] * 0.288675;
    }
    fOriginY = baseNarrow*height/(baseWide-baseNarrow);
}

double DetectorLarge::getY(int eta) {
    return fPartitionYs[eta-1];
}

double DetectorLarge::getYTop(int eta) {
    return fPartitionYTops[eta-1];
}

double DetectorLarge::getWidth(int eta) {
    return fPartitionWidths[eta-1];
}

double DetectorLarge::getStripPitch(int eta) {
    return fPartitionStripPitches[eta-1];
}

double DetectorLarge::getStripPitchSqrt12(int eta) {
    return fPartitionStripPitchesSqrt12[eta-1];
}

Rechit DetectorLarge::createRechit(Cluster cluster) {
    Rechit rechit(
        fChamber,
        -0.5*getWidth(cluster.getEta()) + getStripPitch(cluster.getEta())*cluster.getCenter(),
        cluster.getSize() * getStripPitchSqrt12(cluster.getEta()),
        cluster.getSize()
    );
    rechit.setY(getY(cluster.getEta()));
    return rechit;
}