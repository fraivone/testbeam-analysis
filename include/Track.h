#include<vector>

#include "Minuit2/FCNBase.h"
#include "TObject.h"

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

class Track : public TObject {

    public:

        Track();

        std::vector<Rechit> fRechits;

        void addRechit(Rechit);
        void clear();
        void fit();
        
        double propagate(double);
        double propagationError(double);

        double getIntercept() {return fIntercept;}
        double getSlope() {return fSlope;}
        double getInterceptError() {return fInterceptError;}
        double getSlopeError() {return fSlopeError;}

    private:
        double fIntercept, fSlope;
        double fInterceptError, fSlopeError;
};

#endif