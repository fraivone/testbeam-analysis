#include "Digi.h"

Digi::Digi(int oh, int eta, int strip) {
    fOh = oh;
    fEta = eta;
    fStrip = strip;
}

int Digi::getOh() { return fOh; }
int Digi::getEta() { return fEta; }
int Digi::getStrip() { return fStrip; }