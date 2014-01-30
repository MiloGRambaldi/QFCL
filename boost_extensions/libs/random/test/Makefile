#!/usr/bin/env desres-exec
#
# Makefile, relies on the desres garden to setup its environment. Uses 
# modules from a different architecture if instructed to by DESRES_CROSS_ISA. 
# Use xmk prereq to see pre-requisite modules. 
# 
# Test the environment with: BUILDCLASS=Release32 ./xmk test-env
# 
#{
# eval `desres-cleanenv --print-eval-shell=sh \
#  --keep PREFIX `
#
# source $DESRES_ROOT/modules/init/sourceme.sh
# module load gardenutils/1.9.5/lib-sh; 
# . desres-install-functions
#
# module_load_all \
#   make/3.81-11A/bin \
#   gcc/4.6.2-23A/bin \
#   makefragments/3.2.16 \
#   boost/1_48_0-25/Release/lib \
#
### Sets LDLIBS, CPPFLAGS, CFLAGS from module's env crumbs
# . $GARDENUTILS_PREFIX/lib/desres-install-functions
# desres_module_stripcolons
#
# make -f "$0" "$@"
#}
#

All:

## autodepends doesn't work well if OBJ is empty!  This doesn't
## actually put objects or binaries in objs, though!!
OBJ/:=

#include $(MAKEFRAGMENTS_PREFIX)/autodepends.mk
include $(MAKEFRAGMENTS_PREFIX)/linkwithc++.mk

ifeq (1,0) # Change this to toggle between icpc and gcc
# To get icpc to work with boost we have to link against
# an up-to-date libstdc++.so, but we have to compile with
# an out-of-date include/ STL directory because icpc doesn't
# grok the contents of the latest g++ STL includes.
CXX=desres-with -m gcc/4.4.4-21A/bin \
                -m intel/ccompxe/2011.2.137-05/icc/bin \
                icpc
CPPFLAGS+=-DBOOST_HAS_AESNI
CXXFLAGS+=-ggdb -fast
CXXFLAGS+=-diag-disable 279 # 'controlling expression is a constant'
gcclib:=$(dir $(shell gcc --print-file-name=libstdc++.so))
LDFLAGS+=-L$(gcclib) -Wl,-rpath=$(gcclib)
else

CXX=g++
OPTIMIZE= -O3 -ggdb3
#OPTIMIZE=-O0 -ggdb3
COMMONFLAGS=-Wall $(OPTIMIZE) -march=native
CXXFLAGS+=$(COMMONFLAGS) -std=c++98
CFLAGS+=$(COMMONFLAGS) -std=gnu99

endif

CPPFLAGS+= -I../../..
LDLIBS+= -lboost_unit_test_framework # -lgmpxx -lgmp #-lcrypto
LIB_LDLIBS=$(LDLIBS)

%.s : %.cpp
	$(CXX) -S $(filter-out %gdb,$(CXXFLAGS) $(CPPFLAGS)) $<

%.s : %.c
	$(CC) -S $(filter-out %gdb,$(CFLAGS) $(CPPFLAGS)) $<

HDRS:=../../../boost/random/*.hpp ../../../boost/random/detail/*.hpp *.hpp *.ipp Makefile

Binaries:=test_mulhilo random_speed test_philox2x64 test_aes test_philox test_threefry test_ars ex1

All: $(Binaries)
$(Binaries) : % : %.o
$(Binaries:%=%.o): $(HDRS)
$(Binaries:%=%.s): $(HDRS)

$(filter test_%,$(Binaries:%=%.o)): override CPPFLAGS+=-DBOOST_TEST_DYN_LINK

test_mt19937_64.o test_philox2x64.o : override COMMONFLAGS+= -Wno-strict-aliasing -Wno-sequence-point

.PHONY: test-env
test-env:
	@echo LDFLAGS: $(LDFLAGS) 
	@echo CFLAGS: $(CFLAGS) 
	@echo CXXFLAGS: $(CXXFLAGS)
