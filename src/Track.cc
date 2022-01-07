#include <iostream>
#include <vector>
#include <math.h>

#include "Minuit2/MnUserParameters.h"
#include "Minuit2/MnPrint.h"
#include "Minuit2/MnMigrad.h"
#include "Minuit2/FunctionMinimum.h"

#include "Rechit.h"
#include "Track.h"
#include "LinearFcn.h"

Track::Track() {}

void Track::addRechit(Rechit rechit) {
    fRechits.push_back(rechit);
}

void Track::clear() {
    fRechits.clear();
}

std::vector<double> Track::getRechitCenters() {
    std::vector<double> centers;
    for (auto rechit:fRechits) {
        centers.push_back(rechit.getGlobalCenter());
    }
    return centers;
}

std::vector<double> Track::getRechitZ() {
    std::vector<double> z;
    for (auto rechit:fRechits) {
        z.push_back(rechit.getGlobalZ());
    }
    return z;
}

std::vector<double> Track::getRechitErrors() {
    std::vector<double> errors;
    for (auto rechit:fRechits) {
        errors.push_back(rechit.getError());
    }
    return errors;
}

void Track::fit() {
    LinearFcn chi2Fcn(getRechitCenters(), getRechitZ(), getRechitErrors());
    
    ROOT::Minuit2::MnUserParameters initialPars;
    initialPars.Add("intercept", 0., 0.1);
    initialPars.Add("slope", 0., 0.1);
    ROOT::Minuit2::MnMigrad migrad(chi2Fcn, initialPars);
    ROOT::Minuit2::FunctionMinimum min = migrad();

    fIntercept = migrad.Value("intercept");
    fSlope = migrad.Value("slope");
    fInterceptError = migrad.Error("intercept");
    fSlopeError = migrad.Error("slope");
    fChi2 = min.Fval();
}

double Track::propagate(double z) {
    return fIntercept + z*fSlope;
}

double Track::propagationError(double z) {
    return sqrt(pow(fInterceptError, 2) + pow(z*fSlopeError, 2));
}