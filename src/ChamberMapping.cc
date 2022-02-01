#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "ChamberMapping.h"
#include "CsvReader.h"

ChamberMapping::ChamberMapping(std::string mappingFilePath) {
	fMappingFilePath = mappingFilePath;
}

int ChamberMapping::read() {
	CsvReader mappingReader(fMappingFilePath);
	if (mappingReader.read()<0) return -1;
	std::cout << "Mapping table:" << std::endl;

	int slot, oh, vfat, chamber;
	// iterate on rows:
	for (int irow=0; irow<mappingReader.getNRows(); irow++) {
		slot = std::stoi(mappingReader.getElement("slot", irow));
		oh = std::stoi(mappingReader.getElement("oh", irow));
		vfat = std::stoi(mappingReader.getElement("vfat", irow));
		chamber = std::stoi(mappingReader.getElement("chamber", irow));

		to_chamber[slot][oh][vfat] = chamber;
	}
	return 0;
}

void ChamberMapping::print() {
	std::cout << "Chamber mapping" << std::endl;
	for (auto chamber:to_chamber) std::cout << chamber << " ";
	std::cout << std::endl;
}
