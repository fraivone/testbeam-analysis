#include "Digi.h"

Digi::Digi(int chamber, int eta, int strip) {
    fChamber = chamber;
    fEta = eta;
    fStrip = strip;
}

int Digi::getChamber() { return fChamber; }
int Digi::getEta() { return fEta; }
int Digi::getStrip() { return fStrip; }