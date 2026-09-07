OBJS=CarStudyRangeStudy.o 


#include common Makefile for default targets
include ../../Makefile.project
include $(MOBIUSROOT)/Cpp/BaseClasses/Makefile.common

all: normal
opt: $(ROOTDIR)/lib/$(PLATFORM)/libCarStudyRangeStudy.a
normal debug trace: $(ROOTDIR)/lib/$(PLATFORM)/libCarStudyRangeStudy_debug.a

$(ROOTDIR)/lib/$(PLATFORM)/libCarStudyRangeStudy.a: FORCE
	$(MAKE) lib TARGET=libCarStudyRangeStudy.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="" ROOTLIBDIR="."

$(ROOTDIR)/lib/$(PLATFORM)/libCarStudyRangeStudy_debug.a: FORCE
	$(MAKE) lib TARGET=libCarStudyRangeStudy_debug.a OBJS="$(OBJS)" USERINCLUDE="$(USERINCLUDE)" TYPE="_debug" ROOTLIBDIR="."

clean_sub_dirs:

