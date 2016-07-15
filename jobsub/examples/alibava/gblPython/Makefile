CONFIG=root-config
CXXFLAGS=$(shell $(CONFIG) --cflags)
LIBS=$(shell $(CONFIG) --libs)
LDFLAGS=$(shell $(CONFIG) --ldflags)
CXX=g++
# enable debug settings for GCC
#ADDCXXFLAGS=-ggdb -O0
# OR enable performance optimizations
#ADDCXXFLAGS=-g -O2
ADDCXXFLAGS=-ggdb -O0 -std=c++0x

VPATH=./
HDIR=./

HDRS= $(HDIR)/langaufit.h $(HDIR)/lam_histos.h $(HDIR)/calculate_err.h $(HDIR)/runall.h

LAMOBJS=	lam_histos.o 
CCEOBJS=	cce_histos.o 
FITOBJS=	fit_langaus.o 
ERROBJS=	calculate_err.o 
RUNALLOBJS=	runall.o 
	
all: calculate_err runall lam_histos cce_histos fit_langaus 

runall: $(RUNALLOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(RUNALLOBJS) $(LDFLAGS) $(LIBS)

calculate_err: $(ERROBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(ERROBJS) $(LDFLAGS) $(LIBS)

cce_histos: $(CCEOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(CCEOBJS) $(LDFLAGS) $(LIBS)
fit_langaus: $(FITOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(FITOBJS) $(LDFLAGS) $(LIBS)
lam_histos: $(LAMOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(LAMOBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) $(ADDCXXFLAGS) -c $< 

clean:
	rm -rf lam_histos cce_histos fit_langaus runall calculate_err $(ERROBJS) $(RUNALLOBJ) $(CCEOBJS) $(FITOBJS) $(LAMOBJS) $(OBJS) 
