/********************************************************
* accstrm.cxx
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************/
#include "accstrm.h"

#ifdef G__MEMTEST
#undef malloc
#undef free
#endif

extern "C" void G__cpp_reset_tagtableG__stream();

extern "C" void G__set_cpp_environmentG__stream() {
  G__add_compiledheader("iostrm.h");
  G__add_compiledheader("fstrm.h");
  G__add_compiledheader("sstrm.h");
  G__cpp_reset_tagtableG__stream();
}
class G__accstrmdOcxx_tag {};

void* operator new(size_t size,G__accstrmdOcxx_tag* p) {
  if(p && G__PVOID!=G__getgvp()) return((void*)p);
#ifndef G__ROOT
  return(malloc(size));
#else
  return(::operator new(size));
#endif
}

/* dummy, for exception */
#ifdef G__EH_DUMMY_DELETE
void operator delete(void *p,G__accstrmdOcxx_tag* x) {
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

void G__DELDMY_accstrmdOcxx() { G__operator_delete(0); }

extern "C" int G__cpp_dllrevG__stream() { return(30051515); }

/*********************************************************
* Member function Interface Method
*********************************************************/

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
class G__Sizep2memfuncG__stream {
 public:
  G__Sizep2memfuncG__stream() {p=&G__Sizep2memfuncG__stream::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncG__stream::*p)();
};

size_t G__get_sizep2memfuncG__stream()
{
  G__Sizep2memfuncG__stream a;
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
extern "C" void G__cpp_setup_inheritanceG__stream() {

   /* Setting up class inheritance */
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableG__stream() {

   /* Setting up typedef entry */
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */
extern "C" void G__cpp_setup_memvarG__stream() {
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

/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncG__stream() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
static void G__cpp_setup_global0() {

   /* Setting up global variables */
   G__resetplocal();


   G__resetglobalenv();
}
extern "C" void G__cpp_setup_globalG__stream() {
  G__cpp_setup_global0();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
static void G__cpp_setup_func0() {
   G__lastifuncposition();

}

static void G__cpp_setup_func1() {

   G__resetifuncposition();
}

extern "C" void G__cpp_setup_funcG__stream() {
  G__cpp_setup_func0();
  G__cpp_setup_func1();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableG__stream() {
}


extern "C" void G__cpp_setup_tagtableG__stream() {

   /* Setting up class,struct,union tag entry */
}
extern "C" void G__cpp_setupG__stream(void) {
  G__check_setup_version(30051515,"G__cpp_setupG__stream()");
  G__set_cpp_environmentG__stream();
  G__cpp_setup_tagtableG__stream();

  G__cpp_setup_inheritanceG__stream();

  G__cpp_setup_typetableG__stream();

  G__cpp_setup_memvarG__stream();

  G__cpp_setup_memfuncG__stream();
  G__cpp_setup_globalG__stream();
  G__cpp_setup_funcG__stream();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncG__stream();
  return;
}
class G__cpp_setup_initG__stream {
  public:
    G__cpp_setup_initG__stream() { G__add_setup_func("G__stream",(G__incsetup)(&G__cpp_setupG__stream)); G__call_setup_funcs(); }
   ~G__cpp_setup_initG__stream() { G__remove_setup_func("G__stream"); }
};
G__cpp_setup_initG__stream G__cpp_setup_initializerG__stream;

