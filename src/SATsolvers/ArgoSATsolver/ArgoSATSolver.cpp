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

#include <ArgoSATSolver.h>
#include <Variable.hpp>
#include <Literal.hpp>


using namespace std;
using namespace ArgoSat;

void ArgoSATsolver::InitSolver(unsigned int numVars) {
  solver.reset();
  solver.setNumberOfVariables(numVars);
}



void ArgoSATsolver::addClause(vector<int> &clause) { 
  vector<Literal> c;
  for (vector<int>::iterator it = clause.begin(); it != clause.end(); it++) 
     c.push_back(Literals::fromInt(*it));
  solver.addInitialClause(c);
}



bool ArgoSATsolver::solve() {
  solver.solve();  
  if (solver.isUnsatisfiable()) 
    return false;  

  model = solver.getModel();
  std::vector<Literal> oppositeClause;
  for (Variable var = 0; var < solver.getNumberOfVariables(); var++) {
    if (model.isTrueVariable(var)) 
      oppositeClause.push_back(Literals::literal(var, false));
    else 
      oppositeClause.push_back(Literals::literal(var, true));
  }
  solver.addInitialClause(oppositeClause);

  return true; 
}



bool ArgoSATsolver::isTrueVar(unsigned int var) {
 return (model.isTrueVariable(var-1));
}










