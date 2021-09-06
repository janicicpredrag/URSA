/************************************************************************************
URSA -- Copyright (c) 2017, Predrag Janicic

This file is part of URSA
 
URSA is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
URSA is WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
**************************************************************************************/

#include <MiniSATSolver.h>
#include "core/SolverTypes.h"
#include <vector>
#include <iostream>

using namespace std;
using namespace Minisat;

void MiniSATsolver::InitSolver(unsigned int /*numVars*/) {
  /*solver.reset();
  solver.setNumberOfVariables(numVars);*/
}


void MiniSATsolver::addClause(vector<int> &clause) { 
  vec<Lit> literals;
  Var var;
  for (vector<int>::iterator it = clause.begin(); it != clause.end(); it++) {
    var = abs(*it);
    while (var > solver.nVars()) 
       solver.newVar();
    literals.push ( ((*it) > 0) ? mkLit(var-1,false) : ~mkLit(var-1,false) );
  }
  solver.addClause_(literals); 
}


bool MiniSATsolver::solve() {
//   vec<Lit> dummy;
//   lbool ret = solver.solveLimited(dummy);
   bool ret = solver.solve();
   if (!ret)
     return false;

   vec<Lit> oppositeClause;
   for (Var var = 0; var < solver.nVars(); var++) {
     if (solver.modelValue(var) == l_True) {
       oppositeClause.push(~mkLit(var, false));
     }
     else {
       oppositeClause.push(mkLit(var, false));
     }
   }
   solver.addClause(oppositeClause);

  return true;
}



bool MiniSATsolver::isTrueVar(unsigned int var) {
 return (solver.model[(Var)(var-1)] == l_True);
}










