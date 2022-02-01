#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <utility>
#include <bitset>
//#include "GEMAMC13EventFormat.h"
#include <TFile.h>
#include <TTree.h>

#include "StripMapping.h"
#include "ChamberMapping.h"
#include "GEMAMCEventFormat.h"

class GEMUnpacker {
  public:
    GEMUnpacker(const std::vector<std::string> ifilenames, const std::string isFedKit, const std::string _ofilename) {
      try {
        for (auto ifilename:ifilenames)
          m_files.push_back(std::fopen(ifilename.c_str(), "rb"));
      } catch (int e) {
        std::cout << "An exception occured. Exception code " << e << std::endl;
      }
      ofilename = _ofilename;
      m_isFedKit = isFedKit;
    }

    ~GEMUnpacker() {
      for (auto file:m_files)
        if (file != NULL) std::fclose(file);
    }

    bool readEvent(std::FILE *file, int _slot) {
      slot = _slot;

      // read and print FEROL headers
      if (m_isFedKit == "ferol") {
        std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, file);
        if (sz == 0 ) return false;
        //printf("%016llX\n", m_word);
        std::fread(&m_word, sizeof(uint64_t), 1, file);
        //printf("%016llX\n", m_word);
        std::fread(&m_word, sizeof(uint64_t), 1, file);
        //printf("%016llX\n", m_word);
        // ferol headers read and printed, now read CDF header
        //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
      } else {
        std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, file);
        if (sz == 0 ) return false;
        //read and print "BADC0FFEEBADCAFE" and another artificial header
        //printf("%016llX\n", m_word);
        //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
        //printf("%016llX\n", m_word);
      }

      // m_AMC13Event = new AMC13Event();
      // //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
      // //printf("%016llX\n", m_word);
      // m_AMC13Event->setCDFHeader(m_word);
      // std::fread(&m_word, sizeof(uint64_t), 1, m_file);
      // //printf("%016llX\n", m_word);
      // m_AMC13Event->setAMC13header(m_word);
      // //printf("%016llX\n", m_word);
      // //std::cout << "n_AMC = " << m_AMC13Event->nAMC() << std::endl;
      // // Readout out AMC headers
      // for (unsigned short i = 0; i < m_AMC13Event->nAMC(); i++){
      //     std::fread(&m_word, sizeof(uint64_t), 1, m_file);
      //     //printf("%016llX\n", m_word);
      //     m_AMC13Event->addAMCheader(m_word);
      // }

      // Readout out AMC payloads
      AMCEvent * m_amcEvent = new AMCEvent();
      vfatId = 0;
      oh = 0;

      std::fread(&m_word, 8, 1, file);
      //printf("AMC HEADER1\n");
      //printf("%016llX\n", m_word);
      m_amcEvent->setAMCheader1(m_word);
      std::fread(&m_word, 8, 1, file);
      //printf("AMC HEADER2\n");
      //printf("%016llX\n", m_word);
      m_amcEvent->setAMCheader2(m_word);
      std::fread(&m_word, sizeof(uint64_t), 1, file);
      m_amcEvent->setGEMeventHeader(m_word);

      latency = m_amcEvent->Latency();
      pulse_stretch = m_amcEvent->PULSE_STRETCH();
      //printf("GEM EVENT HEADER\n");
      //printf("%016llX\n", m_word);
      // fill the geb data here
      //std::cout << "GDcount = " << m_amcEvent->GDcount() << std::endl;
      for (unsigned short j = 0; j < m_amcEvent->GDcount(); j++){
          GEBdata * m_gebdata = new GEBdata();
          std::fread(&m_word, sizeof(uint64_t), 1, file);
          m_gebdata->setChamberHeader(m_word);
          //printf("GEM CHAMBER HEADER\n");
          //printf("%016llX\n", m_word);
          // fill the vfat data here
          //std::cout << "Number of VFAT words " << m_gebdata->Vwh() << std::endl;
          //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
          int m_nvb = m_gebdata->Vwh() / 3; // number of VFAT2 blocks. Eventually add here sanity check
          //printf("N vfat blocks %d\n",m_nvb);
          //printf("OH %d\n",m_gebdata->InputID());
          
          oh = m_gebdata->InputID();
          for (unsigned short k = 0; k < m_nvb; k++){
            VFATdata * m_vfatdata = new VFATdata();
            // read 3 vfat block words, totaly 192 bits
            std::fread(&m_word, sizeof(uint64_t), 1, file);
            //printf("VFAT WORD 1 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            //printf("%016llX\n", m_word >> 56);
            //printf("%016llX\n", 0x3f);
            //printf("%016llX\n", 0x3f & (m_word >> 56));
            m_vfatdata->read_fw(m_word);
            std::fread(&m_word, sizeof(uint64_t), 1, file);
            //printf("VFAT WORD 2 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            m_vfatdata->read_sw(m_word);
            std::fread(&m_word, sizeof(uint64_t), 1, file);
            //printf("VFAT WORD 3 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            m_vfatdata->read_tw(m_word);

            vfatId = m_vfatdata->Pos();
            auto hitMapping = stripMappings[{slot, oh}];
            eta = hitMapping->to_eta[vfatId];
            chamber = chamberMapping->to_chamber[slot][oh][vfatId];
            //chamber = hitMapping->to_chamber[oh][vfatId]; // old, not working

            if (verbose) {
              std::cout << "        " << slot << " " << oh << " " << vfatId;
              std::cout << "    " << m_amcEvent->Onum() << " " << m_amcEvent->BX() << " " << m_amcEvent->L1A();
              std::cout << "    " << eta << " " << chamber << std::endl;
            }
            /*if (oh>0) {
              eta = mappingTracker.to_eta[vfatId];
              chamber = mapping.to_chamber_tracker[oh][vfatId];
            } else {
              eta = mapping.to_eta_ge21[vfatId];
              chamber = mapping.to_chamber_ge21[oh][vfatId];
            }*/
            
            direction = eta%2;
            for (int i=0;i<64;i++) {
              if (m_vfatdata->lsData() & (1LL << i)) {
                vecCh.push_back(i);
                vecVfat.push_back(vfatId);
                vecOh.push_back(oh);
                vecSlot.push_back(slot);
                vecDigiEta.push_back(eta);
                vecDigiChamber.push_back(chamber);
                vecDigiDirection.push_back(direction);
                vecDigiStrip.push_back(hitMapping->to_strip[vfatId][i]);
                nhits++;
              }
              if (m_vfatdata->msData() & (1LL << i)) {
                vecCh.push_back(i+64);
                vecVfat.push_back(vfatId);
                vecOh.push_back(oh);
                vecSlot.push_back(slot);
                vecDigiEta.push_back(eta);
                vecDigiChamber.push_back(chamber);
                vecDigiDirection.push_back(direction);
                vecDigiStrip.push_back(hitMapping->to_strip[vfatId][i+64]);
                nhits++;
              }
            }
            delete m_vfatdata;
          }
          std::fread(&m_word, sizeof(uint64_t), 1, file);
          m_gebdata->setChamberTrailer(m_word);
          m_amcEvent->g_add(*m_gebdata);
          delete m_gebdata;
      }
      std::fread(&m_word, sizeof(uint64_t), 1, file);
      m_amcEvent->setGEMeventTrailer(m_word);
      std::fread(&m_word, sizeof(uint64_t), 1, file);
      //printf("AMC TRALIER\n");
      //printf("%016llX\n", m_word);
      m_amcEvent->setAMCTrailer(m_word);

      delete m_amcEvent;
      return true;
    }    

    void unpack(const int max_events, std::map<std::array<int, 2>, StripMapping*> _stripMappings, ChamberMapping* _chamberMapping, bool _verbose) {
      stripMappings = _stripMappings;
      chamberMapping = _chamberMapping;
      verbose = _verbose;

      if (max_events > 0) std::cout << "Unpacking " << max_events << " events" << std::endl;
      else std::cout << "Unpacking all events" << std::endl; 

      TFile *hfile = new TFile(ofilename.c_str(), "RECREATE", "GEM Raw ROOT");
      TTree outputtree("outputtree", "outputtree");

      outputtree.Branch("nhits", &nhits);
      outputtree.Branch("latency", &latency);
      outputtree.Branch("pulse_stretch", &pulse_stretch);
      outputtree.Branch("slot", &vecSlot);
      outputtree.Branch("OH", &vecOh);
      outputtree.Branch("VFAT", &vecVfat);
      outputtree.Branch("CH", &vecCh);

      // digi variable branches
      outputtree.Branch("digiChamber", &vecDigiChamber);
      outputtree.Branch("digiEta", &vecDigiEta);
      outputtree.Branch("digiDirection", &vecDigiDirection);
      outputtree.Branch("digiStrip", &vecDigiStrip);

      int n_evt=0;

      while (true) {
        if ((max_events>0) && (n_evt>max_events)) break;
        if ( (!verbose) && n_evt%1000==0 ) std::cout << "Unpacking event " << n_evt << "         \r";

        // reset all branch variables:
        nhits=0;
        vecSlot.clear();
        vecOh.clear();
        vecVfat.clear();
        vecCh.clear();

        vecDigiEta.clear();
        vecDigiChamber.clear();
        vecDigiDirection.clear();
        vecDigiStrip.clear();

        // read event from raw:
        if (verbose) std::cout << "Event " << n_evt << std::endl;
        for (slot=0; slot<m_files.size(); slot++) {
          // slot == file index. To be improved?
          if (verbose) std::cout << "    File " << m_files.at(slot) << std::endl;
          if (!readEvent(m_files.at(slot), slot)) break;
        }
        outputtree.Fill();
        n_evt++; 
      }
      std::cout << std::endl;
      hfile->Write();
    }
private:
    /* branch and support variables: */
    int nhits;
    int latency;
    int pulse_stretch;
    // raw variables
    std::vector<int> vecSlot;
    std::vector<int> vecOh;
    std::vector<int> vecVfat;
    std::vector<int> vecCh;
    // digi variables
    std::vector<int> vecDigiEta; // even for x, odd for y
    std::vector<int> vecDigiChamber; // 0 to 3 for trackers, 4 for GE21, 5 for ME0
    std::vector<int> vecDigiDirection; // 0 for x, 1 for y
    std::vector<int> vecDigiStrip; // 0 to 357
    // support variables
    int vfatId = 0;
    int slot = 0;
    int oh = 0;
    int eta = 0;
    int strip = 0;
    int chamber = 0;
    int direction = 0;

    /* raw data variables: */
    std::vector<std::FILE *> m_files;
    uint64_t m_word;
    uint32_t m_word32;
    uint64_t fw_;
    uint64_t sw_;
    uint64_t tw_;
    bool type;
    AMCEvent * m_AMCEvent;
    std::string ofilename;
    std::string m_isFedKit;

    std::map<std::array<int, 2>, StripMapping*> stripMappings;
    ChamberMapping *chamberMapping;
    bool verbose;
};
 
int main (int argc, char** argv) {
  std::cout << "Running GEM unpacker..." << std::endl;
  if (argc<4) 
  {
    std::cout << "Usage: RawToDigi ifile(s) ofile [--events max_events] [--format ferol/sdram]" << std::endl;
    return 0;
  }
  std::vector<std::string> ifiles;
  std::string ofile;
  std::string isFedKit = "ferol";
  
  int max_events = -1;
  bool verbose = false;
  std::string geometry = "oct2021";
  bool isUnnamed = true;
  for (int iarg=1; iarg<argc; iarg++) {
    std::string arg = argv[iarg];
    if (arg[0]=='-') { // parse named parameters
      isUnnamed = false; // end of unnamed parameters
      if (arg=="--verbose") verbose = true;
      else if (arg=="--events") max_events = atoi(argv[iarg+1]);
      else if (arg=="--geometry") geometry = argv[iarg+1];
    } else if (isUnnamed) { // unnamed parameters
      if (iarg+1==argc || argv[iarg+1][0]=='-') ofile = arg;
      else ifiles.push_back(arg);
    }
  }
  std::cout << "ifiles ";
  for (auto s:ifiles) std::cout << s << " ";
  std::cout << std::endl;
  std::cout << "ofile " << ofile << std::endl;

  std::string mappingBaseDir = "mapping/"+geometry;
  StripMapping trackerStripMapping(mappingBaseDir+"/tracker_mapping.csv");
  StripMapping ge21StripMapping(mappingBaseDir+"/ge21_mapping.csv");
  StripMapping me0StripMapping(mappingBaseDir+"/me0_mapping.csv");
  ChamberMapping chamberMapping(mappingBaseDir+"/chamber_mapping.csv");

  std::cout << "Reading mapping files..." << std::endl;
  if (trackerStripMapping.read() < 0) {
	  std::cout << "Error reading tracker mapping" << std::endl;
	  return -1;
  }
  if (ge21StripMapping.read() < 0) {
	  std::cout << "Error reading GE2/1 mapping" << std::endl;
	  return -1;
  }
  if (me0StripMapping.read() < 0) {
	  std::cout << "Error reading ME0 mapping" << std::endl;
	  return -1;
  }
  if (me0StripMapping.read() < 0) {
	  std::cout << "Error reading chamber mapping" << std::endl;
	  return -1;
  }
  std::map<std::array<int, 2>, StripMapping*> stripMappings = {
    // multi-index (slot, oh)
    {{0, 0}, &ge21StripMapping},
    {{0, 2}, &trackerStripMapping},
    {{0, 3}, &trackerStripMapping},
    {{1, 0}, &me0StripMapping},
  };
  std::cout << "Mapping files ok." << std::endl;

  GEMUnpacker * m_unpacker = new GEMUnpacker(ifiles, isFedKit, ofile);
  m_unpacker->unpack(max_events, stripMappings, &chamberMapping, verbose);
  delete m_unpacker;
}
