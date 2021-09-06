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
#include "ForgetSelectionStrategyClauseLength.hpp"

#include <algorithm>

#include <Clause.hpp>
#include "Auxiliary.hpp"

namespace ArgoSat {
  
  ForgetSelectionStrategy* ForgetSelectionStrategyClauseLength::createFromCmdLine(Solver& /*solver*/, int argc, char* argv[], int& curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("ClauseLengthForgetSelectionStrategy::Percent clauses for foget expected");
    double p = atof(argv[curr++]);

    return new ForgetSelectionStrategyClauseLength(p);
  }

  void ForgetSelectionStrategyClauseLength::select
  (std::vector<const Clause*>::iterator b_in,
		      std::vector<const Clause*>::iterator e_in,
		      std::vector<const Clause*>::iterator& b_out,
		      std::vector<const Clause*>::iterator& e_out) {
    std::sort(b_in, e_in, ClauseLengthComparator());
    b_out = b_in + (int)((e_in - b_in) * (1 - _percentToForget));
    e_out = e_in;
  }


  bool ForgetSelectionStrategyClauseLength::ClauseLengthComparator::operator() 
    (const Clause* c1, const Clause* c2) const {
    return c1->size() < c2->size();
  }

}//namespace ArgoSat
