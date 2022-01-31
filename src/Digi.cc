#include "Digi.h"

Digi::Digi(int slot, int oh, int eta, int strip) {
    fSlot = slot;
    fOh = oh;
    fEta = eta;
    fStrip = strip;
}

int Digi::getSlot() { return fSlot; }
int Digi::getOh() { return fOh; }
int Digi::getEta() { return fEta; }
int Digi::getStrip() { return fStrip; }