SOURCES := $(wildcard *.cpp)
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES))
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

override PACKAGES := glibmm-2.4

override CXXFLAGS := -g --std=c++0x $(shell pkg-config --cflags $(PACKAGES)) -O2 -pipe -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard $(CXXFLAGS)
override LDFLAGS := $(shell pkg-config --libs-only-l $(PACKAGES)) $(LDFLAGS)
override LIBS := $(shell pkg-config --libs-only-L --libs-only-other $(PACKAGES)) $(LIBS)


PiCutter: $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $+ $(LIBS)

%.d:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -MM $<

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) *.d
	$(RM) *.o
	$(RM) PiCutter

include $(DEPENDS)
