#include <cstdio>

class StripMapping {
    
    public:

	    StripMapping(std::string mappingFile);
	    
	    int read();
	    void print();

	    int to_strip[24][128];
	    int to_eta[12];
	
	private: 
	    std::string fMappingFilePath;
};
