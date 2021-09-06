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
#include "Clause.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {
bool Clause::containsTrueLiteral(const std::vector<Literal>& literals, 
				 const Valuation& valuation) {
  const_iterator i, b = literals.begin(), e = literals.end();
  for (i = b; i != e; i++)
    if (valuation.isTrue(*i))
      return true;
  return false;
}

bool Clause::isTrue(const Valuation& valuation) const {
  return Clause::containsTrueLiteral(_literals, valuation);
}

bool Clause::isFalse(const Valuation& valuation) const {
  const_iterator i, b = begin(), e = end();
  for (i = b; i != e; i++)
    if (!valuation.isFalse(*i))
      return false;
  return true;
}

bool Clause::isUnit(const Valuation& valuation) const {
  return getUnitLiteral(valuation) != 0;
}

const Literal* Clause::getUnitLiteral(const Valuation& valuation) const {
  const Literal* unassignedLiteral = 0;
  const_iterator i, b = begin(), e = end();
  for (i = b; i != e; i++) {
    if (valuation.isUndef(*i)) {
      if (unassignedLiteral == 0) {
	unassignedLiteral = &(*i);	
      } else {
	return 0;
      }
    }
  }

  return unassignedLiteral;
}

extern std::ostream& logger;

std::string Clause::toString() const {
  std::string result = "";
  const_iterator i, b = begin(), e = end();
  for (i = b; i != e; i++) {
    result += Literals::toString(*i) + " ";
  }
  result += "[" + ::toString(_activity) + "]";
  result += _initial ? "[Initial]" : "[Learnt]";
  return result;
}


std::string Clause::valuationString(const Valuation& valuation) const {
  std::string result;
  const_iterator i, b = begin(), e = end();
  for (i = b; i != e; i++) {
    if (valuation.isTrue(*i))
      result += "T";
    else if (valuation.isFalse(*i))
      result += "F";
    else 
      result += "?";
  }
  return result;
}

Clause* Clause::opposite() const {
  std::vector<Literal> literals;
  const_iterator i, b = begin(), e = end();
  for (i = b; i != e; i++)
    literals.push_back(Literals::opposite(*i));
  return new Clause(literals);
}

void Clause::canonicalForm() {
  std::sort(_literals.begin(), _literals.end());
}

bool Clause::subsumes(Clause* subclause) const {
  const_iterator it, beg = subclause->begin(), en = subclause->end();
  for (it = beg; it != en; it++)
    if (!containsLiteral(*it)) {
      return false;
    }
  return true;
}

bool Clause::removeLiteral(Literal literal) {
  iterator it = findLiteral(literal);
  if (it == end())
    return false;
  _literals.erase(it);
  return true;
}

void Clause::removeFalsifiedLiterals(std::vector<Literal>& _literals,
				     const Valuation& v) {
  std::vector<Literal>::iterator i, new_i, 
    b = _literals.begin(), e=_literals.end();
  for (i = b, new_i = b; i != e; i++) {
    if (!v.isFalse(*i)) {
      *new_i = *i;
      new_i++;
    }
  }
  _literals.resize(new_i - b);
}

void Clause::removeDuplicateLiterals(std::vector<Literal>& _literals) {
  if (!containsDuplicateLiterals(_literals))
    return;
  std::sort(_literals.begin(), _literals.end());
  std::vector<Literal> _new_literals;
  std::vector<Literal>::const_iterator i, 
    b = _literals.begin(), e = _literals.end();
  Literal p = *b;
  _new_literals.push_back(p);
  for (i = b + 1; i != e; i++) {
    if (*i != p) {
      p = *i;
      _new_literals.push_back(p);
    }
  }
  _literals = _new_literals;
  assert(!containsDuplicateLiterals(_literals));
}

bool Clause::containsDuplicateLiterals() {
  return Clause::containsDuplicateLiterals(_literals);
}

bool Clause::containsDuplicateLiterals(const std::vector<Literal>& _literals) {
  std::vector<Literal>::const_iterator i, j, 
    b = _literals.begin(), e = _literals.end();
  for (i = b; i != e; i++) {
    for (j = i+1; j != e; j++) {
      if (*i == *j)
	return true;
    }
  }
  return false;
}

bool Clause::isTautology(const std::vector<Literal>& _literals) {
  std::vector<Literal>::const_iterator i, j, 
    b = _literals.begin(), e = _literals.end();
  for (i = b; i != e; i++) {
    for (j = i+1; j != e; j++) {
      if (*i == Literals::opposite(*j))
	return true;
    }
  }
  return false;
}

}//namespace ArgoSat
