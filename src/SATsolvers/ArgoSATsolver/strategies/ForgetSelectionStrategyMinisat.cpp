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
#include "ForgetSelectionStrategyMinisat.hpp"
#include "Solver.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {

  MinisatForgetSelectionStrategy::MinisatForgetSelectionStrategy
  (Solver& solver, double percentToForget, 
   double clauseActivityBumpAmount, double clauseActivityDecayFactor) 
    : _solver(solver), 
      _clauseActivity(clauseActivityBumpAmount, clauseActivityDecayFactor),
      _percentToForget(percentToForget) {
    _solver.addListener(this);
  }

  MinisatForgetSelectionStrategy::~MinisatForgetSelectionStrategy() {
    _solver.removeListener(this);
  }

  void MinisatForgetSelectionStrategy::bumpClauseActivity(const Clause* clause) {
    if (!_solver.isLearntClause(clause))
      return;

    _clauseActivity.bump(clause->getActivity());
    if (clause->getActivity() > MAX_ACTIVITY) {
      rescaleClauseActivities();
    }
  }

  void MinisatForgetSelectionStrategy::rescaleClauseActivities() {
    std::vector<const Clause*>::const_iterator it, 
      beg = _solver.getLearntClauses().begin(), 
      en = _solver.getLearntClauses().end();

    for (it = beg; it != en; it++) {
      _clauseActivity.rescale((*it)->getActivity());
    }
    _clauseActivity.rescaleBumpAmount();
  }

  ForgetSelectionStrategy* MinisatForgetSelectionStrategy::createFromCmdLine
  (Solver& solver, int argc, char* argv[], int& curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("MinisatForgetSelectionStrategy::Percent clauses for foget expected");
    double p = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      return new MinisatForgetSelectionStrategy(solver, p);
  
    double i = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("MinisatForgetSelectionStrategy::Clause activity decay expected");
    double d = atof(argv[curr++]);  
  
    return new MinisatForgetSelectionStrategy(solver, p, i, d);
  }

}//namespace ArgoSat
