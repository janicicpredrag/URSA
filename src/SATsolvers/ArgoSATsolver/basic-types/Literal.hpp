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
#ifndef __LITERAL_H__
#define __LITERAL_H__

#include <cstdlib>
#include "Variable.hpp"

namespace ArgoSat {

  typedef UNSIGNED Literal;

  class Clause;

  class Literals {
  public:
    static Literal opposite(Literal literal) {
      if (isPositive(literal))
	return literal + 1;
      else
	return literal - 1;
    }

    static bool isPositive(Literal literal) {
      return !(literal & 1);
    }

    static bool isNegative(Literal literal) {
      return literal & 1;
    }

    static Literal variable(Literal literal) {
      return literal >> 1;
    }

    static Literal literal(Variable variable, bool positive) {
      return positive ? 
	(variable << 1) : 
	((variable << 1) + 1);
    }

    static Literal fromInt(int num) {
      return num < 0 ? 
	Literals::literal(-num-1, false) : 
	Literals::literal(num-1, true);
    }

    static std::string toString(Literal literal) {
      std::string result = isPositive(literal) ? "" : "-";
      result += Variables::toString(variable(literal));
      return result;
    }

    static std::string literalVectorToString(const std::vector<Literal>& vct, 
					     std::string separator = ", ") {
      std::string result = "";
      size_t n = vct.size();
      for (size_t i = 0; i < n; i++) {
	result += Literals::toString(vct[i]);
	if (i != n - 1)
	  result += separator;
      }
      return result;
    }

    static void shuffleVector(std::vector<Literal>& literals) {
      for (size_t i = 0; i < literals.size(); i++) {
	size_t j = i + (rand() % (literals.size() - i));
	Literal tmp = literals[i];
	literals[i] = literals[j];
	literals[j] = tmp;
      }
    }

    static const unsigned UNDEFINED = (unsigned)(-1);
  };

}//namespace ArgoSat
#endif
