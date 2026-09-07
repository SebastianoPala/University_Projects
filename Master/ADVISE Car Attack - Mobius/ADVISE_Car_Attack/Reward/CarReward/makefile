OBJS=CarRewardPVNodes.o CarRewardPVModel.o 


#include common Makefile for default targets
include ../../Makefile.project
include $(MOBIUSROOT)/Cpp/BaseClasses/Makefile.common

all: normal
opt: $(ROOTDIR)/lib/$(PLATFORM)/libCarRewardPV.a
normal debug trace: $(ROOTDIR)/lib/$(PLATFORM)/libCarRewardPV_debug.a

$(ROOTDIR)/lib/$(PLATFORM)/libCarRewardPV.a: FORCE
	$(MAKE) lib TARGET=libCarRewardPV.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="" ROOTLIBDIR="."

$(ROOTDIR)/lib/$(PLATFORM)/libCarRewardPV_debug.a: FORCE
	$(MAKE) lib TARGET=libCarRewardPV_debug.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="_debug" ROOTLIBDIR="."

clean_sub_dirs:

