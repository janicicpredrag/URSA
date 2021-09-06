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
#ifndef __VARIABLE_SELECTION_STRATEGY_MINISAT_H__
#define __VARIABLE_SELECTION_STRATEGY_MINISAT_H__

#include "SolverListener.hpp"
#include "VariableSelectionStrategy.hpp"
#include "Activities.hpp"
#include "Heap.hpp"

namespace ArgoSat {

  class MinisatVariableSelectionStrategy :
    public VariableSelectionStrategy, public SolverListener {
  public:
    MinisatVariableSelectionStrategy(const Solver& solver, 
				     double bumpAmount = 1.0, double decayFactor = 1.0/0.95,
				     bool useLiteralCountingInitialization = false);

    ~MinisatVariableSelectionStrategy();

    virtual void init();

    virtual void onConflict(const Clause* clause) {
      _activities.decay();
      bumpClauseVariablesActivity(clause);
    }

    virtual void onExplain(Literal /*literal*/, const Clause* clause) {
      bumpClauseVariablesActivity(clause);
    }

    virtual void onBacktrack(Literal literal) {
      Variable variable = Literals::variable(literal);
      if (!_activityHeap.contains(variable)) {
	_activityHeap.push_heap(variable);
      }
    }

    virtual Variable getVariable();

    static VariableSelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

  private:
    void bumpClauseVariablesActivity(const Clause* clause) {
      Clause::const_iterator it,
	beg = clause->begin(), en = clause->end();
      for (it = beg; it != en; it++)
	bumpVariableActivity(Literals::variable(*it));
    }

    void bumpVariableActivity(Variable variable) {
      _activities.bump(variable);
      // update the position in the heap
      if (_activityHeap.contains(variable))
	_activityHeap.increase(variable);
    }

    const Solver& _solver;
    Activities _activities;
    Heap<Variable, Activities::Comparator> _activityHeap;
  };
}//namespace ArgoSat
#endif
