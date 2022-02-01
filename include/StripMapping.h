#include <cstdio>

class StripMapping {
    
    public:

	    std::string fMappingFilePath;

	    StripMapping(std::string mappingFile);
	    
	    int read();
	    void print();

	    int to_strip[24][128];
	    int to_eta[12];
};
