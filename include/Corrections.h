#ifndef DEF_CORRECTIONS
#define DEF_CORRECTIONS

double corrections[2][4] = {
    { -0.269035, +0.292939, -0.332708, +0.0905448 },
    { -2.06244, -0.207079, +0.577936, +0.108215 }
};

double mappingStart[2] = { -44.75, +44.75 };
double mappingScale[2] = { +1, -1 };

// geometry, starting from ge2/1
double zBari1 = -(697+254+294);
double zBari2 = -(254+294);
double zMe0 = -294;
double zBari3 = 170;
double zBari4 = 170+697;

double zChamber[4] = { zBari1, zBari2, zBari3, zBari4 };

// class Corrections {

//     public:

//         double fCorrection[4] = { -0.269035, +0.292939, -0.332708, +0.0905448 };
//         double fCorrectionY[4] = { -2.06244, -0.207079, +0.577936, +0.108215 };

//         Corrections() {}
// };

#endif