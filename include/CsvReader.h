#include <cstdio>

/*
    Parse csv file to map of vectors of type T
*/

class CsvReader {
    
    public:

	    std::string fPath;

	    CsvReader(std::string path);
	    
	    int read();
	    void print();

		int getNRows() { return fElements[fColumnNames[0]].size(); }
		std::string getElement(std::string column, int row) { return fElements[column][row]; }

	    std::vector<std::string> fColumnNames;
	    std::map<std::string, std::vector<std::string>> fElements;
};
