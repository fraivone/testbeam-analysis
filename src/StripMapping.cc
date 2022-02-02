#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "StripMapping.h"
#include "CsvReader.h"

StripMapping::StripMapping(std::string mappingFilePath) {
	fMappingFilePath = mappingFilePath;
}

int StripMapping::read() {
	CsvReader mappingReader(fMappingFilePath);
	if (mappingReader.read()<0) return -1;

	int vfatId, vfatChannel, eta, strip;
	// iterate on rows:
	for (int irow=0; irow<mappingReader.getNRows(); irow++) {
		// unused: oh = std::stoi(mappingRow[columnIndex["oh"]]);
		// unused: chamber = std::stoi(mappingRow[columnIndex["chamber"]]);
		vfatId = std::stoi(mappingReader.getElement("vfatId", irow));
		vfatChannel = std::stoi(mappingReader.getElement("vfatCh", irow));
		eta = std::stoi(mappingReader.getElement("iEta", irow));
		strip = std::stoi(mappingReader.getElement("strip", irow));

		to_eta[vfatId] = eta;
		to_strip[vfatId][vfatChannel] = strip;
	}
	return 0;
}

void StripMapping::print() {
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
