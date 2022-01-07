#include <iostream>
#include <vector>
#include <math.h>

#include "Track2D.h"
#include "Rechit2D.h"

void Track2D::addRechit(Rechit2D rechit) {
    fTrackX.addRechit(rechit.fRechitX);
    fTrackY.addRechit(rechit.fRechitY);
}

void Track2D::clear() {
    fTrackX.clear();
    fTrackY.clear();
}

void Track2D::fit() {
    fTrackX.fit();
    fTrackY.fit();
}

Hit Track2D::propagate(DetectorGeometry *detector) {
    return Hit(
        detector,
        propagateX(detector), propagateY(detector), detector->getPositionZ(),
        propagationErrorX(detector), propagationErrorY(detector), 0.
    );
}