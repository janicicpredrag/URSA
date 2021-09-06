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
#ifndef __FORGET_SELECTION_STRATEGY_CLAUSE_LENGTH_H__
#define __FORGET_SELECTION_STRATEGY_CLAUSE_LENGTH_H__

#include <cassert>
#include "ForgetSelectionStrategy.hpp"

namespace ArgoSat {

  // Clause length based Forget clause selection
  class ForgetSelectionStrategyClauseLength : public ForgetSelectionStrategy {
  public:
    ForgetSelectionStrategyClauseLength(double percentToForget = 0.5) 
      : _percentToForget(percentToForget) {
      assert(0 < percentToForget && percentToForget <= 1);
    }

    virtual void select(std::vector<const Clause*>::iterator b_in,
			std::vector<const Clause*>::iterator e_in,
			std::vector<const Clause*>::iterator& b_out,
			std::vector<const Clause*>::iterator& e_out);

    static ForgetSelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);
  private:
    struct ClauseLengthComparator {
      bool operator() (const Clause* c1, const Clause* c2) const;
    };
  
    double _percentToForget;
  };

}//namespace ArgoSat
#endif
