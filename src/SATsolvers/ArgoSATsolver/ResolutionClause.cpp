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
#include "ResolutionClause.hpp"
#include "Solver.hpp"
#include "Auxiliary.hpp"
#include <algorithm>

// inline void
// fill(bool* __first, bool* __last, const bool& __c) {
//    bool __tmp = __c;
//    memset(__first, static_cast<unsigned char>(__tmp), __last - __first);
//}

namespace ArgoSat {

  size_t ResolutionClause::level(Literal literal) const {
    return _solver.getTrail().level(literal);
  }

  size_t ResolutionClause::levelVariable(Variable variable) const {
    return _solver.getTrail().levelVariable(variable);
  }

  size_t ResolutionClause::currentLevel() const {
    return _solver.getTrail().currentLevel();
  }

  void ResolutionClause::clear() {
    assert(_containsVariable.size() == _solver.getNumberOfVariables());
    _containsVariable.assign(_containsVariable.size(), false);
    _lowerLevelLiterals.clear();
    _numCurrentLevelLiterals = 0;
    _lastLiteral = Literals::UNDEFINED;
    _clause = 0;
  }

  std::string ResolutionClause::toString() const {
    return lowerLevelLiteralsToString() + " | " + 
      ::toString(getNumberOfCurrentLevelLiterals());
  }

}// namespace ArgoSat
