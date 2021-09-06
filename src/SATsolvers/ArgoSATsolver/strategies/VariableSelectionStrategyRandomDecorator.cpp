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
#include "VariableSelectionStrategyRandomDecorator.hpp"
#include "Auxiliary.hpp"
#include "Solver.hpp"

namespace ArgoSat {

  Variable VariableSelectionStrategyRandomDecorator::getVariable() {
    if (randFloat() <= _percentRandom) 
      return getRandomUndefinedVariable();
    else {	
      return _strategy->getVariable();
    }
  }

  Variable VariableSelectionStrategyRandomDecorator::getRandomUndefinedVariable() {
    std::vector<Variable> undefined_vars;
    undefined_vars.reserve(_solver.getNumberOfVariables());
    for (Variable v = 0; v < _solver.getNumberOfVariables(); v++) {
      if (_solver.getTrail().isUndefVariable(v)) {
	undefined_vars.push_back(v);
      }
    }
    int pos = randInt(undefined_vars.size());
    Variable v = undefined_vars[pos];
    return v;
  }

  VariableSelectionStrategy* VariableSelectionStrategyRandomDecorator::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      return new VariableSelectionStrategyRandomDecorator(solver, 0, 1.0);

    double percent = atof(argv[curr++]);

    VariableSelectionStrategy* strategy = VariableSelectionStrategy::createFromCmdLine(solver, argc, argv, curr);

    return new VariableSelectionStrategyRandomDecorator(solver, strategy, percent);
  }

}//namespace ArgoSat
