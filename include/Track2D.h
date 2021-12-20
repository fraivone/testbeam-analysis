#include<vector>

#include "Track.h"
#include "Rechit2D.h"

#ifndef DEF_TRACK2D
#define DEF_TRACK2D

class Track2D {

    public:

        Track2D() {}

        void addRechit(Rechit2D);
        void clear();
        void fit();

        double getInterceptX() {return fTrackX.getIntercept();}
        double getInterceptY() {return fTrackY.getIntercept();}
        double getSlopeX() {return fTrackX.getSlope();}
        double getSlopeY() {return fTrackY.getSlope();}
        double getInterceptErrorX() {return fTrackX.getInterceptError();}
        double getInterceptErrorY() {return fTrackY.getInterceptError();}
        double getSlopeErrorX() {return fTrackX.getSlopeError();}
        double getSlopeErrorY() {return fTrackY.getSlopeError();}
    private:
        Track fTrackX, fTrackY;
};

#endif