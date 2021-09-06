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
#ifndef __VARIABLE_SELECTION_STRATEGY_RANDOM_DECORATOR_H__
#define __VARIABLE_SELECTION_STRATEGY_RANDOM_DECORATOR_H__

#include "VariableSelectionStrategy.hpp"

namespace ArgoSat {

  class VariableSelectionStrategyRandomDecorator : public VariableSelectionStrategy {
  public:
    VariableSelectionStrategyRandomDecorator(Solver& solver, VariableSelectionStrategy* strategy, float percentRandom) 
      : _solver(solver),
	_strategy(strategy), 
	_percentRandom(percentRandom) {
    }

    ~VariableSelectionStrategyRandomDecorator() {
      delete _strategy;
    }

    Variable getVariable();

    static VariableSelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

  private:

    Variable getRandomUndefinedVariable();

    Solver& _solver;
    VariableSelectionStrategy* _strategy;
    float _percentRandom;
  };

}//namespace ArgoSat
#endif
