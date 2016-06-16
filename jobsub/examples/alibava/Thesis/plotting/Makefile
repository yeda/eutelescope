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

HDRS= $(HDIR)/formatting.h 

ChapRecoOBJS=	chapReco.o 
ChapTrackOBJS=	chapTrack.o 

all: chapReco chapTrack

chapReco: $(ChapRecoOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(ChapRecoOBJS) $(LDFLAGS) $(LIBS)

chapTrack: $(ChapTrackOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(ADDCXXFLAGS) $(ChapTrackOBJS) $(LDFLAGS) $(LIBS)

%.o: %.cc $(HDRS)
	$(CXX) $(CXXFLAGS) $(ADDCXXFLAGS) -c $< 

clean:
	rm -rf chapReco chapTrack $(ChapRecoOBJS) $(ChapTrackOBJS) $(OBJS) 
