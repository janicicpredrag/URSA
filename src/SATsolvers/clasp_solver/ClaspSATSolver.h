/************************************************************************************
URSA -- Copyright (c) 2010, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#ifndef CLASPSATSOLVER_H_INCLUDED
#define CLASPSATSOLVER_H_INCLUDED

#include <clasp/include/model_enumerators.h>
#include <vector>
#include "options.h"
#include "SATsolver.h"


using namespace std;

class ClaspSATsolver : public SATsolver {

public:
   ClaspSATsolver() { };
   ~ClaspSATsolver() { };
   void InitSolver(unsigned int numVars);
   void addClause(vector<int> &clause); 
   bool solve();
   bool isTrueVar(unsigned int var);

private:
	Clasp::Options       options;
	Clasp::Solver        solver;
	uint32        problemSize;
	struct Clasp::Enumerator*   enum_;
	Clasp::PodVector<uint8>::type flags;
  bool bFirstCall;

};



#endif

