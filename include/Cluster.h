#include "Digi.h"

#ifndef DEF_CLUSTER
#define DEF_CLUSTER

class Cluster {

    public:

        int fOh, fEta;
        int fFirst, fLast;

        Cluster() {}
        Cluster(int oh, int eta, int first, int last);

        bool isNeighbour(int strip);
        void extend(int strip);
        double getCenter();
        double getSize();

        int getOh();
        int getEta();
        int getFirst();
        int getLast();

        static std::vector<Cluster> fromDigis(std::vector<Digi> digis);

        int getChamber() { return (fOh-2)*2 + (fEta-1)/2; }
        int getDirection() { return (fEta+1) % 2; }
    
        // std::ostream& operator<<(std::ostream &os) {
        //     return os << fOh << "." << fEta << "." << fFirst << "." << fLast;
        // }
        void print() {
            std::cout << fOh << "oh." << fEta << "eta." << fFirst << "first." << fLast << "last" << std::endl;
        }
};

#endif