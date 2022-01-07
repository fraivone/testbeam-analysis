#ifndef DEF_CORRECTIONS
#define DEF_CORRECTIONS

double mappingStart[4][2] = {
    { -44.75, +44.75 },
    { -44.75, +44.75 },
    { -44.75, +44.75 },
    { -44.75, +44.75 }
};
double mappingScale[4][2] = {
    { +1, -1 },
    { +1, -1 },
    { +1, -1 },
    { +1, -1 }
};
double mappingCorrections[4][2] = {
    { -0.269035, -2.06244 },
    { +0.292939, -0.207079 },
    { -0.332708, +0.577936 },
    { +0.0905448, +0.108215 }
};
double mappingPitch[4] = { 0.25, 0.25, 0.25, 0.25 };
double mappingPitchSqrt12[4] = { 0.07217, 0.07217, 0.07217, 0.07217 };

// geometry, starting from ge2/1
double zBari1 = -(697+254+294);
double zBari2 = -(254+294);
double zGe21 = 0;
double zMe0 = -294;
double zBari3 = 170;
double zBari4 = 170+697;

double zChamber[5] = { zBari1, zBari2, zBari3, zBari4, zGe21 };

// class Corrections {

//     public:

//         double fCorrection[4] = { -0.269035, +0.292939, -0.332708, +0.0905448 };
//         double fCorrectionY[4] = { -2.06244, -0.207079, +0.577936, +0.108215 };

//         Corrections() {}
// };

#endif