#include <iostream>
#include <vector>
#include <math.h>

#include "Rechit.h"
#include "Track.h"

Track::addRechit(Rechit rechit) {
    fRechits.push_back(rechit);
}