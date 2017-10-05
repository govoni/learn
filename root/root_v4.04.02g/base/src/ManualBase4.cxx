//
// File generated by utils\src\rootcint_tmp.exe at Wed Jan 12 09:24:21 2005.
// Do NOT change. Changes will be lost next time file is generated
//

#include "RConfig.h"
#if !defined(R__ACCESS_IN_SYMBOL)
//Break the privacy of classes -- Disabled for the moment
#define private public
#define protected public
#endif

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

#include "ManualBase4.h"
#include "TClass.h"
#include "TBuffer.h"
#include "TStreamerInfo.h"
#include "TMemberInspector.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"

#include "TCollectionProxy.h"

namespace ROOT {
   namespace Shadow {
   } // Of namespace ROOT::Shadow
} // Of namespace ROOT

/********************************************************
* base/src/ManualBase4.cxx
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************/

#ifdef G__MEMTEST
#undef malloc
#undef free
#endif

extern "C" void G__cpp_reset_tagtableManualBase4();

extern "C" void G__set_cpp_environmentManualBase4() {
  G__add_compiledheader("base/inc/TROOT.h");
  G__add_compiledheader("base/inc/TMemberInspector.h");
  G__add_compiledheader("TDirectory.h");
  G__cpp_reset_tagtableManualBase4();
}
class G__basedIsrcdIManualBase4dOcxx_tag {};

void* operator new(size_t size,G__basedIsrcdIManualBase4dOcxx_tag* p) {
  if(p && G__PVOID!=G__getgvp()) return((void*)p);
#ifndef G__ROOT
  return(malloc(size));
#else
  return(::operator new(size));
#endif
}

/* dummy, for exception */
#ifdef G__EH_DUMMY_DELETE
void operator delete(void *p,G__basedIsrcdIManualBase4dOcxx_tag* x) {
  if((long)p==G__getgvp() && G__PVOID!=G__getgvp()) return;
#ifndef G__ROOT
  free(p);
#else
  ::operator delete(p);
#endif
}
#endif

static void G__operator_delete(void *p) {
  if((long)p==G__getgvp() && G__PVOID!=G__getgvp()) return;
#ifndef G__ROOT
  free(p);
#else
  ::operator delete(p);
#endif
}

void G__DELDMY_basedIsrcdIManualBase4dOcxx() { G__operator_delete(0); }

extern "C" int G__cpp_dllrevManualBase4() { return(30051515); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* TDirectory */
#include "ManualBase4Body.h"

/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncManualBase4 {
 public:
  G__Sizep2memfuncManualBase4() {p=&G__Sizep2memfuncManualBase4::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncManualBase4::*p)();
};

size_t G__get_sizep2memfuncManualBase4()
{
  G__Sizep2memfuncManualBase4 a;
  G__setsizep2memfunc((int)a.sizep2memfunc());
  return((size_t)a.sizep2memfunc());
}


/*********************************************************
* virtual base class offset calculation interface
*********************************************************/

   /* Setting up class inheritance */

/*********************************************************
* Inheritance information setup/
*********************************************************/
extern "C" void G__cpp_setup_inheritanceManualBase4() {

   /* Setting up class inheritance */
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableManualBase4() {

   /* Setting up typedef entry */
   G__search_typename2("Int_t",105,-1,0,
-1);
   G__setnewtype(-1,"Signed integer 4 bytes (int)",0);
   G__search_typename2("Option_t",99,-1,256,
-1);
   G__setnewtype(-1,"Option string (const char)",0);
   G__search_typename2("vector<TStreamerInfo*>",117,G__get_linked_tagnum(&G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("_Ranit<TStreamerInfo*,difference_type,const_pointer,const_reference>",117,G__get_linked_tagnum(&G__ManualBase4LN__RanitlETStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,G__get_linked_tagnum(&G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR));
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("reverse_iterator<iterator>",117,G__get_linked_tagnum(&G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR),0,G__get_linked_tagnum(&G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR));
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>::iterator_category,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>::value_type,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>::difference_type,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>::pointer,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>::reference>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<random_access_iterator_tag,TStreamerInfo*>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<random_access_iterator_tag,TStreamerInfo*>vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator::difference_type>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<random_access_iterator_tag,TStreamerInfo*>vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator::difference_type>TStreamerInfo**>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("reverse_iterator<const_iterator>",117,G__get_linked_tagnum(&G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLconst_iteratorgR),0,G__get_linked_tagnum(&G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR));
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>::iterator_category,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>::value_type,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>::difference_type,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>::pointer,iterator_traits<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>::reference>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<random_access_iterator_tag,TStreamerInfo*>long>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
   G__search_typename2("iterator<random_access_iterator_tag,TStreamerInfo*>long>TStreamerInfo**>",117,G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name: v4-04-02g $:$Id: ManualBase4.cxx,v 1.4 2005/01/12 17:59:52 brun Exp $",0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */
extern "C" void G__cpp_setup_memvarManualBase4() {
}
/***********************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
***********************************************************/

/*********************************************************
* Member function information setup for each class
*********************************************************/
static void G__setup_memfuncTDirectory(void) {
   /* TDirectory */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__ManualBase4LN_TDirectory));
   G__memfunc_setup("WriteObject",1122,G__ManualBase4_134_5_8,105,-1,G__defined_typename("Int_t"),0,3,1,1,0,
"Y - - 10 - obj C - - 10 - name "
"C - 'Option_t' 10 \"\" option",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("GetObject",887,G__ManualBase4_134_6_8,121,-1,-1,0,2,1,1,0,
"C - - 10 - namecycle Y - - 1 - ptr",(char*)NULL,(void*)NULL,0);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncManualBase4() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
static void G__cpp_setup_global0() {

   /* Setting up global variables */
   G__resetplocal();

}

static void G__cpp_setup_global1() {

   G__resetglobalenv();
}
extern "C" void G__cpp_setup_globalManualBase4() {
  G__cpp_setup_global0();
  G__cpp_setup_global1();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
static void G__cpp_setup_func0() {
   G__lastifuncposition();

}

static void G__cpp_setup_func1() {
}

static void G__cpp_setup_func2() {

   G__resetifuncposition();
}

extern "C" void G__cpp_setup_funcManualBase4() {
  G__cpp_setup_func0();
  G__cpp_setup_func1();
  G__cpp_setup_func2();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR = { "vector<TStreamerInfo*,allocator<TStreamerInfo*> >" , 99 , -1 };
G__linked_taginfo G__ManualBase4LN__RanitlETStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR = { "_Ranit<TStreamerInfo*,long,TStreamerInfo**,TStreamerInfo*&>" , 115 , -1 };
G__linked_taginfo G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR = { "iterator<random_access_iterator_tag,TStreamerInfo*,long,TStreamerInfo**,TStreamerInfo*&>" , 115 , -1 };
G__linked_taginfo G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR = { "reverse_iterator<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>" , 99 , -1 };
G__linked_taginfo G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR = { "iterator<random_access_iterator_tag,TStreamerInfo*,vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator::difference_type,TStreamerInfo**,TStreamerInfo*&>" , 115 , -1 };
G__linked_taginfo G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLconst_iteratorgR = { "reverse_iterator<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::const_iterator>" , 99 , -1 };
G__linked_taginfo G__ManualBase4LN_TDirectory = { "TDirectory" , 99 , -1 };

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableManualBase4() {
  G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR.tagnum = -1 ;
  G__ManualBase4LN__RanitlETStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR.tagnum = -1 ;
  G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR.tagnum = -1 ;
  G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR.tagnum = -1 ;
  G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR.tagnum = -1 ;
  G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLconst_iteratorgR.tagnum = -1 ;
  G__ManualBase4LN_TDirectory.tagnum = -1 ;
}


extern "C" void G__cpp_setup_tagtableManualBase4() {

   /* Setting up class,struct,union tag entry */
   G__get_linked_tagnum(&G__ManualBase4LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR);
   G__get_linked_tagnum(&G__ManualBase4LN__RanitlETStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR);
   G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOlongcOTStreamerInfomUmUcOTStreamerInfomUaNgR);
   G__get_linked_tagnum(&G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR);
   G__get_linked_tagnum(&G__ManualBase4LN_iteratorlErandom_access_iterator_tagcOTStreamerInfomUcOvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorcLcLdifference_typecOTStreamerInfomUmUcOTStreamerInfomUaNgR);
   G__get_linked_tagnum(&G__ManualBase4LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLconst_iteratorgR);
   G__tagtable_setup(G__get_linked_tagnum(&G__ManualBase4LN_TDirectory),sizeof(TDirectory),-1,65280,"Describe directory structure in memory",NULL,G__setup_memfuncTDirectory);
}
extern "C" void G__cpp_setupManualBase4(void) {
  G__check_setup_version(30051515,"G__cpp_setupManualBase4()");
  G__set_cpp_environmentManualBase4();
  G__cpp_setup_tagtableManualBase4();

  G__cpp_setup_inheritanceManualBase4();

  G__cpp_setup_typetableManualBase4();

  G__cpp_setup_memvarManualBase4();

  G__cpp_setup_memfuncManualBase4();
  G__cpp_setup_globalManualBase4();
  G__cpp_setup_funcManualBase4();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncManualBase4();
  return;
}
class G__cpp_setup_initManualBase4 {
  public:
    G__cpp_setup_initManualBase4() { G__add_setup_func("ManualBase4",(G__incsetup)(&G__cpp_setupManualBase4)); G__call_setup_funcs(); }
   ~G__cpp_setup_initManualBase4() { G__remove_setup_func("ManualBase4"); }
};
G__cpp_setup_initManualBase4 G__cpp_setup_initializerManualBase4;

