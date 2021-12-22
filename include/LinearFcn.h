#include <vector>

#include "Minuit2/FCNBase.h"

#ifndef DEF_LINEARFCN
#define DEF_LINEARFCN

class LinearFcn : public ROOT::Minuit2::FCNBase {
    public:
        LinearFcn(const std::vector<double> &meas, const std::vector<double> &pos, const std::vector<double>& mvar):
        fMeas(meas), fPos(pos), fMVar(mvar), fErrorDef(1.) {}

        ~LinearFcn() {}

        virtual double Up() const {return fErrorDef;}
        virtual double operator()(const std::vector<double>&) const;

        std::vector<double> measurements() const {return fMeas;}
        std::vector<double> positions() const {return fPos;}
        std::vector<double> variances() const {return fMVar;}

        void setErrorDef(double def) {fErrorDef = def;}

    private:
        std::vector<double> fMeas;
        std::vector<double> fPos;
        std::vector<double> fMVar;
        double fErrorDef;
};

#endif