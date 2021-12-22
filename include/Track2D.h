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

        double propagateX(double z) {return fTrackX.propagate(z);}
        double propagateY(double z) {return fTrackY.propagate(z);}
        double propagationErrorX(double z) {return fTrackX.propagationError(z);}
        double propagationErrorY(double z) {return fTrackY.propagationError(z);}

        double getInterceptX() {return fTrackX.getIntercept();}
        double getInterceptY() {return fTrackY.getIntercept();}
        double getSlopeX() {return fTrackX.getSlope();}
        double getSlopeY() {return fTrackY.getSlope();}
        double getInterceptErrorX() {return fTrackX.getInterceptError();}
        double getInterceptErrorY() {return fTrackY.getInterceptError();}
        double getSlopeErrorX() {return fTrackX.getSlopeError();}
        double getSlopeErrorY() {return fTrackY.getSlopeError();}
        double getChi2X() {return fTrackX.getChi2();}
        double getChi2Y() {return fTrackY.getChi2();}

    private:
    
        Track fTrackX, fTrackY;
};

#endif