#ifndef ROOT_TMVA_ClassifierFactory
#define ROOT_TMVA_ClassifierFactory

/////////////////////////////////////////////////////////////////
///
/// Abstract ClassifierFactory template that handles arbitrary types
/// 
/// This template creates ClassifierFactory stores creator functors
/// to template parameter class. ClassifierFactory is a singelton class
/// which is explicitly deleted.
///
/// Source: Andrei Alexandrescu, Modern C++ Design
///
/////////////////////////////////////////////////////////////////

// C++
#include <map> 
#include <string>
#include <vector>

// Local
#include "TString.h"


namespace TMVA
{
   class IMethod;
   class DataSetInfo;
   
   class ClassifierFactory
   {       
   public:
      
      // typedef for functor that creates object of class IMethod
      typedef IMethod* (*Creator)(const TString& job, const TString& title,
                                  DataSetInfo& dsi, const TString& option ); 
      
   public:
      
      static ClassifierFactory& Instance();
      
      bool Register(const std::string &name, Creator creator); 

      bool Unregister(const std::string &name);

      IMethod* Create(const std::string &name,
                      const TString& job,
                      const TString& title,
                      DataSetInfo& dsi,
                      const TString& option); 

      IMethod* Create(const std::string &name,
                      DataSetInfo& dsi,
                      const TString& weightfile =""); 
      
      const std::vector<std::string> List() const;
 
      void Print() const;

   private:

      // must use Instance() method to access/create ClassifierFactory
      ClassifierFactory(){};
      ~ClassifierFactory(){};

      // ClassifierFactory is singleton and can not be copied
      // These two methods are private and not defined by design
      ClassifierFactory(const ClassifierFactory &);
      const ClassifierFactory& operator =(const ClassifierFactory &);
      
   private: 
      
      static ClassifierFactory *fgInstance;
      
      typedef std::map<std::string, Creator> CallMap; 
      
      CallMap fCalls;       
   };

}

/////////////////////////////////////////////////////////////////
///
/// for example
///
/// REGISTER_METHOD(Fisher)
/// 
/// expands to this code:
///
/// namespace
/// {
///    TMVA::IMethod* CreateMethod()
///    {
///       return (TMVA::IMethod*) new TMVA::MethodFisher;
///    }
///    bool RegisteredMethod = TMVA::ClassifierFactory<TMVA::MethodBase>::Instance(). 
///       Register("Method", CreateMethodFisher);
/// }
///
/////////////////////////////////////////////////////////////////

#define REGISTER_METHOD(CLASS)					\
   namespace								\
   {									\
      TMVA::IMethod* CreateMethod##CLASS(const TString& job, const TString& title, TMVA::DataSetInfo& dsi, const TString& option) \
      {									\
         if(job=="" && title=="") { \
            return (TMVA::IMethod*) new TMVA::Method##CLASS(dsi, option); \
         } else {  \
            return (TMVA::IMethod*) new TMVA::Method##CLASS(job, title, dsi, option); \
         }  \
      }  \
      bool RegisteredMethod = TMVA::ClassifierFactory::Instance().	\
	       Register(#CLASS, CreateMethod##CLASS);				\
      bool AddedTypeMapping = TMVA::Types::Instance().AddTypeMapping(TMVA::Types::k##CLASS, #CLASS); \
   }


#endif


