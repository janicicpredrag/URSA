/******************************************************************************
 * Copyright (C) 2007-2009. Filip Maric, Predrag Janicic
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program is inspired by MiniSat solver (C) Een, Sorensson 2003-2006.
 ******************************************************************************/

#include <string.h>
#include "VariableSelectionStrategyMinisat.hpp"
#include "Solver.hpp"


namespace ArgoSat {

  MinisatVariableSelectionStrategy::MinisatVariableSelectionStrategy
  (const Solver& solver, double bumpAmount, 
   double decayFactor, bool /*useLiteralCountingInitialization*/)
    : _solver(solver),
      _activities(bumpAmount, decayFactor),
      _activityHeap(Activities::Comparator(&_activities)) {
    _solver.addListener(this);
    init();
  }

  MinisatVariableSelectionStrategy::~MinisatVariableSelectionStrategy() {
    _solver.removeListener(this);
  }
  
  void MinisatVariableSelectionStrategy::init() {
    unsigned numVars = _solver.getNumberOfVariables();
    _activities.init(numVars);
    for (Variable var = 0; var < numVars; var++) {
      _activityHeap.push_heap(var);
    }
  }

  Variable MinisatVariableSelectionStrategy::getVariable() {
    Variable maxVar;
    do {
      maxVar = _activityHeap.pop_heap();
    } while (!(_solver.getTrail().isUndefVariable(maxVar)));

    return maxVar;
  }

  VariableSelectionStrategy* MinisatVariableSelectionStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    bool useLiteralCountingInitialization = false;
    if (curr < argc && strcmp(argv[curr], "init") == 0) {
      curr++;
      useLiteralCountingInitialization = true;
    }

    if (curr >= argc || (!isFloat(argv[curr])))
      return new MinisatVariableSelectionStrategy(solver, useLiteralCountingInitialization);

    double bumpFactor = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("MinisatVariableSelectionStrategy::VariableDecayFactor expected");
    double decayFactor = atof(argv[curr++]);

    return new MinisatVariableSelectionStrategy(solver, bumpFactor, decayFactor, useLiteralCountingInitialization);
  }

}//namespace ArgoSat
