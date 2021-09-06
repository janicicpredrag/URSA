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
#ifndef __TRAIL_H__
#define __TRAIL_H__

#include "Valuation.hpp"

/* 
   _trail: 1 2 | 3 -4 5 | 6 -7
   _levels: 2 5

   _trail: | -1 2 | 3 -4 5
   _levels: 0 2 
*/

namespace ArgoSat {
class Trail : public Valuation {
public:
  Trail() 
    : _firstUnitLiteral(0) {
  }

  unsigned currentLevel() const {
    return _levels.size();
  }

  unsigned level(Literal l) const { 
    assert(containsLiteral(l));
    return levelVariable(Literals::variable(l));
  }

  void push(Literal l, bool decision) {
    assert(!hasUnitLiteral());
    assert(!containsLiteral(l));
    assert(!containsLiteral(Literals::opposite(l)));
    
    if (decision)
      _levels.push_back(size());
    _variableLevels[Literals::variable(l)] = currentLevel();
    _firstUnitLiteral++;

    Valuation::push(l);
  }

  Literal pop() {
    if (_firstUnitLiteral >= size())
      _firstUnitLiteral--;

    Literal l = Valuation::pop();

    bool decision = (size() == _levels.back());
    if (decision)
      _levels.pop_back();

    _variableLevels[Literals::variable(l)] = UNKNOWN;
    return l;
  }

  void clear() {
    Valuation::clear();
    _levels.clear();
    _variableLevels.clear();
  }

  std::string toString() const;

  void print() const {
    cout << toString() << endl;
  }

  virtual void addVariable(Variable var) {
    Valuation::addVariable(var);
    while (_variableLevels.size() <= var)
      _variableLevels.push_back(UNKNOWN);
  }

  unsigned levelVariable(Variable v) const {
    assert(_variableLevels[v] != UNKNOWN);
    return _variableLevels[v];
  }

  void enqueueUnitLiteral(Literal l) {
    assert(!containsLiteral(l));
    assert(!containsLiteral(Literals::opposite(l)));
    _variableLevels[Literals::variable(l)] = currentLevel();
    Valuation::push(l);
  }

  bool hasUnitLiteral() {
    return _firstUnitLiteral < size();
  }

  Literal assertNextUnitLiteral() {
    assert(hasUnitLiteral());
    Literal l = _literals[_firstUnitLiteral++];

    _variableLevels[Literals::variable(l)] = currentLevel();
    return l;
  }

  unsigned sizeUntilLevel(unsigned level) const {
	assert(level <= currentLevel());
	if (level == currentLevel())
	  return size();

	return _levels[level];
  }

private:
  static const unsigned UNKNOWN;

  size_t _firstUnitLiteral;
  std::vector<unsigned> _levels;

  // Cached decision level of each variable
  std::vector<unsigned> _variableLevels;
};
}//namespace ArgoSat

#endif
