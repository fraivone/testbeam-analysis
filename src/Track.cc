#include <iostream>
#include <vector>
#include <math.h>

#include <TMinuit.h>

#include "Rechit.h"
#include "Track.h"

TMinuit trackMinuit(2);
// static linear track function
void linearFunctionChi2(int &npar, double *gin, double &chi2, double *par, int iflag) {
    double q = par[0], m = par[1];
    chi2 = 0.;
    Track *track = (Track *) trackMinuit.GetObjectFit(); 
    for (Rechit rechit:track->fRechits) {
        chi2 += pow(rechit.getCenter()-m*rechit.getZ()-q, 2)/pow(rechit.getError(), 2);
    }
}

Track::Track() {
    trackMinuit.SetFCN(linearFunctionChi2);
}

void Track::addRechit(Rechit rechit) {
    fRechits.push_back(rechit);
}

void Track::clear() {
    fRechits.clear();
}

void Track::fit() {
    trackMinuit.DefineParameter(0, "intercept", 0., 0., 0., 0.);
    trackMinuit.DefineParameter(1, "slope", 0., 0., 0., 0.);
    trackMinuit.Command("MIGRAD");
    trackMinuit.GetParameter(0, fIntercept, fInterceptError);
    trackMinuit.GetParameter(1, fSlope, fSlopeError);
}