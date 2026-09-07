OBJS=CarADVISE.o 


#include common Makefile for default targets
include ../../Makefile.project
include $(MOBIUSROOT)/Cpp/BaseClasses/Makefile.common

all: normal
opt: $(ROOTDIR)/lib/$(PLATFORM)/libCarADVISE.a
normal debug trace: $(ROOTDIR)/lib/$(PLATFORM)/libCarADVISE_debug.a

$(ROOTDIR)/lib/$(PLATFORM)/libCarADVISE.a: FORCE
	$(MAKE) lib TARGET=libCarADVISE.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="" ROOTLIBDIR="."

$(ROOTDIR)/lib/$(PLATFORM)/libCarADVISE_debug.a: FORCE
	$(MAKE) lib TARGET=libCarADVISE_debug.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="_debug" ROOTLIBDIR="."

clean_sub_dirs:

