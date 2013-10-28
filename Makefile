SOURCES := $(wildcard *.cpp)
PROTO := $(wildcard proto/*.proto)
PROTO_SOURCES := $(patsubst proto/%.proto, %.pb.cc, $(PROTO))
PROTO_OBJECTS :=$(patsubst %.cc, %.o, $(PROTO_SOURCES))
PROTO_HEADERS :=$(patsubst %.cc, %.h, $(PROTO_SOURCES)) 
OBJECTS := $(patsubst %.cpp,%.o,$(SOURCES)) 
DEPENDS := $(patsubst %.cpp,%.d,$(SOURCES))

override PACKAGES := glibmm-2.4 protobuf

override CXXFLAGS := -g --std=c++0x $(shell pkg-config --cflags $(PACKAGES)) -O2 -pipe -mcpu=arm1176jzf-s -mfpu=vfp -mfloat-abi=hard $(CXXFLAGS)
override LDFLAGS := $(shell pkg-config --libs-only-l $(PACKAGES)) $(LDFLAGS)
override LIBS := $(shell pkg-config --libs-only-L --libs-only-other $(PACKAGES)) $(LIBS)

.SILENT:

PiCutter: $(OBJECTS) $(PROTO_OBJECTS)
	echo LD $@
	$(CXX) $(LDFLAGS) -o $@ $+ $(LIBS)
	
%.d:%.cpp $(PROTO_HEADERS)
	echo DEP $@
	$(CXX) $(CXXFLAGS) -o $@ -MM $<

%.o:%.cpp $(PROTO_HEADERS)
	echo CXX $@
	$(CXX) $(CXXFLAGS) -o $@ -c $<

%.o:%.cc $(PROTO_HEADERS)
	echo CC  $@
	$(CXX) $(CXXFLAGS) -o $@ -c $<	

%.pb.h %.pb.cc: proto/%.proto
	echo PROTO $@
	protoc --cpp_out=./ --proto_path=proto $<

.PHONY clean:
	$(RM) *.d
	$(RM) *.o
	$(RM) *.pb.*
	$(RM) PiCutter

include $(DEPENDS)
