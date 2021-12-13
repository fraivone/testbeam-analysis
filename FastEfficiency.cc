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

#include "TestBeamMapping.h"
#include "GEMAMCEventFormat.h"

class GEMUnpacker
{
  public:
    GEMUnpacker(const std::string & ifilename, const std::string & isFedKit, const std::string & _ofilename) {
      ifile = ifilename;
      try {
        m_file = std::fopen(ifilename.c_str(), "rb");
      } catch (int e) {
        std::cout << "An exception occured. Exception code " << e << std::endl;
      }
      //ofilename = ifilename.substr(0, ifilename.size()-4);
      //ofilename += ".raw.root";
      ofilename = _ofilename;
      m_isFedKit = isFedKit;
    }

    ~GEMUnpacker() {
      if (m_file != NULL) std::fclose(m_file);
    }

    void unpack(const int max_events, std::map<std::pair<int, int>, TestBeamMapping*> mappings) {

      //if (max_events > 0) std::cout << "Unpacking " << max_events << " events" << std::endl;
      //else std::cout << "Unpacking all events" << std::endl; 

     TFile *hfile = new TFile(ofilename.c_str(),"RECREATE","GEM Raw ROOT");
     TTree outputtree("outputtree","outputtree");
     
     int nhits;
     int latency;
     int pulse_stretch;
     
     // raw hit variables
     std::vector<int> vecOH;
     std::vector<int> vecVFAT;
     std::vector<int> vecCH;
     
     // digi variables
     std::vector<int> vecDigiEta; // even for x, odd for y
     std::vector<int> vecDigiChamber; // 0 to 3 for trackers, 4 and 5 for GE21 and ME0 (not implemented yet)
     std::vector<int> vecDigiDirection; // 0 for x, 1 for y
     std::vector<int> vecDigiStrip; // 0 to 357

     // support variables
     int vfatId = 0;
     int fed = 1478; // TODO: add ME0
     int oh = 0;
     int eta = 0;
     int strip = 0;
     int chamber = 0;
     int direction = 0;

     outputtree.Branch("nhits", &nhits);
     outputtree.Branch("latency", &latency);
     outputtree.Branch("pulse_stretch", &pulse_stretch);
     outputtree.Branch("OH", &vecOH);
     outputtree.Branch("VFAT", &vecVFAT);
     outputtree.Branch("CH", &vecCH);

    // digi variable branches
     outputtree.Branch("digiEta", &vecDigiEta);
     outputtree.Branch("digiChamber", &vecDigiChamber);
     outputtree.Branch("digiDirection", &vecDigiDirection);
     outputtree.Branch("digiStrip", &vecDigiStrip);

     int n_evt=0;
     int n_signals = 0;

     while (true) {
        bool foundHit = false;
        if ((max_events>0) && (n_evt>max_events)) break;
         //std::cout << "Unpacking event " << n_evt << " " << foundHit << " " << n_signals << std::endl;
         // read and print FEROL headers
         if (m_isFedKit == "ferol") {
             std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             if (sz == 0 ) break;
             //printf("%016llX\n", m_word);
             std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             //printf("%016llX\n", m_word);
             std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             //printf("%016llX\n", m_word);
             // ferol headers read and printed, now read CDF header
             //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
         } else {
             std::size_t sz = std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             if (sz == 0 ) break;
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
        nhits=0;
        vecOH.clear();
        vecVFAT.clear();
        vecCH.clear();

        vecDigiEta.clear();
        vecDigiChamber.clear();
        vecDigiDirection.clear();
        vecDigiStrip.clear();

        vfatId = 0;
        oh = 0;

         std::fread(&m_word, 8, 1, m_file);
         //printf("AMC HEADER1\n");
         //printf("%016llX\n", m_word);
         m_amcEvent->setAMCheader1(m_word);
         std::fread(&m_word, 8, 1, m_file);
         //printf("AMC HEADER2\n");
         //printf("%016llX\n", m_word);
         m_amcEvent->setAMCheader2(m_word);
         std::fread(&m_word, sizeof(uint64_t), 1, m_file);
         m_amcEvent->setGEMeventHeader(m_word);

         latency = m_amcEvent->Latency();
         pulse_stretch = m_amcEvent->PULSE_STRETCH();
         //printf("GEM EVENT HEADER\n");
         //printf("%016llX\n", m_word);
         // fill the geb data here
         //std::cout << "GDcount = " << m_amcEvent->GDcount() << std::endl;
         for (unsigned short j = 0; j < m_amcEvent->GDcount() && !(foundHit); j++){
           //std::cout << "here1" << std::endl;
             GEBdata * m_gebdata = new GEBdata();
             std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             m_gebdata->setChamberHeader(m_word);
             //printf("GEM CHAMBER HEADER\n");
             //printf("%016llX\n", m_word);
             // fill the vfat data here
             //std::cout << "Number of VFAT words " << m_gebdata->Vwh() << std::endl;
             //std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             int m_nvb = m_gebdata->Vwh() / 3; // number of VFAT2 blocks. Eventually add here sanity check
             //printf("N vfat blocks %d\n",m_nvb);
             //printf("OH %d\n",m_gebdata->InputID());
             
             for (unsigned short k = 0; k < m_nvb && !(foundHit); k++){
                //std::cout << "here2" << std::endl;
                 VFATdata * m_vfatdata = new VFATdata();
                 // read 3 vfat block words, totaly 192 bits
                 std::fread(&m_word, sizeof(uint64_t), 1, m_file);
                 //printf("VFAT WORD 1 ");
                 //std::cout << std::bitset<64>(m_word) << std::endl;
                 //printf("%016llX\n", m_word);
                 //printf("%016llX\n", m_word >> 56);
                 //printf("%016llX\n", 0x3f);
                 //printf("%016llX\n", 0x3f & (m_word >> 56));
                 m_vfatdata->read_fw(m_word);
                 std::fread(&m_word, sizeof(uint64_t), 1, m_file);
                 //printf("VFAT WORD 2 ");
                 //std::cout << std::bitset<64>(m_word) << std::endl;
                 //printf("%016llX\n", m_word);
                 m_vfatdata->read_sw(m_word);
                 std::fread(&m_word, sizeof(uint64_t), 1, m_file);
                 //printf("VFAT WORD 3 ");
                 //std::cout << std::bitset<64>(m_word) << std::endl;
                 //printf("%016llX\n", m_word);
                 m_vfatdata->read_tw(m_word);

                vfatId = m_vfatdata->Pos();
                oh = m_gebdata->InputID();

                // auto hitMapping = mappings[std::make_pair(fed, oh)];
                // eta = hitMapping->to_eta[vfatId];
                // chamber = hitMapping->to_chamber[oh][vfatId];

                //std::cout << "here3" << std::endl;
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
                    if (oh==0) {
                      n_signals++;
                      foundHit = true;
                      //std::cout << "here" << std::endl;
                      break;
                    }
			              // vecCH.push_back(i);
                    // vecVFAT.push_back(vfatId);
                    // vecOH.push_back(oh);
                    // vecDigiEta.push_back(eta);
                    // vecDigiChamber.push_back(chamber);
                    // vecDigiDirection.push_back(direction);
                    // vecDigiStrip.push_back(hitMapping->to_strip[vfatId][i]);
                    nhits++;
                  }
                  if (m_vfatdata->msData() & (1LL << i)) {
                    if (oh==0) {
                      n_signals++;
                      foundHit = true;
                      //std::cout << "here" << std::endl;
                      break;
                    }
                    // vecCH.push_back(i+64);
                    // vecVFAT.push_back(vfatId);
                    // vecOH.push_back(oh);
                    // vecDigiEta.push_back(eta);
                    // vecDigiChamber.push_back(chamber);
                    // vecDigiDirection.push_back(direction);
                    // vecDigiStrip.push_back(hitMapping->to_strip[vfatId][i+64]);
                    nhits++;
                  }
                }
                delete m_vfatdata;
             }
             std::fread(&m_word, sizeof(uint64_t), 1, m_file);
             m_gebdata->setChamberTrailer(m_word);
             m_amcEvent->g_add(*m_gebdata);
             delete m_gebdata;
         }
         std::fread(&m_word, sizeof(uint64_t), 1, m_file);
         m_amcEvent->setGEMeventTrailer(m_word);
         std::fread(&m_word, sizeof(uint64_t), 1, m_file);
         //printf("AMC TRALIER\n");
         //printf("%016llX\n", m_word);
         m_amcEvent->setAMCTrailer(m_word);

         delete m_amcEvent;

        // apply mapping and create digis
        //std::cout << nhits << std::endl;
        /*for (int nHit=0; nHit<nhits; nHit++) {
          vecDigiEta.push_back(to_eta[vecVFAT[nHit]]);
          vecDigiStrip.push_back(to_strip[vecVFAT[nHit]][vecCH[nHit]]);
        }*/

         //GEMtree.Fill();
	      outputtree.Fill();
        n_evt++; 
         //if (n_evt>2) break;
       //std::cout << lscount << " " <<  mscount << std::endl;
       //std::cout << std::endl;
       endEvent:continue;
     }

    //  std::cout << std::endl;
    //  std::cout << std::endl;
     double efficiency = (double) n_signals/n_evt;
     std::cout << n_signals << " " << n_evt << " ";
     std::cout << ifile;
     std::cout << " " << efficiency << std::endl;
     //std::cout << std::endl;
     //std::cout << lscount << " " <<  mscount << std::endl;
     //outputtree.Write();
     hfile->Write();// Save file with tree
    }
private:
    std::FILE *m_file;
    std::string ifile;
    uint64_t m_word;
    uint32_t m_word32;
    uint64_t fw_;
    uint64_t sw_;
    uint64_t tw_;
    bool type;
    AMCEvent * m_AMCEvent;
    std::string ofilename;
    std::string m_isFedKit;
};
 
int main (int argc, char** argv)
{
  //std::cout << "[GEMUnpacker]: ---> Main()" << std::endl;
  if (argc<4) 
  {
    std::cout << "Please provide input filename and source type" << std::endl;
    std::cout << "Usage: RawToDigi ifile ferol(sdram) ofile [max_events]" << std::endl;
    return 0;
  }
  std::string ifile   = argv[1];
  std::string isFedKit = argv[2];
  std::string ofile   = argv[3];
  
  int max_events = -1;
  if (argc>4) max_events = atoi(argv[4]);

  TestBeamMapping trackerMapping("mapping/tracker_mapping.csv");
  TestBeamMapping ge21Mapping("mapping/ge21_mapping.csv");

  //std::cout << "Reading mapping files..." << std::endl;
  if (trackerMapping.read() < 0) {
	  //std::cout << "Error reading tracker mapping" << std::endl;
	  return -1;
  }
  if (ge21Mapping.read() < 0) {
	  //std::cout << "Error reading GE2/1 mapping" << std::endl;
	  return -1;
  }

  std::map<std::pair<int, int>, TestBeamMapping*> mappings = {
    {std::make_pair(1478, 0), &ge21Mapping},
    {std::make_pair(1478, 2), &trackerMapping},
    {std::make_pair(1478, 3), &trackerMapping},
  };

  /*
  std::cout << "Tracker mapping:" << std::endl;
  trackerMapping.print();

  std::cout << "GE2/1 mapping:" << std::endl;
  ge21Mapping.print();
  */

  GEMUnpacker * m_unpacker = new GEMUnpacker(ifile, isFedKit, ofile);
  m_unpacker->unpack(max_events, mappings);
  delete m_unpacker;
}
