CONFIG=root-config
CXXFLAGS=$(shell $(CONFIG) --cflags)
LIBS=$(shell $(CONFIG) --libs)
LDFLAGS=$(shell $(CONFIG) --ldflags)
CXX=g++
# enable debug settings for GCC
#ADDCXXFLAGS=-ggdb -O0
# OR enable performance optimizations
ADDCXXFLAGS=-g -O2
VPATH=./
HDIR=./

HDRS= $(HDIR)/langaufit.h

CCEOBJS=	cce_histos.o 
FITOBJS=	fit_langaus.o 

all: cce_histos fit_langaus
cce_histos: $(CCEOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(CCEOBJS) $(LDFLAGS) $(LIBS)
fit_langaus: $(FITOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(FITOBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) $(ADDCXXFLAGS) -c $< 

clean:
	rm -rf cce_histos fit_langaus $(OBJS) 
