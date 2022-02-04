#include <cstdio>

class ChamberMapping {
    
    public:

	    ChamberMapping(std::string mappingFile);
	    
	    void print();

	    int to_chamber[4][4][24]; // slot, oh, vfat -> chamber
};