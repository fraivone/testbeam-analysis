#include<vector>

#include "Minuit2/FCNBase.h"

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

// void linearFunction(int &npar, double *gin, double &chi2, double *par, int iflag) {
//     double q = par[0], m = par[1];
//     chi2 = 0.;
//     for (int i=0; i<ndata; i++) {
//         chi2 += pow(x_i-m*z_i-q, 2)/pow(err_i, 2);
//     }
// }

class Track {

    public:

        std::vector<Rechit> fRechits;

        double fIntercept, fSlope;
        double fInterceptError, fSlopeError;

        void addRechit(Rechit);
        void clear();
        void fit();
};

#endif