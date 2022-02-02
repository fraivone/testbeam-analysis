#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "CsvReader.h"

CsvReader::CsvReader(std::string path) {
	fPath = path;
}

int CsvReader::read() {
	// parse csv mapping file
	std::string csvLine, buffer;
	std::ifstream csvFile(fPath);
	std::vector<std::string> mappingRow;

	if (csvFile.is_open()) {
		for (int lineIndex=0; getline(csvFile, csvLine); lineIndex++) {
			mappingRow.clear();

			std::stringstream lineStream(csvLine);
			// split line by separator
			while (getline(lineStream, buffer, ',')) {
				mappingRow.push_back(buffer);
			}

			if (lineIndex==0) { // parse header
				fColumnNames = mappingRow;
				// for (auto colName:fColumnNames) {
				// 	fElements[colName] = std::vector<T>();
				// }
			} else { // parse mapping row
				for (int icol=0; icol<mappingRow.size(); icol++) {
					// std::stringstream convertToT(mappingRow[icol]);
					// T value;
					// convertToT >> value;
					// fElements[fColumnNames[icol]].push_back(value);
					fElements[fColumnNames[icol]].push_back(mappingRow[icol]);
				}
			}
		}
		csvFile.close();
		return 0;
	}
	else return -1;
}

void CsvReader::print() {
	for (auto colName:fColumnNames) std::cout << colName << "\t";
	std::cout << std::endl;

	int minRows = getNRows();
	if (minRows>10) minRows = 10;
	for (int irow=0; irow<minRows; irow++) {
		for (auto colName:fColumnNames) std::cout << fElements[colName][irow] << "\t";
		std::cout << std::endl;
	}
}