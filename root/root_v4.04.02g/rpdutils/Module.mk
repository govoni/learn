# Module.mk for Rootd/Proofd authentication utilities
# Copyright (c) 2002 Rene Brun and Fons Rademakers
#
# Author: Gerardo Ganis, 7/4/2003

MODDIR       := rpdutils
MODDIRS      := $(MODDIR)/src
MODDIRI      := $(MODDIR)/inc

RPDUTILDIR   := $(MODDIR)
RPDUTILDIRS  := $(RPDUTILDIR)/src
RPDUTILDIRI  := $(RPDUTILDIR)/inc

##### $(RPDUTILO) #####
RPDUTILH     := $(wildcard $(MODDIRI)/*.h)
RPDUTILS     := $(wildcard $(MODDIRS)/*.cxx)
RPDUTILO     := $(RPDUTILS:.cxx=.o)

RPDUTILDEP   := $(RPDUTILO:.o=.d)

##### for libSrvAuth #####
SRVAUTHS     := $(MODDIRS)/rpdutils.cxx $(MODDIRS)/ssh.cxx
SRVAUTHO     := $(SRVAUTHS:.cxx=.o)

SRVAUTHLIB   := $(LPATH)/libSrvAuth.$(SOEXT)

##### Flags used in rootd amd proofd Module.mk #####
# use shadow passwords for authentication
ifneq ($(SHADOWFLAGS),)
SHADOWLIBS   := $(SHADOWLIBDIR) $(SHADOWLIB)
endif

# use AFS for authentication
ifneq ($(AFSLIB),)
AFSFLAGS     := -DR__AFS
AFSLIBS      := $(AFSLIBDIR) $(AFSLIB)
endif

# use SRP for authentication
ifneq ($(SRPLIB),)
SRPFLAGS     := $(SRPINCDIR:%=-I%)
SRPLIBS      := $(SRPLIBDIR) $(SRPLIB)
endif

# use krb5 for authentication
ifneq ($(KRB5LIB),)
KRB5FLAGS     := $(KRB5INCDIR:%=-I%)
KRB5LIBS      := $(KRB5LIBDIR) $(KRB5LIB)
endif

# use Globus for authentication
ifneq ($(GLOBUSLIB),)
GLBSFLAGS     := $(GLOBUSINCDIR:%=-I%)
GLBSLIBS      := $(GLOBUSLIBDIR) $(GLOBUSLIB)
SRVAUTHS      += $(MODDIRS)/globus.cxx
SRVAUTHO      += $(MODDIRS)/globus.o
else
RPDUTILS      := $(filter-out $(MODDIRS)/globus.cxx,$(RPDUTILS))
RPDUTILO      := $(filter-out $(MODDIRS)/globus.o,$(RPDUTILO))
GLBSLIBS      += $(SSLLIBDIR) $(SSLLIB)
endif

# Combined...
AUTHFLAGS     := $(EXTRA_AUTHFLAGS) $(SHADOWFLAGS) $(AFSFLAGS) $(SRPFLAGS) \
                 $(KRB5FLAGS) $(GLBSFLAGS) $(SSLFLAGS)
AUTHLIBS      := $(SHADOWLIBS) $(AFSLIBS) $(SRPLIBS) $(KRB5LIBS) $(GLBSLIBS) \
                 $(COMERRLIBDIR) $(COMERRLIB) $(RESOLVLIB) \
                 $(CRYPTOLIBDIR) $(CRYPTOLIB)

# used in the main Makefile
ALLHDRS       += $(patsubst $(MODDIRI)/%.h,include/%.h,$(RPDUTILH))
ALLLIBS       += $(SRVAUTHLIB)

# include all dependency files
INCLUDEFILES  += $(RPDUTILDEP)

##### local rules #####
include/%.h:    $(RPDUTILDIRI)/%.h
		cp $< $@

$(SRVAUTHLIB):  $(SRVAUTHO) $(DAEMONUTILSO) $(MAINLIBS) $(SRVAUTHLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libSrvAuth.$(SOEXT) $@ "$(SRVAUTHO)" \
		   "$(DAEMONUTILSO) $(CRYPTLIBS) $(AUTHLIBS)"

all-rpdutils:   $(RPDUTILO) $(SRVAUTHLIB)

clean-rpdutils:
		@rm -f $(RPDUTILO)

clean::         clean-rpdutils

distclean-rpdutils: clean-rpdutils
		@rm -f $(RPDUTILDEP) $(SRVAUTHLIB)

distclean::     distclean-rpdutils

##### extra rules ######
$(RPDUTILO): %.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(AUTHFLAGS) -o $@ -c $<

$(DAEMONUTILSO): %.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(AUTHFLAGS) -o $@ -c $<
