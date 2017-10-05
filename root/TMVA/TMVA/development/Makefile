######################################################################
# Project: Examples of how to use TMVA                               #
###################################################################### 

#MAKEFLAGS = --no-print-directory -r -s

INCLUDE = -I../include $(shell root-config --cflags)
LIBS    = -L../lib/ $(shell root-config --libs) -lTMVA.1 -lMLP -lTreePlayer

BINS = MakeClassTester

CLASSES = $(patsubst weights/%.C,%.o,$(wildcard weights/*.class.C))

all: $(BINS)

#dev: devTMVA

default: $(CLASSES)

%.o : weights/%.C 
	@echo -n "Building $@ ... "
	@$(CXX) $(INCLUDES) $(CXXFLAGS) -g -c $< -o $(notdir $@)
	@echo "Done"

$(BINS): % : %.cxx ../src/*.h
	@echo -n "Building $@ ... "
	@echo	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	$(CXX) $(CCFLAGS) $< $(INCLUDE) $(LIBS) -o $@
	@echo "Done"

clean:
	rm -f $(BINS)




