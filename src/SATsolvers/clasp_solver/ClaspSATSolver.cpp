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

#include <clasp/include/satelite.h>
#include <clasp/include/cb_enumerator.h>
#include <clasp/include/model_enumerators.h>
#include "options.h"
#include <iostream>
#include <ClaspSATSolver.h>

using namespace std;
using namespace Clasp;

// return values
const int S_UNKNOWN       = 0;
const int S_SATISFIABLE   = 10;
const int S_UNSATISFIABLE = 20;
const int S_ERROR         = EXIT_FAILURE;
const int S_MEMORY        = 107;



class QuitePrinter : public ModelPrinter, public CBConsequences::CBPrinter {
public:
	explicit QuitePrinter(bool quiet) : quiet_(quiet)         { } 
	void printModel(const Solver&, const ModelEnumerator&) { }
	void printReport(const Solver&, const ModelEnumerator&) {  }
	void printConsequences(const Solver&, const CBConsequences&) { }
	void printReport(const Solver&, const CBConsequences&) { }
	void printMini(MinimizeConstraint*, const char*) { }

private:
	bool quiet_;
};



void ClaspSATsolver::InitSolver(unsigned int numVars) {
  options.parse(0, NULL, std::cout, solver); 
  options.setDefaults();
  options.numModels=0;
  options.quiet=true;
  options.dimacs=true;
  if (options.satPreParams[0] != 0) {
    // enable and configure the sat preprocessor
    SatElite::SatElite* pre = new SatElite::SatElite(solver);
    pre->options.maxIters = options.satPreParams[0];
    pre->options.maxOcc   = options.satPreParams[1];
    pre->options.maxTime  = options.satPreParams[2];
    pre->options.elimPure = options.numModels == 1;
    pre->options.verbose  = 0;
    solver.strategies().satPrePro.reset(pre);
  }
  options.recordSol=false;
  enum_ = (Enumerator*)new BacktrackEnumerator(new QuitePrinter(options.quiet), options.projectConfig);

  solver.reserveVars(numVars+1);
  flags.resize(numVars+1);
  for (unsigned int v = 1; v <= numVars; ++v) 
    solver.addVar(Var_t::atom_var);
  solver.startAddConstraints();
  bFirstCall=true;
}



void ClaspSATsolver::addClause(vector<int> &clause) { 
  LitVec currentClause;
  ClauseCreator nc(&solver);
  // bool ret = true;
  bool sat = false;
  nc.start();

  // For each var v: 0000p1p2c1c2
  // p1: set if v occurs negatively in any clause
  // p2: set if v occurs positively in any clause
  // c1: set if v occurs negatively in the current clause
  // c2: set if v occurs positively in the current clause
  for (vector<int>::iterator it = clause.begin(); it != clause.end(); it++) {
   int lit = *it;
   Literal rLit;
   rLit = lit >= 0 ? posLit(lit) : negLit(-lit);
   if ( (flags[rLit.var()] & (1+rLit.sign())) == 0 ) {
     flags[rLit.var()] |= 1+rLit.sign();
     nc.add(rLit);
     currentClause.push_back(rLit);
     if ((flags[rLit.var()] & 3u) == 3u) 
       sat = true;
   }
  }

  for (LitVec::iterator it = currentClause.begin(); it != currentClause.end(); ++it) {
   flags[it->var()] &= ~3u; // clear "in clause"-flags
   if (!sat) 
     flags[it->var()] |= ((1 + it->sign()) << 2);
  }
  /*ret =*/ sat || nc.end();
}



bool ClaspSATsolver::solve() {
  static int state;

  if(bFirstCall) {
    bFirstCall=false;
    state=0;
    bool ret = true, res;

    SatPreprocessor* p = 0;
    if (p) 
      solver.strategies().satPrePro.reset(p);
    for (Var i = 1; ret && i <= solver.numVars(); ++i) {
      uint8 d = (flags[i]>>2);
    	if (d == 0) 
        ret = solver.force(negLit(i), 0); 
    } 

    res = solver.endAddConstraints();
    problemSize = solver.numConstraints();

    assert(enum_);
    enum_->setNumModels(options.numModels);

    solver.add(enum_);
    options.solveParams.setEnumerator( *enum_ );
    if (res) {
	    if (options.redFract == 0.0) {
		    options.solveParams.setReduceParams((uint32)-1, 1.0, (uint32)-1, options.redOnRestart);
	    }
	    else {
		    options.solveParams.setReduceParams(static_cast<uint32>(problemSize/options.redFract)
			    , options.redInc
			    , static_cast<uint32>(problemSize*options.redMax)
			    , options.redOnRestart
		    );
	    }
    }
    else 
      return false;
  }

  bool r;
  do 
    r = Clasp::solve(solver, options.solveParams,&state);
  while(state!=3 && state!=6 && r); 

  if (state==3) 
    return true;
  else 
    return false;
}
 


bool ClaspSATsolver::isTrueVar(unsigned int var) {
 return (solver.value(var) != value_false);
}








