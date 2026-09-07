OBJS=


#include common Makefile for default targets
include ../../Makefile.project
include $(MOBIUSROOT)/Cpp/BaseClasses/Makefile.common

BINDIR=
MAINLIB=-lsimMain
MAINLIB_debug=-lsimMain_debug
SIMLIB=sim
SIMLIB_debug=sim_debug
SIMLIB_trace=sim_trace
PROJECTLIBS=-lCarStudyRangeStudy -lCarRewardPV -lCarADVISE 
PROJECTLIBS_debug=-lCarStudyRangeStudy_debug -lCarRewardPV_debug -lCarADVISE_debug 
MOBIUSLIBS=-lTRex -lStudy -lPV -lADVISE -lbase -lstate -lmobutil -ldist 
MOBIUSLIBS_debug=-lTRex_debug -lStudy_debug -lPV_debug -lADVISE_debug -lbase_debug -lstate_debug -lmobutil_debug -ldist_debug 
USERDEFLIBNAME=
USERDEFLIBDIR=

all: normal

opt \
CarSimSim_$(ARCH): SUBMODELS/opt
	$(MAKE) exec3 "TARGET=CarSimSim_$(ARCH)" "USERLIBDIR=-L$(MOBIUSROOT)/Cpp/lib/$(PLATFORM)$(FORCE_ARCH) $(USERDEFLIBDIR)" "USERLIB=$(MAINLIB) -l$(SIMLIB) $(USERDEFLIBNAME) $(PROJECTLIBS) $(USERDEFLIBNAME) $(MOBIUSLIBS)" 

normal debug \
CarSimSim_$(ARCH)_debug: SUBMODELS/debug
	$(MAKE) exec3 "TARGET=CarSimSim_$(ARCH)_debug" "USERLIBDIR=-L$(MOBIUSROOT)/Cpp/lib/$(PLATFORM)$(FORCE_ARCH) $(USERDEFLIBDIR)" "USERLIB=$(MAINLIB_debug) -l$(SIMLIB_debug) $(USERDEFLIBNAME) $(PROJECTLIBS_debug) $(USERDEFLIBNAME) $(MOBIUSLIBS_debug) " "BUILDTARGET=_debug"

trace \
CarSimSim_$(ARCH)_trace:
	$(MAKE) exec3 "TARGET=CarSimSim_$(ARCH)_trace" "USERLIBDIR=-L$(MOBIUSROOT)/Cpp/lib/$(PLATFORM)$(FORCE_ARCH) $(USERDEFLIBDIR)" "USERLIB=$(MAINLIB_debug) -l$(SIMLIB_trace) $(USERDEFLIBNAME) $(PROJECTLIBS_debug) $(USERDEFLIBNAME) $(MOBIUSLIBS_debug) " "BUILDTARGET=_debug"

clean_sub_dirs:
	cd ../../Atomic/Car/ && $(MAKE) clean
	cd ../../Reward/CarReward/ && $(MAKE) clean
	cd ../../Study/CarStudy/ && $(MAKE) clean

SUBMODELS/opt SUBMODELS/debug:
	@echo "NEW_SUBMODEL_LIB:[Car]"
	cd ../../Atomic/Car/ && $(MAKE) $(@F)
	@echo "NEW_SUBMODEL_LIB:[CarReward]"
	cd ../../Reward/CarReward/ && $(MAKE) $(@F)
	@echo "NEW_SUBMODEL_LIB:[CarStudy]"
	cd ../../Study/CarStudy/ && $(MAKE) $(@F)


