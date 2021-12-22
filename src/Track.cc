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
        std::cout << "CHI2 is " << chi2 << std::endl;
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
    trackMinuit.SetObjectFit(this);
    trackMinuit.SetFCN(linearFunctionChi2);
    trackMinuit.DefineParameter(0, "intercept", 0., 0., 0., 0.);
    trackMinuit.DefineParameter(1, "slope", 0., 0., 0., 0.);
    
    double arglist[10];
    arglist[0] = 0.5;
    int ierflg = 0;
    trackMinuit.mnexcm("SET ERR",arglist,1,ierflg);
    trackMinuit.mnexcm("MIGRAD",arglist,0,ierflg);
    trackMinuit.mnexcm("MINOS",arglist,0,ierflg);
    //trackMinuit.Command("MIGRAD");
    //trackMinuit.Command("MIGRAD");
    trackMinuit.GetParameter(0, fIntercept, fInterceptError);
    trackMinuit.GetParameter(1, fSlope, fSlopeError);
}

double Track::propagate(double z) {
    return fIntercept + z*fSlope;
}

double Track::propagationError(double z) {
    return sqrt(pow(fInterceptError, 2) + pow(z*fSlopeError, 2));
}