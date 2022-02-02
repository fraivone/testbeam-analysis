#include <cstdio>

class ChamberMapping {
    
    public:

	    std::string fMappingFilePath;

	    ChamberMapping(std::string mappingFile);
	    
	    int read();
	    void print();

	    int to_chamber[4][4][24]; // slot, oh, vfat -> chamber
};