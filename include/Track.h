#include <vector>

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

class Track {

    public:

        Track();

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
        double getChi2Reduced() {return fChi2/(fRechits.size()-1);}
        bool isValid() {return fIsValid;}

    private:
    
        std::vector<Rechit> fRechits;
        double fIntercept, fSlope;
        double fInterceptError, fSlopeError;
        double fChi2;
        bool fIsValid;
};

#endif