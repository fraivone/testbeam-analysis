#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "ChamberMapping.h"
#include "DataFrame.h"

ChamberMapping::ChamberMapping(std::string mappingFilePath) {
	DataFrame mappingDataFrame = DataFrame::fromCsv(mappingFilePath);

	int slot, oh, vfat, chamber;
	// iterate on rows:
	for (int irow=0; irow<mappingDataFrame.getNRows(); irow++) {
		slot = std::stoi(mappingDataFrame.getElement("slot", irow));
		oh = std::stoi(mappingDataFrame.getElement("oh", irow));
		vfat = std::stoi(mappingDataFrame.getElement("vfat", irow));
		chamber = std::stoi(mappingDataFrame.getElement("chamber", irow));

		to_chamber[slot][oh][vfat] = chamber;
	}
}

void ChamberMapping::print() {
	std::cout << "Chamber mapping" << std::endl;
	for (auto chamber:to_chamber) std::cout << chamber << " ";
	std::cout << std::endl;
}
