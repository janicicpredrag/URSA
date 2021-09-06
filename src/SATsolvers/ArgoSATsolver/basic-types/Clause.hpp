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
#ifndef __CLAUSE_H__
#define __CLAUSE_H__

#include "Literal.hpp"
#include "Valuation.hpp"

namespace ArgoSat {
class Clause {
public:
  typedef std::vector<Literal>::const_iterator const_iterator;
  typedef std::vector<Literal>::iterator iterator;

  Clause(const std::vector<Literal>& literals, bool initial=false) 
    : _literals(literals),
      _initial(initial),
      _activity(0.0) {
    assert(!containsDuplicateLiterals(literals));
    assert(literals.size() > 1);
  }

  const_iterator begin() const {
    return _literals.begin();
  }

  size_t size() const {
    return _literals.size();
  }

  const_iterator end() const {
    return _literals.end();
  }

  iterator begin() {
    return _literals.begin();
  }

  Literal operator[] (size_t i) const {
    return _literals[i];
  }

  Literal& operator[] (size_t i) {
    return _literals[i];
  }

  bool containsLiteral(Literal literal) const {
    return findLiteral(literal) != end();
  }

  const_iterator findLiteral(Literal literal) const {
    return std::find(_literals.begin(), _literals.end(), literal);
  }
  iterator findLiteral(Literal literal) {
    return std::find(_literals.begin(), _literals.end(), literal);
  }

  bool removeLiteral(Literal literal);
  static void removeDuplicateLiterals(std::vector<Literal>& _literals);
  static void removeFalsifiedLiterals(std::vector<Literal>& _literals, 
				      const Valuation& v);
  static bool containsTrueLiteral(const std::vector<Literal>& _literals,
				  const Valuation& v);
  static bool isTautology(const std::vector<Literal>& _literals);

  bool containsDuplicateLiterals();

  bool isTrue(const Valuation& v) const;
  bool isFalse(const Valuation& v) const;
  bool isUnit(const Valuation& v) const;
  const Literal* getUnitLiteral(const Valuation& v) const;
  bool subsumes(Clause* subclause) const;
    
  Clause* opposite() const;
  void canonicalForm();

  std::string toString() const;
  std::string valuationString(const Valuation& valuation) const;

    
private:
  static bool containsDuplicateLiterals(const std::vector<Literal>& literals);

  void swapLiterals(size_t i, size_t j) {
    Literal tmp = _literals[i];
    _literals[i] = _literals[j];
    _literals[j] = tmp;
  }

  std::vector<Literal> _literals;


  /////////////////////////////////////
  // Additional data that is cached
  ////////////////////////////////////
public:
  void setWatchLiteral1(Literal literal) {
    const_iterator i = findLiteral(literal);
    assert(i != end());
    swapLiterals(0, i - begin());
  }

  void setWatchLiteral2(Literal literal) {
    const_iterator i = findLiteral(literal);
    assert(i != end());
    swapLiterals(1, i - begin());
  }

  void setWatch1(size_t position) {
    swapLiterals(0, position);
  }

  void setWatch2(size_t position) {
    swapLiterals(1, position);
  }

  Literal watch1() const {
    assert(size() > 0);
    return _literals[0];
  }

  Literal watch2() const {
    assert(size() > 1);
    return _literals[1];
  }

  void swapWatches() {
    swapLiterals(0, 1);
  }

  static const unsigned NOT_FOUND = (unsigned)(-1);
  size_t unfalsifiedNonWatchedLiteralPosition(const Valuation& valuation) {
    const_iterator i,
      b = begin(), e = end();
    size_t pos = NOT_FOUND;

    for (i = b + 2; i != e; i++) {
      // if (!valuation.isFalse(*i))
      //    return i - b;

      // Try to find a true literal and put a watch on it
      // this seems to speed up things a little bit
      if (valuation.isTrue(*i))
	return i - b;

      if (valuation.isUndef(*i))
	pos = i - b;
    }
    return pos;
  }

  double& getActivity() const {
    return _activity;
  }

  bool isInitial() const {
    return _initial;
  }

private:
  bool _initial;
  mutable double _activity;
};

}//namespace ArgoSat
#endif
