#include<vector>

#include "Minuit2/FCNBase.h"

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

class Track {

    public:

        Track();

        std::vector<Rechit> fRechits;

        double fIntercept, fSlope;
        double fInterceptError, fSlopeError;

        void addRechit(Rechit);
        void clear();
        void fit();
};

#endif