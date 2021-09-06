SHELL = /bin/sh
CXX = g++
LDFLAGS = -static-libstdc++
CXXFLAGS = -std=c++11 -O2 -DNDEBUG  
ADDFLAGS ?=
INCLUDE = -IAbstractNumbers -IParser -IInterpreter -IUtils -ISATsolvers -ISATsolvers/clasp_solver -ISATsolvers/clasp_solver/include -ISATsolvers/clasp_solver/program_opts -ISATsolvers/ArgoSATsolver -ISATsolvers/ArgoSATsolver/basic-types -ISATsolvers/ArgoSATsolver/auxiliary -ISATsolvers/ArgoSATsolver/listeners -ISATsolvers/ArgoSATsolver/strategies -ISATsolvers/minisat  

ALL_CXXFLAGS = $(CXXFLAGS) -W -Wall $(INCLUDE)  $(ADDFLAGS)

sources := $(wildcard Interpreter/*.cpp)
sources += $(wildcard AbstractNumbers/*.cpp)
sources += $(wildcard Utils/*.cpp)
sources += $(wildcard SATsolvers/ArgoSATsolver/*.cpp)
sources += $(wildcard SATsolvers/ArgoSATsolver/auxiliary/*.cpp)
sources += $(wildcard SATsolvers/ArgoSATsolver/basic-types/*.cpp)
sources += $(wildcard SATsolvers/ArgoSATsolver/listeners/*.cpp)
sources += $(wildcard SATsolvers/ArgoSATsolver/strategies/*.cpp)
sources += $(wildcard SATsolvers/clasp_solver/*.cpp)
sources += $(wildcard SATsolvers/clasp_solver/clasp/lib/*.cpp)
sources += $(wildcard SATsolvers/clasp_solver/program_opts/*.cpp)
sources += $(wildcard SATsolvers/minisat/*.cpp)
sources += $(wildcard SATsolvers/minisat/core/*.cc)
sources += $(wildcard SATsolvers/minisat/mtl/*.cc)
sources += $(wildcard SATsolvers/minisat/simp/*.cc)
sources += $(wildcard SATsolvers/minisat/utils/*.cc)

TARGET:=ursa
objects:=$(patsubst %.c,%.o, $(patsubst %.cpp,%.o,$(sources)))
deps:=$(patsubst %.cpp,%.dep,$(sources))
bindir = .

$(TARGET):	$(objects) $(deps) Parser/lex.yy.o Parser/ursa.tab.o 
	$(CXX) $(ALL_CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(objects) Parser/lex.yy.o Parser/ursa.tab.o

Parser/lex.yy.o : Parser/lex.yy.c Parser/ursa.tab.hpp  
	g++ -c -IParser Parser/lex.yy.c
	mv lex.yy.o Parser
Parser/lex.yy.c : Parser/ursa.l
	flex Parser/ursa.l 
	mv lex.yy.c Parser
Parser/ursa.tab.o : Parser/ursa.tab.cpp 
	g++ -c -IParser Parser/ursa.tab.cpp
	mv ursa.tab.o Parser
Parser/ursa.tab.cpp Parser/ursa.tab.hpp: Parser/ursa.ypp
	bison -d Parser/ursa.ypp
	mv ursa.tab.* Parser 

%.o:	%.cpp
	$(CXX) $(ALL_CXXFLAGS) -c $< -o $@ 

%.dep: %.cpp
	$(CXX) $(ALL_CXXFLAGS) $(INCLUDE) -MM $< > $@

.PHONY = clean
clean:
	rm `find . -name '*.dep'` `find . -name '*.o'` `find . -name '*.cpp~'` `find . -name '*.hpp~'` `find . -name '*.cc~'` `find . -name '*.h~'` `find . -name '*.yy.c'` `find . -name '*.tab.cpp'` `find . -name '*.l~'` `find . -name '*.ypp~'` `find . -name '.directory'` 





