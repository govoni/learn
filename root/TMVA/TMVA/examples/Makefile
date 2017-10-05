######################################################################
# Project: TMVA - Toolkit for Multivariate Data Analysis             #
# Code   : Example usage for TMVA analyses                           #
###################################################################### 

MAKEFLAGS = --no-print-directory -r -s

INCLUDE = -I../include $(shell root-config --cflags)
LIBS    = -L../lib/ $(shell root-config --libs) -lTMVA.1 -lMLP -lTreePlayer -lMinuit

BINS = TMVAnalysis TMVApplication TMVApplicationAlternative TMVAGAexample TMVAGAexample2 

all: $(BINS)

$(BINS): % : %.cxx ../src/*.h
	@echo -n "Building $@ ... "
	@echo	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	@echo "Done"

clean:
	rm -f $(BINS)
