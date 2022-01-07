#include <vector>

#include "DetectorGeometry.h"
#include "Rechit.h"
#include "Rechit2D.h"

#ifndef DEF_DETECTORTRACKER
#define DEF_DETECTORTRACKER

class DetectorTracker : public DetectorGeometry {

    public:

        DetectorTracker(int oh, int chamber, double sizeX, double sizeY, int nStrips);

        double getY(int eta);
        double getWidth(int eta);
        
        double getSizeX() { return fSize[0]; }
        double getSizeY() { return fSize[1]; }
        double getNStrips() { return fNumberStrips; }
        double getStripPitch() { return fStripPitch; }
        double getStripPitchSqrt12() { return fStripPitchSqrt12; }

        Rechit createRechit(Cluster cluster);
        Rechit2D createRechit2D(Cluster cluster1, Cluster cluster2);
        void mapRechit2D(Rechit2D *rechit);
    
    private:

        int fOh, fChamber, fNumberPartitions;
        double fSize[2];
        int fScale[2] = {1, -1};
        int fNumberStrips;
        double fStripPitch, fStripPitchSqrt12;

        std::vector<double> fPartitionYs;
        std::vector<double> fPartitionWidths;
        std::vector<double> fPartitionStripPitches;
        std::vector<double> fPartitionStripPitchesSqrt12;
};

#endif