// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME dIafsdIcerndOchdIuserdIadIapelleccdIdqmdIunpackerdItestmIbeammIunpackerdIdOdOdIdItestmIbeammIunpackerdIdOdIsrcdIdicdIEventDict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// The generated code does not explicitly qualifies STL entities
namespace std {} using namespace std;

// Header files passed as explicit arguments
#include "Event.h"

// Header files passed via #pragma extra_include

namespace ROOT {
   static TClass *VFATdata_Dictionary();
   static void VFATdata_TClassManip(TClass*);
   static void *new_VFATdata(void *p = 0);
   static void *newArray_VFATdata(Long_t size, void *p);
   static void delete_VFATdata(void *p);
   static void deleteArray_VFATdata(void *p);
   static void destruct_VFATdata(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::VFATdata*)
   {
      ::VFATdata *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::VFATdata));
      static ::ROOT::TGenericClassInfo 
         instance("VFATdata", "GEMAMCEventFormat.h", 5,
                  typeid(::VFATdata), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &VFATdata_Dictionary, isa_proxy, 4,
                  sizeof(::VFATdata) );
      instance.SetNew(&new_VFATdata);
      instance.SetNewArray(&newArray_VFATdata);
      instance.SetDelete(&delete_VFATdata);
      instance.SetDeleteArray(&deleteArray_VFATdata);
      instance.SetDestructor(&destruct_VFATdata);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::VFATdata*)
   {
      return GenerateInitInstanceLocal((::VFATdata*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::VFATdata*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *VFATdata_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::VFATdata*)0x0)->GetClass();
      VFATdata_TClassManip(theClass);
   return theClass;
   }

   static void VFATdata_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *GEBdata_Dictionary();
   static void GEBdata_TClassManip(TClass*);
   static void *new_GEBdata(void *p = 0);
   static void *newArray_GEBdata(Long_t size, void *p);
   static void delete_GEBdata(void *p);
   static void deleteArray_GEBdata(void *p);
   static void destruct_GEBdata(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::GEBdata*)
   {
      ::GEBdata *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::GEBdata));
      static ::ROOT::TGenericClassInfo 
         instance("GEBdata", "GEMAMCEventFormat.h", 136,
                  typeid(::GEBdata), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &GEBdata_Dictionary, isa_proxy, 4,
                  sizeof(::GEBdata) );
      instance.SetNew(&new_GEBdata);
      instance.SetNewArray(&newArray_GEBdata);
      instance.SetDelete(&delete_GEBdata);
      instance.SetDeleteArray(&deleteArray_GEBdata);
      instance.SetDestructor(&destruct_GEBdata);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::GEBdata*)
   {
      return GenerateInitInstanceLocal((::GEBdata*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::GEBdata*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *GEBdata_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::GEBdata*)0x0)->GetClass();
      GEBdata_TClassManip(theClass);
   return theClass;
   }

   static void GEBdata_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *AMCEvent_Dictionary();
   static void AMCEvent_TClassManip(TClass*);
   static void *new_AMCEvent(void *p = 0);
   static void *newArray_AMCEvent(Long_t size, void *p);
   static void delete_AMCEvent(void *p);
   static void deleteArray_AMCEvent(void *p);
   static void destruct_AMCEvent(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::AMCEvent*)
   {
      ::AMCEvent *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::AMCEvent));
      static ::ROOT::TGenericClassInfo 
         instance("AMCEvent", "GEMAMCEventFormat.h", 264,
                  typeid(::AMCEvent), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &AMCEvent_Dictionary, isa_proxy, 4,
                  sizeof(::AMCEvent) );
      instance.SetNew(&new_AMCEvent);
      instance.SetNewArray(&newArray_AMCEvent);
      instance.SetDelete(&delete_AMCEvent);
      instance.SetDeleteArray(&deleteArray_AMCEvent);
      instance.SetDestructor(&destruct_AMCEvent);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::AMCEvent*)
   {
      return GenerateInitInstanceLocal((::AMCEvent*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::AMCEvent*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *AMCEvent_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::AMCEvent*)0x0)->GetClass();
      AMCEvent_TClassManip(theClass);
   return theClass;
   }

   static void AMCEvent_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static void *new_EventHeader(void *p = 0);
   static void *newArray_EventHeader(Long_t size, void *p);
   static void delete_EventHeader(void *p);
   static void deleteArray_EventHeader(void *p);
   static void destruct_EventHeader(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::EventHeader*)
   {
      ::EventHeader *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::EventHeader >(0);
      static ::ROOT::TGenericClassInfo 
         instance("EventHeader", ::EventHeader::Class_Version(), "Event.h", 32,
                  typeid(::EventHeader), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::EventHeader::Dictionary, isa_proxy, 4,
                  sizeof(::EventHeader) );
      instance.SetNew(&new_EventHeader);
      instance.SetNewArray(&newArray_EventHeader);
      instance.SetDelete(&delete_EventHeader);
      instance.SetDeleteArray(&deleteArray_EventHeader);
      instance.SetDestructor(&destruct_EventHeader);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::EventHeader*)
   {
      return GenerateInitInstanceLocal((::EventHeader*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::EventHeader*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

namespace ROOT {
   static void *new_Event(void *p = 0);
   static void *newArray_Event(Long_t size, void *p);
   static void delete_Event(void *p);
   static void deleteArray_Event(void *p);
   static void destruct_Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Event*)
   {
      ::Event *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TInstrumentedIsAProxy< ::Event >(0);
      static ::ROOT::TGenericClassInfo 
         instance("Event", ::Event::Class_Version(), "Event.h", 50,
                  typeid(::Event), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &::Event::Dictionary, isa_proxy, 4,
                  sizeof(::Event) );
      instance.SetNew(&new_Event);
      instance.SetNewArray(&newArray_Event);
      instance.SetDelete(&delete_Event);
      instance.SetDeleteArray(&deleteArray_Event);
      instance.SetDestructor(&destruct_Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Event*)
   {
      return GenerateInitInstanceLocal((::Event*)0);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Event*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));
} // end of namespace ROOT

//______________________________________________________________________________
atomic_TClass_ptr EventHeader::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *EventHeader::Class_Name()
{
   return "EventHeader";
}

//______________________________________________________________________________
const char *EventHeader::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventHeader*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int EventHeader::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::EventHeader*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *EventHeader::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventHeader*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *EventHeader::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::EventHeader*)0x0)->GetClass(); }
   return fgIsA;
}

//______________________________________________________________________________
atomic_TClass_ptr Event::fgIsA(0);  // static to hold class pointer

//______________________________________________________________________________
const char *Event::Class_Name()
{
   return "Event";
}

//______________________________________________________________________________
const char *Event::ImplFileName()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int Event::ImplFileLine()
{
   return ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
TClass *Event::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
TClass *Event::Class()
{
   if (!fgIsA.load()) { R__LOCKGUARD(gInterpreterMutex); fgIsA = ::ROOT::GenerateInitInstanceLocal((const ::Event*)0x0)->GetClass(); }
   return fgIsA;
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_VFATdata(void *p) {
      return  p ? new(p) ::VFATdata : new ::VFATdata;
   }
   static void *newArray_VFATdata(Long_t nElements, void *p) {
      return p ? new(p) ::VFATdata[nElements] : new ::VFATdata[nElements];
   }
   // Wrapper around operator delete
   static void delete_VFATdata(void *p) {
      delete ((::VFATdata*)p);
   }
   static void deleteArray_VFATdata(void *p) {
      delete [] ((::VFATdata*)p);
   }
   static void destruct_VFATdata(void *p) {
      typedef ::VFATdata current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::VFATdata

namespace ROOT {
   // Wrappers around operator new
   static void *new_GEBdata(void *p) {
      return  p ? new(p) ::GEBdata : new ::GEBdata;
   }
   static void *newArray_GEBdata(Long_t nElements, void *p) {
      return p ? new(p) ::GEBdata[nElements] : new ::GEBdata[nElements];
   }
   // Wrapper around operator delete
   static void delete_GEBdata(void *p) {
      delete ((::GEBdata*)p);
   }
   static void deleteArray_GEBdata(void *p) {
      delete [] ((::GEBdata*)p);
   }
   static void destruct_GEBdata(void *p) {
      typedef ::GEBdata current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::GEBdata

namespace ROOT {
   // Wrappers around operator new
   static void *new_AMCEvent(void *p) {
      return  p ? new(p) ::AMCEvent : new ::AMCEvent;
   }
   static void *newArray_AMCEvent(Long_t nElements, void *p) {
      return p ? new(p) ::AMCEvent[nElements] : new ::AMCEvent[nElements];
   }
   // Wrapper around operator delete
   static void delete_AMCEvent(void *p) {
      delete ((::AMCEvent*)p);
   }
   static void deleteArray_AMCEvent(void *p) {
      delete [] ((::AMCEvent*)p);
   }
   static void destruct_AMCEvent(void *p) {
      typedef ::AMCEvent current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::AMCEvent

//______________________________________________________________________________
void EventHeader::Streamer(TBuffer &R__b)
{
   // Stream an object of class EventHeader.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(EventHeader::Class(),this);
   } else {
      R__b.WriteClassBuffer(EventHeader::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_EventHeader(void *p) {
      return  p ? new(p) ::EventHeader : new ::EventHeader;
   }
   static void *newArray_EventHeader(Long_t nElements, void *p) {
      return p ? new(p) ::EventHeader[nElements] : new ::EventHeader[nElements];
   }
   // Wrapper around operator delete
   static void delete_EventHeader(void *p) {
      delete ((::EventHeader*)p);
   }
   static void deleteArray_EventHeader(void *p) {
      delete [] ((::EventHeader*)p);
   }
   static void destruct_EventHeader(void *p) {
      typedef ::EventHeader current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::EventHeader

//______________________________________________________________________________
void Event::Streamer(TBuffer &R__b)
{
   // Stream an object of class Event.

   if (R__b.IsReading()) {
      R__b.ReadClassBuffer(Event::Class(),this);
   } else {
      R__b.WriteClassBuffer(Event::Class(),this);
   }
}

namespace ROOT {
   // Wrappers around operator new
   static void *new_Event(void *p) {
      return  p ? new(p) ::Event : new ::Event;
   }
   static void *newArray_Event(Long_t nElements, void *p) {
      return p ? new(p) ::Event[nElements] : new ::Event[nElements];
   }
   // Wrapper around operator delete
   static void delete_Event(void *p) {
      delete ((::Event*)p);
   }
   static void deleteArray_Event(void *p) {
      delete [] ((::Event*)p);
   }
   static void destruct_Event(void *p) {
      typedef ::Event current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Event

namespace ROOT {
   static TClass *vectorlEunsignedsPchargR_Dictionary();
   static void vectorlEunsignedsPchargR_TClassManip(TClass*);
   static void *new_vectorlEunsignedsPchargR(void *p = 0);
   static void *newArray_vectorlEunsignedsPchargR(Long_t size, void *p);
   static void delete_vectorlEunsignedsPchargR(void *p);
   static void deleteArray_vectorlEunsignedsPchargR(void *p);
   static void destruct_vectorlEunsignedsPchargR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<unsigned char>*)
   {
      vector<unsigned char> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<unsigned char>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<unsigned char>", -2, "vector", 210,
                  typeid(vector<unsigned char>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEunsignedsPchargR_Dictionary, isa_proxy, 0,
                  sizeof(vector<unsigned char>) );
      instance.SetNew(&new_vectorlEunsignedsPchargR);
      instance.SetNewArray(&newArray_vectorlEunsignedsPchargR);
      instance.SetDelete(&delete_vectorlEunsignedsPchargR);
      instance.SetDeleteArray(&deleteArray_vectorlEunsignedsPchargR);
      instance.SetDestructor(&destruct_vectorlEunsignedsPchargR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<unsigned char> >()));

      ::ROOT::AddClassAlternate("vector<unsigned char>","std::vector<unsigned char, std::allocator<unsigned char> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<unsigned char>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEunsignedsPchargR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<unsigned char>*)0x0)->GetClass();
      vectorlEunsignedsPchargR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEunsignedsPchargR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEunsignedsPchargR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned char> : new vector<unsigned char>;
   }
   static void *newArray_vectorlEunsignedsPchargR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<unsigned char>[nElements] : new vector<unsigned char>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEunsignedsPchargR(void *p) {
      delete ((vector<unsigned char>*)p);
   }
   static void deleteArray_vectorlEunsignedsPchargR(void *p) {
      delete [] ((vector<unsigned char>*)p);
   }
   static void destruct_vectorlEunsignedsPchargR(void *p) {
      typedef vector<unsigned char> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<unsigned char>

namespace ROOT {
   static TClass *vectorlEVFATdatagR_Dictionary();
   static void vectorlEVFATdatagR_TClassManip(TClass*);
   static void *new_vectorlEVFATdatagR(void *p = 0);
   static void *newArray_vectorlEVFATdatagR(Long_t size, void *p);
   static void delete_vectorlEVFATdatagR(void *p);
   static void deleteArray_vectorlEVFATdatagR(void *p);
   static void destruct_vectorlEVFATdatagR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<VFATdata>*)
   {
      vector<VFATdata> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<VFATdata>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<VFATdata>", -2, "vector", 210,
                  typeid(vector<VFATdata>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEVFATdatagR_Dictionary, isa_proxy, 0,
                  sizeof(vector<VFATdata>) );
      instance.SetNew(&new_vectorlEVFATdatagR);
      instance.SetNewArray(&newArray_vectorlEVFATdatagR);
      instance.SetDelete(&delete_vectorlEVFATdatagR);
      instance.SetDeleteArray(&deleteArray_vectorlEVFATdatagR);
      instance.SetDestructor(&destruct_vectorlEVFATdatagR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<VFATdata> >()));

      ::ROOT::AddClassAlternate("vector<VFATdata>","std::vector<VFATdata, std::allocator<VFATdata> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<VFATdata>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEVFATdatagR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<VFATdata>*)0x0)->GetClass();
      vectorlEVFATdatagR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEVFATdatagR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEVFATdatagR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<VFATdata> : new vector<VFATdata>;
   }
   static void *newArray_vectorlEVFATdatagR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<VFATdata>[nElements] : new vector<VFATdata>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEVFATdatagR(void *p) {
      delete ((vector<VFATdata>*)p);
   }
   static void deleteArray_vectorlEVFATdatagR(void *p) {
      delete [] ((vector<VFATdata>*)p);
   }
   static void destruct_vectorlEVFATdatagR(void *p) {
      typedef vector<VFATdata> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<VFATdata>

namespace ROOT {
   static TClass *vectorlEGEBdatagR_Dictionary();
   static void vectorlEGEBdatagR_TClassManip(TClass*);
   static void *new_vectorlEGEBdatagR(void *p = 0);
   static void *newArray_vectorlEGEBdatagR(Long_t size, void *p);
   static void delete_vectorlEGEBdatagR(void *p);
   static void deleteArray_vectorlEGEBdatagR(void *p);
   static void destruct_vectorlEGEBdatagR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<GEBdata>*)
   {
      vector<GEBdata> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<GEBdata>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<GEBdata>", -2, "vector", 210,
                  typeid(vector<GEBdata>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEGEBdatagR_Dictionary, isa_proxy, 0,
                  sizeof(vector<GEBdata>) );
      instance.SetNew(&new_vectorlEGEBdatagR);
      instance.SetNewArray(&newArray_vectorlEGEBdatagR);
      instance.SetDelete(&delete_vectorlEGEBdatagR);
      instance.SetDeleteArray(&deleteArray_vectorlEGEBdatagR);
      instance.SetDestructor(&destruct_vectorlEGEBdatagR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<GEBdata> >()));

      ::ROOT::AddClassAlternate("vector<GEBdata>","std::vector<GEBdata, std::allocator<GEBdata> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<GEBdata>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEGEBdatagR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<GEBdata>*)0x0)->GetClass();
      vectorlEGEBdatagR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEGEBdatagR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEGEBdatagR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<GEBdata> : new vector<GEBdata>;
   }
   static void *newArray_vectorlEGEBdatagR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<GEBdata>[nElements] : new vector<GEBdata>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEGEBdatagR(void *p) {
      delete ((vector<GEBdata>*)p);
   }
   static void deleteArray_vectorlEGEBdatagR(void *p) {
      delete [] ((vector<GEBdata>*)p);
   }
   static void destruct_vectorlEGEBdatagR(void *p) {
      typedef vector<GEBdata> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<GEBdata>

namespace ROOT {
   static TClass *vectorlEAMCEventgR_Dictionary();
   static void vectorlEAMCEventgR_TClassManip(TClass*);
   static void *new_vectorlEAMCEventgR(void *p = 0);
   static void *newArray_vectorlEAMCEventgR(Long_t size, void *p);
   static void delete_vectorlEAMCEventgR(void *p);
   static void deleteArray_vectorlEAMCEventgR(void *p);
   static void destruct_vectorlEAMCEventgR(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const vector<AMCEvent>*)
   {
      vector<AMCEvent> *ptr = 0;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(vector<AMCEvent>));
      static ::ROOT::TGenericClassInfo 
         instance("vector<AMCEvent>", -2, "vector", 210,
                  typeid(vector<AMCEvent>), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &vectorlEAMCEventgR_Dictionary, isa_proxy, 0,
                  sizeof(vector<AMCEvent>) );
      instance.SetNew(&new_vectorlEAMCEventgR);
      instance.SetNewArray(&newArray_vectorlEAMCEventgR);
      instance.SetDelete(&delete_vectorlEAMCEventgR);
      instance.SetDeleteArray(&deleteArray_vectorlEAMCEventgR);
      instance.SetDestructor(&destruct_vectorlEAMCEventgR);
      instance.AdoptCollectionProxyInfo(TCollectionProxyInfo::Generate(TCollectionProxyInfo::Pushback< vector<AMCEvent> >()));

      ::ROOT::AddClassAlternate("vector<AMCEvent>","std::vector<AMCEvent, std::allocator<AMCEvent> >");
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const vector<AMCEvent>*)0x0); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *vectorlEAMCEventgR_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const vector<AMCEvent>*)0x0)->GetClass();
      vectorlEAMCEventgR_TClassManip(theClass);
   return theClass;
   }

   static void vectorlEAMCEventgR_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_vectorlEAMCEventgR(void *p) {
      return  p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<AMCEvent> : new vector<AMCEvent>;
   }
   static void *newArray_vectorlEAMCEventgR(Long_t nElements, void *p) {
      return p ? ::new((::ROOT::Internal::TOperatorNewHelper*)p) vector<AMCEvent>[nElements] : new vector<AMCEvent>[nElements];
   }
   // Wrapper around operator delete
   static void delete_vectorlEAMCEventgR(void *p) {
      delete ((vector<AMCEvent>*)p);
   }
   static void deleteArray_vectorlEAMCEventgR(void *p) {
      delete [] ((vector<AMCEvent>*)p);
   }
   static void destruct_vectorlEAMCEventgR(void *p) {
      typedef vector<AMCEvent> current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class vector<AMCEvent>

namespace {
  void TriggerDictionaryInitialization_EventDict_Impl() {
    static const char* headers[] = {
"Event.h",
0
    };
    static const char* includePaths[] = {
"/usr/include/root",
"/afs/cern.ch/user/a/apellecc/dqm/unpacker/test-beam-unpacker/..//test-beam-unpacker/include",
"/usr/include/root",
"/afs/cern.ch/user/a/apellecc/dqm/unpacker/test-beam-unpacker/",
0
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "EventDict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$GEMAMCEventFormat.h")))  __attribute__((annotate("$clingAutoload$Event.h")))  VFATdata;
class __attribute__((annotate("$clingAutoload$GEMAMCEventFormat.h")))  __attribute__((annotate("$clingAutoload$Event.h")))  GEBdata;
class __attribute__((annotate("$clingAutoload$GEMAMCEventFormat.h")))  __attribute__((annotate("$clingAutoload$Event.h")))  AMCEvent;
class __attribute__((annotate(R"ATTRDUMP(Event Header)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$Event.h")))  EventHeader;
class __attribute__((annotate(R"ATTRDUMP(Event structure)ATTRDUMP"))) __attribute__((annotate("$clingAutoload$Event.h")))  Event;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "EventDict dictionary payload"


#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
#include "Event.h"

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"AMCEvent", payloadCode, "@",
"Event", payloadCode, "@",
"EventHeader", payloadCode, "@",
"GEBdata", payloadCode, "@",
"VFATdata", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("EventDict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_EventDict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_EventDict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_EventDict() {
  TriggerDictionaryInitialization_EventDict_Impl();
}
