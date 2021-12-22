#include <vector>

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

class Track {

    public:

        Track();

        std::vector<Rechit> fRechits;

        void addRechit(Rechit);
        void clear();
        void fit();
        
        double propagate(double);
        double propagationError(double);

        std::vector<double> getRechitZ();
        std::vector<double> getRechitCenters();
        std::vector<double> getRechitErrors();
        double getIntercept() {return fIntercept;}
        double getSlope() {return fSlope;}
        double getInterceptError() {return fInterceptError;}
        double getSlopeError() {return fSlopeError;}
        double getChi2() {return fChi2;}

    private:
    
        double fIntercept, fSlope;
        double fInterceptError, fSlopeError;
        double fChi2;
};

#endif