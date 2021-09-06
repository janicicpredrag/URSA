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

#ifndef ARGOSATSOLVER_H_INCLUDED
#define ARGOSATSOLVER_H_INCLUDED

#include <vector>
#include "SATsolver.h"
#include "Solver.hpp"
#include "Valuation.hpp"


using namespace std;

class ArgoSATsolver : public SATsolver {

public:
    ArgoSATsolver() { };
    ~ArgoSATsolver() { };
   void InitSolver(unsigned int numVars);
   void addClause(vector<int> &clause); 
   bool solve();
   bool isTrueVar(unsigned int var);

private:
   ArgoSat::Solver solver;
   ArgoSat::Valuation model;

};


#endif

