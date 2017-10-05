######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
###################################################################### 

MAKEFLAGS = --no-print-directory -r -s

include Makefile.arch

CXXFLAGS := -I../include $(CXXFLAGS)
LIBS     := -L../lib -lTMVA $(LIBS)

all: $(BINS)

ObjSuf = o
SrcSuf = cxx
ExeSuf =

STRESSTMVAO = stressTMVA.$(ObjSuf)
STRESSTMVAS = stressTMVA.$(SrcSuf)
STRESSTMVA  = stressTMVA$(ExeSuf)

OBJS = $(STRESSTMVAO)
PROGRAMS = $(STRESSTMVA)

all: $(PROGRAMS)

.SUFFIXES: .$(SrcSuf)

.$(SrcSuf).$(ObjSuf):
	echo $(CXX) $(INCLUDE) $(CXXFLAGS) -c $<
	$(CXX) $(INCLUDE) $(CXXFLAGS) -c $<

$(STRESSTMVA): $(STRESSTMVAO)
ifeq ($(PLATFORM),win32)
	$(LD) $(LDFLAGS) $^ $(LIBS) '$(ROOTSYS)/lib/libTMVA.lib'   '$(ROOTSYS)/lib/libMinuit.lib' '$(ROOTSYS)/lib/libMLP.lib' '$(ROOTSYS)/lib/libTreePlayer.lib' $(OutPutOpt)$@
	$(MT_EXE)
else
	echo $(LD) $(LDFLAGS) $^ $(LIBS) -lMinuit -lMLP -lTreePlayer $(OutPutOpt)$@
	$(LD) $(LDFLAGS) $^ $(LIBS) -lMinuit -lMLP -lTreePlayer $(OutPutOpt)$@
endif
	@echo "$@ done"

clean:
	rm -f $(OBJS) $(PROGRAMS)
