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
          for (auto ifilename:ifilenames){
          m_files.push_back(std::fopen(ifilename.c_str(), "rb"));
          }
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

    void setParameters(bool _verbose, bool _checkSyncronization) {
      verbose = _verbose;
      checkSyncronization = _checkSyncronization;
    }
    
    void PrintHEX2Binary(uint64_t m_word){
        unsigned long i; 
        i = 1UL<<(sizeof(m_word)*8-1);
        while(i>0){
            if(m_word&i)
                printf("1"); 
            else 
                printf("0"); 
            i >>= 1;
        }
    }

    int readEvent(int slot) {
      // slot = _slot;

      // read and print FEROL headers
      if (m_isFedKit == "ferol") {
        std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
        if (sz == 0 ) return -1; // end of file reached
        //printf("%016llX\n", m_word);
        std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
        //printf("%016llX\n", m_word);
        std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
        //printf("%016llX\n", m_word);
        // ferol headers read and printed, now read CDF header
        //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
      } else {
        std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
        if (sz == 0 ) return -1; // end of file reached
        //read and print "BADC0FFEEBADCAFE" and another artificial header
        //printf("%016llX\n", m_word);
        //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
        //printf("%016llX\n", m_word);
      }

      // Unpack AMC13 Header
      if (isAMC13){
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
          if(verbose){
              printf("CDFHeader %016llX\n", m_word);
              printf("--------------------------------------\n");
              printf("headerMarker1 = ");
              PrintHEX2Binary( (m_word >> 60) & 0xf);
              printf("\nEventType = ");
              PrintHEX2Binary( (m_word >> 56) & 0xf);
              printf("\nL1aID = %d\n", ((m_word >> 32) & 0xffffff) );
              printf("bxID = %d\n", ((m_word >> 20) & 0xfff) );
              printf("fedID = %d\n", ((m_word >> 8) & 0xfff) );
              printf("--------------------------------------\n");
              // Finish unpack of CDF
          }

          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));


          if(verbose){
              printf("\n\nAMC13 Header %016llX\n", m_word);
              printf("--------------------------------------\n");
              printf("NumberAMCs = %d\n", ((m_word >> 52) & 0xf) );
              printf("OrbitID = %d\n", ((m_word >> 4) & 0xffffffff) );
              printf("headerMarker2 = %d\n", ((m_word >> 0) & 0xf) );
              printf("--------------------------------------\n");
              // Finish unpack AMC13 header
          }
      
          // Unpacking now the AMC13 block, to be looped over n of amc (here only 1 AMC)
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
          
          if(verbose){
              printf("\n\nAMC13 Block %016llX\n", m_word);
              printf("--------------------------------------\n");
              printf("BlockSize = %d\n", ((m_word >> 32) & 0xffffff) );
              printf("AMC ID = %d\n", ((m_word >> 16) & 0xf) );
              printf("--------------------------------------\n");
              // Print unpacked AMC13 block
          }
      }// Finish unpacking AMC13 Header

      int AMC_slot = (int)((m_word >> 16) & 0xf);

      //     m_AMC13Event->addAMCheader(m_word);
      // }


      // Readout out AMC payloads
      AMCEvent * m_amcEvent = new AMCEvent();
      vfatId = 0;
      oh = 0;


      std::fread(&m_word, 8, 1, m_files.at(slot));
      //printf("AMC HEADER1\n");
      //printf("%016llX\n", m_word);
      m_amcEvent->setAMCheader1(m_word);
      std::fread(&m_word, 8, 1, m_files.at(slot));
      //printf("AMC HEADER2\n");
      //printf("%016llX\n", m_word);
      m_amcEvent->setAMCheader2(m_word);
      std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
      m_amcEvent->setGEMeventHeader(m_word);

      l1a = m_amcEvent->L1A();
      if (checkSyncronization) {
        for (int otherL1A:eventVecL1A) {
          if (l1a!=otherL1A) {
            if (verbose) {
              std::cout << "ERROR: different L1As in event: ";
              std::cout << l1a << " " << otherL1A << ", exiting..." << std::endl;
            }
            return 128; // l1a out of sync
          }
        }
        eventVecL1A.push_back(l1a);
      }
      latency = m_amcEvent->Latency();
      pulse_stretch = m_amcEvent->PULSE_STRETCH();

      //printf("GEM EVENT HEADER\n");
      //printf("%016llX\n", m_word);
      // fill the geb data here
      //std::cout << "GDcount = " << m_amcEvent->GDcount() << std::endl;
      for (unsigned short j = 0; j < m_amcEvent->GDcount(); j++) {
          GEBdata * m_gebdata = new GEBdata();
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
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
          for (unsigned short k = 0; k < m_nvb; k++) {
            VFATdata * m_vfatdata = new VFATdata();
            // read 3 vfat block words, totaly 192 bits
            std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
            //printf("VFAT WORD 1 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            //printf("%016llX\n", m_word >> 56);
            //printf("%016llX\n", 0x3f);
            //printf("%016llX\n", 0x3f & (m_word >> 56));
            m_vfatdata->read_fw(m_word);
            std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
            //printf("VFAT WORD 2 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            m_vfatdata->read_sw(m_word);
            std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
            //printf("VFAT WORD 3 ");
            //std::cout << std::bitset<64>(m_word) << std::endl;
            //printf("%016llX\n", m_word);
            m_vfatdata->read_tw(m_word);

            vfatId = m_vfatdata->Pos();
            chamber = chamberMapping->to_chamber[AMC_slot][oh][vfatId];
            if (stripMappings.count(chamber)==0) 
                {
                    std::cout << "Found slot = " << AMC_slot << "\tOH = " << oh << "\tVFATID = " << vfatId << " that should correspond to Chamber = " << chamber << std::endl;
                    std::cout << "However no strips have been mapped to this chamber "<< chamber << std::endl;
                    std::cin.get();
                    return 0;}
            StripMapping *stripMapping = stripMappings.at(chamber);

            if (verbose) {
              std::cout << "        " << AMC_slot << "\t" << oh << "\t" << vfatId;
              std::cout << "\t" << m_amcEvent->Onum() << "\t" << m_amcEvent->BX() << "\t" << l1a;
              std::cout << "\t" << chamber << std::endl;
            }

            direction = eta%2;
            for (int i=0;i<64;i++) {
              if (m_vfatdata->lsData() & (1LL << i)) {
                vecCh.push_back(i);
                vecVfat.push_back(vfatId);
                vecOh.push_back(oh);
                vecSlot.push_back(AMC_slot);
                eta = stripMapping->to_eta[vfatId][i];
                vecDigiEta.push_back(eta);
                vecDigiChamber.push_back(chamber);
                vecDigiDirection.push_back(direction);
                vecDigiStrip.push_back(stripMapping->to_strip[vfatId][i]);
                nhits++;
                hitsPerVFAT[vfatId]++;
              }
              if (m_vfatdata->msData() & (1LL << i)) {
                vecCh.push_back(i+64);
                vecVfat.push_back(vfatId);
                vecOh.push_back(oh);
                vecSlot.push_back(AMC_slot);
                eta = stripMapping->to_eta[vfatId][i+64];
                vecDigiEta.push_back(eta);
                vecDigiChamber.push_back(chamber);
                vecDigiDirection.push_back(direction);
                vecDigiStrip.push_back(stripMapping->to_strip[vfatId][i+64]);
                nhits++;
                hitsPerVFAT[vfatId]++;
              }
            }
            delete m_vfatdata;
          }
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
          m_gebdata->setChamberTrailer(m_word);
          m_amcEvent->g_add(*m_gebdata);
          delete m_gebdata;
      }
      std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
      m_amcEvent->setGEMeventTrailer(m_word);
      // printf("PulseStretch = ");
      // PrintHEX2Binary( (m_word >> 8));
      // printf("\n");
      std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
      m_amcEvent->setAMCTrailer(m_word);
      

      // Unpack AMC13 Trailer
      if(isAMC13){
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
          // CDF Trailer
          std::fread(&m_word, sizeof(uint64_t), 1, m_files.at(slot));
      }// Finish unpacking AMC13 trailer

      delete m_amcEvent;

      return 0;
    }    

    int unpack(const int max_events, std::map<int, StripMapping*> _stripMappings, ChamberMapping* _chamberMapping) {
      stripMappings = _stripMappings;
      chamberMapping = _chamberMapping;

      int unpackerStatus = 0;

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
      outputtree.Branch("hitspervfat", hitsPerVFAT,"hitsPerVFAT[24]/I");

      // digi variable branches
      outputtree.Branch("digiChamber", &vecDigiChamber);
      outputtree.Branch("digiEta", &vecDigiEta);
      outputtree.Branch("digiDirection", &vecDigiDirection);
      outputtree.Branch("digiStrip", &vecDigiStrip);

      int n_evt=0;
      // Loop over all files
      for (int slot=0; slot<m_files.size(); slot++) {
          // slot == file index. To be improved?
          while (true) {
              if ((max_events>0) && (n_evt>max_events)) break;
              if ( n_evt%1000==0 ) std::cout << "Unpacking event " << n_evt << "         \r";

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
              
              eventVecL1A.clear();
              for(int k=0; k<24; k++) hitsPerVFAT[k] = 0;
              if (verbose) std::cout<<"hitsPerVFAT[0]"<<"\t"<<hitsPerVFAT[0]<<std::endl;

              if (verbose) {
                  std::cout << "Event " << n_evt << std::endl;
                  std::cout << "    File " << m_files.at(slot) << std::endl;
                  std::cout << "        " << "slot\toh\tvfat\tOC\tBX\tL1A\teta\tchamber" << std::endl;
              }

              // read event from raw:
              readStatus = readEvent(slot);
              n_evt++;           
              if (readStatus<0) break; // end of file
              else if (readStatus==128) {
                  unpackerStatus = readStatus; // L1A out of sync
                  std::cout << "Found mismatching L1As in event " << n_evt << ", stopping..." << std::endl;
                  break;
              }
              if(verbose)
                  {
                      std::cout<<"PulseStretch\t"<<pulse_stretch<<std::endl;
                      std::cout<<"Latency"<<"\t"<<latency<<"\t{";
                      for (int i: vecVfat)
                          std::cout << i << ",";
                      std::cout<<"}"<<std::endl;
                  }
              outputtree.Fill();

          }
      }
      std::cout << std::endl;
      hfile->Write();
      return unpackerStatus;
    }



private:
    int readStatus = 0;
    int hitsPerVFAT[24];    
    /* branch and support variables: */
    int nhits;
    int latency;
    int pulse_stretch;
    int l1a;
    std::vector<int> eventVecL1A;
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
    //int slot = 0;
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

    std::map<int, StripMapping*> stripMappings;
    ChamberMapping *chamberMapping;
    bool verbose=false, checkSyncronization=false, isAMC13=false;
};
 
int main (int argc, char** argv) {
  std::cout << "Running GEM unpacker..." << std::endl;
  if (argc<3) 
  {
    std::cout << "Usage: RawToDigi ifile(s) ofile [--events max_events] [--geometry geometryname] [--format ferol/sdram] [--verbose] [--check-sync] [--isAMC13]" << std::endl;
    return 0;
  }
  std::vector<std::string> ifiles;
  std::string ofile;
  std::string isFedKit = "ferol";
  
  int max_events = -1;
  std::string geometry = "IntegrationStand";
  bool verbose = false;
  bool checkSyncronization = false;
  bool isAMC13 = false;
  bool isUnnamed = true;
  for (int iarg=1; iarg<argc; iarg++) {
    std::string arg = argv[iarg];
    if (arg[0]=='-') { // parse named parameters
      isUnnamed = false; // end of unnamed parameters
      if (arg=="--verbose") verbose = true;
      else if (arg=="--check-sync") checkSyncronization = true;
      if (arg=="--isAMC13") isAMC13 = true;
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

  std::cout << "Reading mapping files..." << std::endl;
  std::string mappingBaseDir = "mapping/"+geometry;
  StripMapping trackerStripMapping(mappingBaseDir+"/tracker_mapping.csv");
  StripMapping ge21StripMapping(mappingBaseDir+"/ge21_mapping.csv");
  StripMapping me0StripMapping(mappingBaseDir+"/me0_mapping.csv");
  StripMapping rectangularStripMapping(mappingBaseDir+"/20x10_mapping.csv");
  ChamberMapping chamberMapping(mappingBaseDir+"/chamber_mapping.csv");
  std::cout << "Mapping files ok." << std::endl;

  std::map<int, StripMapping*> stripMappings = {
    {0, &trackerStripMapping},
    {1, &trackerStripMapping},
    {2, &trackerStripMapping},
    {3, &trackerStripMapping},
    {4, &ge21StripMapping},
    {5, &me0StripMapping},
    {6, &rectangularStripMapping}
  };
  // if (verbose) {
  //     for (auto stripMapping:stripMappings) {
  //         std::cout << "Chamber " << stripMapping.first << std::endl;
  //         stripMapping.second->print();
  //     }

  //     chamberMapping.print();
  // }

  GEMUnpacker * m_unpacker = new GEMUnpacker(ifiles, isFedKit, ofile);
  m_unpacker->setParameters(verbose, checkSyncronization);
  int unpackerStatus = m_unpacker->unpack(max_events, stripMappings, &chamberMapping);
  delete m_unpacker;
  std::cout << "Output file saved to " << ofile << std::endl;

  return unpackerStatus;
}
