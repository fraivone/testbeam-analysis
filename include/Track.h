#include<vector>

#include "Rechit.h"

#ifndef DEF_TRACK
#define DEF_TRACK

class Track {

    public:

        std::vector<Rechit> fRechits;

        Track() {}

        void addRechit(Rechit);
};

#endif