#include <cassert>
#include <math.h>
#include <iostream>

#include "LinearFcn.h"

double LinearFcn::operator()(const std::vector<double>& par) const {
    assert(par.size() == 2);
    double chi2 = 0.;
    double linearFunctionValue = 0.;
    for(unsigned int n = 0; n < fMeas.size(); n++) {
        linearFunctionValue = par[0] + fPos[n]*par[1];
        chi2 += pow((linearFunctionValue - fMeas[n])/fMVar[n], 2);
    }
    return chi2;
}