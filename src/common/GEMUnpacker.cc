#include <cstdio>
#include <iostream>
#include <cstdint>
#include <vector>
#include <array>
#include <bitset>
//#include "GEMAMC13EventFormat.h"
//#include "TestBeamMapping.h"
#if defined(__CINT__) && !defined(__MAKECINT__)
    #include "libEvent.so"
#else
    #include "Event.h"
#endif
#include <TFile.h>
#include <TTree.h>

int to_chamber[4][12] = {
    {-1, -1, -1, -1, -2, -1, -2, -1, -2, -2, -2, -2},
    {-3, -3, -3, -3, -4, -3, -4, -3, -4, -4, -4, -4},
    {1, 1, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0},
    {3, 3, 3, 3, 2, 3, 2, 3, 2, 2, 2, 2}
};
int to_strip[12][128] = {
    {147, 146, 148, 149, 145, 144, 150, 151, 143, 142, 152, 153, 141, 140, 154, 155, 139, 138, 156, 157, 137, 136, 158, 159, 135, 134, 160, 161, 133, 132, 163, 162, 131, 130, 164, 165, 129, 128, 166, 167, 127, 126, 168, 169, 125, 124, 170, 171, 123, 49, 172, 173, 52, 53, 174, 175, 56, 57, 176, 177, 60, 61, 178, 179, 64, 65, 180, 181, 68, 69, 182, 183, 72, 73, 184, 185, 76, 236, 186, 187, 235, 234, 188, 189, 233, 232, 190, 191, 231, 230, 192, 193, 229, 228, 194, 195, 227, 226, 196, 197, 225, 224, 198, 199, 223, 222, 200, 201, 221, 220, 203, 202, 219, 218, 205, 204, 217, 216, 207, 206, 215, 214, 209, 208, 213, 212, 211, 210},
    {262, 261, 263, 264, 260, 259, 265, 266, 258, 257, 267, 268, 256, 255, 269, 270, 254, 253, 271, 272, 252, 251, 273, 274, 250, 249, 275, 276, 248, 247, 278, 277, 246, 245, 279, 280, 244, 243, 281, 282, 242, 241, 283, 284, 240, 239, 285, 286, 238, 237, 287, 288, 52, 53, 289, 290, 56, 57, 291, 292, 60, 61, 293, 294, 358, 357, 295, 296, 356, 355, 297, 298, 354, 353, 299, 300, 352, 351, 301, 302, 350, 349, 303, 304, 348, 347, 305, 306, 346, 345, 307, 308, 344, 343, 309, 310, 342, 341, 311, 312, 340, 339, 313, 314, 338, 337, 315, 316, 336, 335, 318, 317, 334, 333, 320, 319, 332, 331, 322, 321, 330, 329, 324, 323, 328, 327, 326, 325},
    {32, 31, 33, 34, 30, 29, 35, 36, 28, 27, 37, 38, 26, 25, 39, 40, 24, 23, 41, 42, 22, 21, 43, 44, 20, 19, 45, 46, 18, 17, 48, 47, 16, 15, 49, 50, 14, 13, 51, 52, 12, 11, 53, 54, 10, 9, 55, 56, 8, 7, 57, 58, 6, 5, 59, 60, 4, 3, 61, 62, 2, 1, 63, 64, 64, 65, 65, 66, 68, 69, 67, 68, 72, 73, 69, 70, 122, 121, 71, 72, 120, 119, 73, 74, 118, 117, 75, 76, 116, 115, 77, 78, 114, 113, 79, 80, 112, 111, 81, 82, 110, 109, 83, 84, 108, 107, 85, 86, 106, 105, 88, 87, 104, 103, 90, 89, 102, 101, 92, 91, 100, 99, 94, 93, 98, 97, 96, 95},
    {262, 261, 263, 264, 260, 259, 265, 266, 258, 257, 267, 268, 256, 255, 269, 270, 254, 253, 271, 272, 252, 251, 273, 274, 250, 249, 275, 276, 248, 247, 278, 277, 246, 245, 279, 280, 244, 243, 281, 282, 242, 241, 283, 284, 240, 239, 285, 286, 238, 237, 287, 288, 52, 53, 289, 290, 56, 57, 291, 292, 60, 61, 293, 294, 358, 357, 295, 296, 356, 355, 297, 298, 354, 353, 299, 300, 352, 351, 301, 302, 350, 349, 303, 304, 348, 347, 305, 306, 346, 345, 307, 308, 344, 343, 309, 310, 342, 341, 311, 312, 340, 339, 313, 314, 338, 337, 315, 316, 336, 335, 318, 317, 334, 333, 320, 319, 332, 331, 322, 321, 330, 329, 324, 323, 328, 327, 326, 325},
    {32, 31, 33, 34, 30, 29, 35, 36, 28, 27, 37, 38, 26, 25, 39, 40, 24, 23, 41, 42, 22, 21, 43, 44, 20, 19, 45, 46, 18, 17, 48, 47, 16, 15, 49, 50, 14, 13, 51, 52, 12, 11, 53, 54, 10, 9, 55, 56, 8, 7, 57, 58, 6, 5, 59, 60, 4, 3, 61, 62, 2, 1, 63, 64, 64, 65, 65, 66, 68, 69, 67, 68, 72, 73, 69, 70, 122, 121, 71, 72, 120, 119, 73, 74, 118, 117, 75, 76, 116, 115, 77, 78, 114, 113, 79, 80, 112, 111, 81, 82, 110, 109, 83, 84, 108, 107, 85, 86, 106, 105, 88, 87, 104, 103, 90, 89, 102, 101, 92, 91, 100, 99, 94, 93, 98, 97, 96, 95},
    {147, 146, 148, 149, 145, 144, 150, 151, 143, 142, 152, 153, 141, 140, 154, 155, 139, 138, 156, 157, 137, 136, 158, 159, 135, 134, 160, 161, 133, 132, 163, 162, 131, 130, 164, 165, 129, 128, 166, 167, 127, 126, 168, 169, 125, 124, 170, 171, 123, 49, 172, 173, 52, 53, 174, 175, 56, 57, 176, 177, 60, 61, 178, 179, 64, 65, 180, 181, 68, 69, 182, 183, 72, 73, 184, 185, 76, 236, 186, 187, 235, 234, 188, 189, 233, 232, 190, 191, 231, 230, 192, 193, 229, 228, 194, 195, 227, 226, 196, 197, 225, 224, 198, 199, 223, 222, 200, 201, 221, 220, 203, 202, 219, 218, 205, 204, 217, 216, 207, 206, 215, 214, 209, 208, 213, 212, 211, 210},
    {147, 146, 148, 149, 145, 144, 150, 151, 143, 142, 152, 153, 141, 140, 154, 155, 139, 138, 156, 157, 137, 136, 158, 159, 135, 134, 160, 161, 133, 132, 163, 162, 131, 130, 164, 165, 129, 128, 166, 167, 127, 126, 168, 169, 125, 124, 170, 171, 123, 49, 172, 173, 52, 53, 174, 175, 56, 57, 176, 177, 60, 61, 178, 179, 64, 65, 180, 181, 68, 69, 182, 183, 72, 73, 184, 185, 76, 236, 186, 187, 235, 234, 188, 189, 233, 232, 190, 191, 231, 230, 192, 193, 229, 228, 194, 195, 227, 226, 196, 197, 225, 224, 198, 199, 223, 222, 200, 201, 221, 220, 203, 202, 219, 218, 205, 204, 217, 216, 207, 206, 215, 214, 209, 208, 213, 212, 211, 210},
    {32, 31, 33, 34, 30, 29, 35, 36, 28, 27, 37, 38, 26, 25, 39, 40, 24, 23, 41, 42, 22, 21, 43, 44, 20, 19, 45, 46, 18, 17, 48, 47, 16, 15, 49, 50, 14, 13, 51, 52, 12, 11, 53, 54, 10, 9, 55, 56, 8, 7, 57, 58, 6, 5, 59, 60, 4, 3, 61, 62, 2, 1, 63, 64, 64, 65, 65, 66, 68, 69, 67, 68, 72, 73, 69, 70, 122, 121, 71, 72, 120, 119, 73, 74, 118, 117, 75, 76, 116, 115, 77, 78, 114, 113, 79, 80, 112, 111, 81, 82, 110, 109, 83, 84, 108, 107, 85, 86, 106, 105, 88, 87, 104, 103, 90, 89, 102, 101, 92, 91, 100, 99, 94, 93, 98, 97, 96, 95},
    {262, 261, 263, 264, 260, 259, 265, 266, 258, 257, 267, 268, 256, 255, 269, 270, 254, 253, 271, 272, 252, 251, 273, 274, 250, 249, 275, 276, 248, 247, 278, 277, 246, 245, 279, 280, 244, 243, 281, 282, 242, 241, 283, 284, 240, 239, 285, 286, 238, 237, 287, 288, 52, 53, 289, 290, 56, 57, 291, 292, 60, 61, 293, 294, 358, 357, 295, 296, 356, 355, 297, 298, 354, 353, 299, 300, 352, 351, 301, 302, 350, 349, 303, 304, 348, 347, 305, 306, 346, 345, 307, 308, 344, 343, 309, 310, 342, 341, 311, 312, 340, 339, 313, 314, 338, 337, 315, 316, 336, 335, 318, 317, 334, 333, 320, 319, 332, 331, 322, 321, 330, 329, 324, 323, 328, 327, 326, 325},
    {32, 31, 33, 34, 30, 29, 35, 36, 28, 27, 37, 38, 26, 25, 39, 40, 24, 23, 41, 42, 22, 21, 43, 44, 20, 19, 45, 46, 18, 17, 48, 47, 16, 15, 49, 50, 14, 13, 51, 52, 12, 11, 53, 54, 10, 9, 55, 56, 8, 7, 57, 58, 6, 5, 59, 60, 4, 3, 61, 62, 2, 1, 63, 64, 64, 65, 65, 66, 68, 69, 67, 68, 72, 73, 69, 70, 122, 121, 71, 72, 120, 119, 73, 74, 118, 117, 75, 76, 116, 115, 77, 78, 114, 113, 79, 80, 112, 111, 81, 82, 110, 109, 83, 84, 108, 107, 85, 86, 106, 105, 88, 87, 104, 103, 90, 89, 102, 101, 92, 91, 100, 99, 94, 93, 98, 97, 96, 95},
    {262, 261, 263, 264, 260, 259, 265, 266, 258, 257, 267, 268, 256, 255, 269, 270, 254, 253, 271, 272, 252, 251, 273, 274, 250, 249, 275, 276, 248, 247, 278, 277, 246, 245, 279, 280, 244, 243, 281, 282, 242, 241, 283, 284, 240, 239, 285, 286, 238, 237, 287, 288, 52, 53, 289, 290, 56, 57, 291, 292, 60, 61, 293, 294, 358, 357, 295, 296, 356, 355, 297, 298, 354, 353, 299, 300, 352, 351, 301, 302, 350, 349, 303, 304, 348, 347, 305, 306, 346, 345, 307, 308, 344, 343, 309, 310, 342, 341, 311, 312, 340, 339, 313, 314, 338, 337, 315, 316, 336, 335, 318, 317, 334, 333, 320, 319, 332, 331, 322, 321, 330, 329, 324, 323, 328, 327, 326, 325},
    {147, 146, 148, 149, 145, 144, 150, 151, 143, 142, 152, 153, 141, 140, 154, 155, 139, 138, 156, 157, 137, 136, 158, 159, 135, 134, 160, 161, 133, 132, 163, 162, 131, 130, 164, 165, 129, 128, 166, 167, 127, 126, 168, 169, 125, 124, 170, 171, 123, 49, 172, 173, 52, 53, 174, 175, 56, 57, 176, 177, 60, 61, 178, 179, 64, 65, 180, 181, 68, 69, 182, 183, 72, 73, 184, 185, 76, 236, 186, 187, 235, 234, 188, 189, 233, 232, 190, 191, 231, 230, 192, 193, 229, 228, 194, 195, 227, 226, 196, 197, 225, 224, 198, 199, 223, 222, 200, 201, 221, 220, 203, 202, 219, 218, 205, 204, 217, 216, 207, 206, 215, 214, 209, 208, 213, 212, 211, 210}
};
int to_eta[12] = {3, 3, 3, 4, 2, 4, 2, 4, 2, 1, 1, 1};

class GEMUnpacker
{
  public:
    GEMUnpacker(const std::string & ifilename, const std::string & isFedKit, const std::string & _ofilename)
    {
      try {
        m_file = std::fopen(ifilename.c_str(), "rb");
      }
      catch (int e)
      {
        std::cout << "An exception occured. Exception code " << e << std::endl;
      }
      //ofilename = ifilename.substr(0, ifilename.size()-4);
      //ofilename += ".raw.root";
      ofilename = _ofilename;
      m_isFedKit = isFedKit;
    }
    ~GEMUnpacker()
    {
      if (m_file != NULL) std::fclose(m_file);
    }
    void unpack(const int max_events)
    {

      if (max_events > 0) std::cout << "Unpacking " << max_events << " events" << std::endl;
      else std::cout << "Unpacking all events" << std::endl; 

     TFile *hfile = new TFile(ofilename.c_str(),"RECREATE","GEM Raw ROOT");
     TTree GEMtree("GEMtree","A Tree with GEM Events");
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
     int oh = 0;
     int eta = 0;
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

     Event *ev = new Event(); 
     GEMtree.Branch("GEMEvents", &ev);
     long long int n = 0;
     int n_evt=0;

     int mscount = 0;
     int lscount = 0;

     while (true) {
        if ((max_events>0) && (n_evt>max_events)) break;
         if ( n_evt%1000==0 ) std::cout << "Unpacking event " << n_evt << "         \r";
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
         for (unsigned short j = 0; j < m_amcEvent->GDcount(); j++){
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
             
             for (unsigned short k = 0; k < m_nvb; k++){
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

                 
                 //m_gebdata->v_add(*m_vfatdata);
                //std::cout << m_vfatdata->Pos() << " lsd " << std::bitset<64>(m_vfatdata->lsData()) << std::endl;
                //std::cout << m_vfatdata->Pos() << " msd " << std::bitset<64>(m_vfatdata->msData()) << std::endl;
                //uint64_t lsdata = uint64_t(VFATsecond{sw_}.lsData1) << 48 | VFATthird{tw_}.lsData2;
                //uint64_t msdata = uint64_t(VFATfirst{fw_}.msData1) << 48 | VFATsecond{sw_}.msData2;
                
                // if(m_vfatdata->lsData()){
                // //if(lsdata){
                //   for (int i=0;i<64;i++){
                //     if(m_vfatdata->lsData() & (1LL << i)){
                //       vecCH.push_back(i);
                //       //std::cout << m_vfatdata->Pos() << std::endl;
                //       vecVFAT.push_back(m_vfatdata->Pos());
                //       vecOH.push_back(m_gebdata->InputID());
                //       nhits++;
                //       lscount++;
                //     }
                //   }
                // }
                // if(m_vfatdata->msData()){
                // //} else if(msdata){
                //   for (int i=0;i<64;i++){
                //     if(m_vfatdata->msData() & (1LL << i)){
                //       vecCH.push_back(i+64);
                //       vecVFAT.push_back(m_vfatdata->Pos());
                //       //std::cout << m_vfatdata->Pos() << std::endl;
                //       vecOH.push_back(m_gebdata->InputID());
                //       nhits++;
                //       mscount++;
                //     }
                //   }
                // }

                vfatId = m_vfatdata->Pos();
                oh = m_gebdata->InputID();
                eta = to_eta[vfatId];
                chamber = to_chamber[oh][vfatId];
                direction = eta%2;
                for (int i=0;i<64;i++) {
                  if (m_vfatdata->lsData() & (1LL << i)) {
                    vecVFAT.push_back(vfatId);
                    vecOH.push_back(oh);
                    vecDigiEta.push_back(eta);
                    vecDigiChamber.push_back(chamber);
                    vecDigiDirection.push_back(direction);
                    vecDigiStrip.push_back(to_strip[vfatId][i]);
                    nhits++;
                  }
                  if (m_vfatdata->msData() & (1LL << i)) {
                    vecCH.push_back(i+64);
                    vecVFAT.push_back(vfatId);
                    vecOH.push_back(oh);
                    vecDigiEta.push_back(eta);
                    vecDigiChamber.push_back(chamber);
                    vecDigiDirection.push_back(direction);
                    vecDigiStrip.push_back(to_strip[vfatId][i+64]);
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

         
         ev->Build(true);
         // ev->SetHeader(m_AMC13Event->LV1_id(), 0, 0);
         ev->addAMCEvent(*m_amcEvent);
         delete m_amcEvent;

        // apply mapping and create digis
        //std::cout << nhits << std::endl;
        /*for (int nHit=0; nHit<nhits; nHit++) {
          vecDigiEta.push_back(to_eta[vecVFAT[nHit]]);
          vecDigiStrip.push_back(to_strip[vecVFAT[nHit]][vecCH[nHit]]);
        }*/

         //GEMtree.Fill();
	      outputtree.Fill();
         ev->Clear();
        n_evt++; 
         //if (n_evt>2) break;
       //std::cout << lscount << " " <<  mscount << std::endl;
       //std::cout << std::endl;

     }

     std::cout << std::endl;
     std::cout << std::endl;

     //std::cout << std::endl;
     //std::cout << lscount << " " <<  mscount << std::endl;
     outputtree.Write();
     hfile->Write();// Save file with tree
    }
private:
    std::FILE *m_file;
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
  std::cout << "[GEMUnpacker]: ---> Main()" << std::endl;
  if (argc<4) 
  {
    std::cout << "Please provide input filename and source type" << std::endl;
    std::cout << "Usage: <path>/unpacker ifile ferol(sdram) ofile" << std::endl;
    return 0;
  }
  std::string ifile   = argv[1];
  std::string isFedKit = argv[2];
  std::string ofile   = argv[3];
  
  int max_events = -1;
  if (argc>4) max_events = atoi(argv[4]);

  GEMUnpacker * m_unpacker = new GEMUnpacker(ifile, isFedKit, ofile);
  m_unpacker->unpack(max_events);
  delete m_unpacker;
}
