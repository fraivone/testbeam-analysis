#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>

#include "StripMapping.h"
#include "DataFrame.h"

StripMapping::StripMapping(std::string mappingFilePath) {
	DataFrame mappingDataFrame = DataFrame::fromCsv(mappingFilePath);

    // populate mapping with zeros
    for (int i=0; i<24; i++) {
		for (int j=0; j<128; j++)
            to_strip[i][j] = 0;
	}

	int vfatId, vfatChannel, eta, strip;
	// iterate on rows:
	for (int irow=0; irow<mappingDataFrame.getNRows(); irow++) {
		// unused: oh = std::stoi(mappingRow[columnIndex["oh"]]);
		// unused: chamber = std::stoi(mappingRow[columnIndex["chamber"]]);
		vfatId = std::stoi(mappingDataFrame.getElement("vfatId", irow));
		vfatChannel = std::stoi(mappingDataFrame.getElement("vfatCh", irow));
		eta = std::stoi(mappingDataFrame.getElement("iEta", irow));
		strip = std::stoi(mappingDataFrame.getElement("strip", irow));

		to_eta[vfatId][vfatChannel] = eta;
		to_strip[vfatId][vfatChannel] = strip;
	}
}

void StripMapping::print() {
	std::cout << "Eta mapping" << std::endl;

	std::cout << "Strip mapping" << std::endl;
    std::cout << "vfat\tchannel\tiEta\tstrip" << std::endl;
	for (int i=0; i<12; i++) {
		for (int j=0; j<128; j++)
            std::cout << i << "\t" << j << "\t" << to_strip[i][j] << "\t" << to_eta[i][j] << std::endl;
	}
	std::cout << std::endl;
}
