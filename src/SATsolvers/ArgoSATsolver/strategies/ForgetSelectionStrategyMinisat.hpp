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
#ifndef __FORGET_SELECTION_STRATEGY_MINISAT_H__
#define __FORGET_SELECTION_STRATEGY_MINISAT_H__

#include "ForgetSelectionStrategy.hpp"
#include "SolverListener.hpp"
#include "ActivityMeasure.hpp"

namespace ArgoSat {

  // Activity based Forget clause selection
  class MinisatForgetSelectionStrategy : public ForgetSelectionStrategy, public SolverListener {
  public:
    MinisatForgetSelectionStrategy(Solver& solver, 
				   double percentToForget = 0.5,
				   double clauseActivityBumpAmount = 1.0,
				   double clauseActivityDecayFactor = 1.0/0.999
				   );

    ~MinisatForgetSelectionStrategy();

    virtual void onConflict(const Clause* conflictClause) {
      bumpClauseActivity(conflictClause);
      _clauseActivity.decay();
    }

    virtual void select(std::vector<const Clause*>::iterator b_in,
			std::vector<const Clause*>::iterator e_in,
			std::vector<const Clause*>::iterator& b_out,
			std::vector<const Clause*>::iterator& e_out) {
      std::sort(b_in, e_in, ClauseActivityComparator());
      b_out = b_in + (int)((e_in - b_in) * (1 - _percentToForget));
      e_out = e_in;
    }

    static ForgetSelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

  private:
    Solver& _solver;
    ActivityMeasure _clauseActivity;

    struct ClauseActivityComparator {
      bool operator() (const Clause* c1, const Clause* c2) {
	return c1->getActivity() > c2->getActivity();
      }
    };
     
    void bumpClauseActivity(const Clause* clause);

    void rescaleClauseActivities();

    double _percentToForget;
  };

}//namespace ArgoSat
#endif
