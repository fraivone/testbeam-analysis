#include<vector>

#include "Track.h"
#include "Rechit2D.h"

#ifndef DEF_TRACK2D
#define DEF_TRACK2D

class Track2D {

    public:

        Track2D() {}

        Track fTrackX, fTrackY;

        void addRechit(Rechit2D);
        void clear();
        void fit();
};

#endif