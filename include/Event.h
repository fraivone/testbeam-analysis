#ifndef ROOT_Event
#define ROOT_Event

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Event                                                                //
//                                                                      //
// Description of the GEM event                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// some pre-compiler voodoo, CINT...
#ifdef __APPLE__ 
typedef char __signed;
#include <_types/_uint8_t.h> 
#include <_types/_uint16_t.h>
#include <_types/_uint32_t.h> 
#include <_types/_uint64_t.h> 
#else 
#include <stdint.h> 
#endif

#include "TObject.h"
#include "TClonesArray.h"
#include "TRefArray.h"
#include "TRef.h"
#include "TH1.h"
#include "TBits.h"
#include "TMath.h"
#include "GEMAMCEventFormat.h"

class EventHeader {

    private:
       Int_t   fEvtNum;
       Int_t   fRun;
       Int_t   fDate;
    
    public:
       EventHeader() : fEvtNum(0), fRun(0), fDate(0) { }
       virtual ~EventHeader() { }
       void   Set(Int_t i, Int_t r, Int_t d) { fEvtNum = i; fRun = r; fDate = d; }
       Int_t  GetEvtNum() const { return fEvtNum; }
       Int_t  GetRun() const { return fRun; }
       Int_t  GetDate() const { return fDate; }
    
       ClassDef(EventHeader,1)              //Event Header
};

class Event : public TObject {

    private:
        EventHeader    fEvtHdr;
        
        std::vector<AMCEvent> famcs;      // 

        bool fisEventGood;

    public:
        Event();
        virtual ~Event();
        void SetHeader(Int_t i, Int_t run, Int_t date);
        int GetEventNumber(){return fEvtHdr.GetEvtNum();}
        void Build(bool isEventGood_);
        void addAMCEvent(const AMCEvent &amcevent){famcs.push_back(amcevent);}
        void Clear();

        std::vector<AMCEvent> amcs(){ return famcs;}

        ClassDef(Event,2)               //Event structure
};

#endif
