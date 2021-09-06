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
#ifndef __LITERAL_OCCURENCE_COUNTER_H__
#define __LITERAL_OCCURENCE_COUNTER_H__

#include "Clause.hpp"

namespace ArgoSat {
class LiteralOccurenceCounter {
public:
  LiteralOccurenceCounter(size_t numVars) {
      _numVars = numVars;
  }

  void count(const std::vector<const Clause*>& clauses) {
    _counts.resize(2*_numVars, 0);
    std::vector<const Clause*>::const_iterator i,
      b = clauses.begin(), e = clauses.end();
    for (i = b; i != e; i++) {
      Clause::const_iterator j, cb = (*i)->begin(), ce = (*i)->end();
      for (j = cb; j != ce; j++)
	_counts[*j]++;
    }
  }

  void printCounts() {
    for (Variable v = 0; v < _numVars; v++) {
      Literal pos = Literals::literal(v, true);
      Literal neg = Literals::literal(v, false);
      cout << Literals::toString(pos) 
	   << " : " << _counts[pos] << "    "
	   << Literals::toString(neg) 
	   << " : " << _counts[neg] << endl;
    }
  }

  unsigned getCount(Literal l) {
    return _counts[l];
  }

private:
  size_t _numVars;
  std::vector<unsigned> _counts;
};

}//namespace ArgoSat

#endif
