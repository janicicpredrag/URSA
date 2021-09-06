SHELL = /bin/sh
CXX = g++
CXXFLAGS ?= -O3 -DNDEBUG
ADDFLAGS ?= 
INCLUDE = -I../../ -I.
ALL_CXXFLAGS = -W -Wall $(INCLUDE) $(CXXFLAGS) $(ADDFLAGS)
TARGET = ../bin/libclasp.a
sources:=$(wildcard *.cpp)
objects:=$(patsubst %.cpp,%.o,$(sources))
deps:=$(patsubst %.cpp,%.dep,$(sources))

$(TARGET):	$(objects) $(deps)
	$(AR)  cvrs $(TARGET) $(objects)

ifneq ($(MAKECMDGOALS),clean)
-include $(sources:.cpp=.dep)
endif

%.o:	%.cpp
	$(CXX) $(ALL_CXXFLAGS) -c $< -o $@

%.dep: %.cpp
	$(CXX) $(INCLUDE) -MM $< > $@

.PHONY:	clean
clean:
	rm -f $(TARGET)
	rm -f $(objects)
	rm -f $(deps)

