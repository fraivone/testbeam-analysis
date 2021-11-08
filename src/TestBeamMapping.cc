#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "TestBeamMapping.h"

TestBeamMapping::TestBeamMapping(std::string mappingFilePath) {
	fMappingFilePath = mappingFilePath;
}

int TestBeamMapping::read() {
	// parse csv mapping file
	std::string csvLine, buffer;
	std::ifstream mappingFile(fMappingFilePath);
	std::vector<std::string> mappingRow;
	int vfatId, vfatChannel, eta, strip;
	std::string columnNames[4] = { "vfatId", "vfatCh", "iEta", "strip" };
	std::map<std::string, int> columnIndex;
	if (mappingFile.is_open()) {
		for (int lineIndex=0; getline(mappingFile, csvLine); lineIndex++) {
			mappingRow.clear();
			std::stringstream lineStream(csvLine);
			while (getline(lineStream, buffer, ',')) {
				mappingRow.push_back(buffer);
			}

			if (lineIndex==0) { // parse header
				for (std::string columnName:columnNames) {
					auto it = std::find(mappingRow.begin(), mappingRow.end(), columnName);
					if (it != mappingRow.end()) columnIndex[columnName] = it - mappingRow.begin();
					else return -1;
				}
			} else { // parse mapping row
				vfatId = std::stoi(mappingRow[columnIndex["vfatId"]]);
				vfatChannel = std::stoi(mappingRow[columnIndex["vfatCh"]]);
				eta = std::stoi(mappingRow[columnIndex["iEta"]]);
				strip = std::stoi(mappingRow[columnIndex["strip"]]);

				to_eta[vfatId] = eta;
				to_strip[vfatId][vfatChannel] = strip;
			}
		}
		mappingFile.close();
		return 0;
	}
	else return -1;
}

void TestBeamMapping::print() {
	std::cout << "Eta mapping" << std::endl;
	for (auto eta:to_eta) std::cout << eta << " ";
	std::cout << std::endl;

	std::cout << "Strip mapping" << std::endl;
	for (int i=0; i<12; i++) {
		for (int j=0; j<128; j++) std::cout << to_strip[i][j] << " ";
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
