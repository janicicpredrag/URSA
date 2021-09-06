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
#ifndef __VALUATION_H__
#define __VALUATION_H__

#include "Variable.hpp"
#include "Literal.hpp"

#include <cassert>
#include <vector>
#include <iostream>
#include <algorithm>
using std::cout;
using std::endl;

namespace ArgoSat {

  class Valuation {
  public:
    virtual ~Valuation() {}

    void setNumberOfVariables(unsigned n) {
      assert(empty());
      for (Variable i = 0; i < n; i++)
	addVariable(i);
    }

    virtual void addVariable(Variable var) {
      assert(empty());
      while(_variableValues.size() <= var)
	_variableValues.push_back(UNDEF);
    }

    unsigned size() const {
      return _literals.size();
    }

    bool empty() const {
      return size() == 0;
    }

    void clear() {
      _literals.clear();
      _variableValues.clear();
    }

    void push(Literal l) {
      assert(!containsLiteral(l));
      assert(!containsLiteral(Literals::opposite(l)));

      _literals.push_back(l);
      _variableValues[Literals::variable(l)] = Literals::isPositive(l) ? TRUE : FALSE; 
    }

    Literal pop() {
      assert(size() > 0);
      Literal l = _literals.back();
      Variable v = Literals::variable(l);
      _variableValues[v] = UNDEF;
      _literals.pop_back();
      return l;
    }

    Literal top() const {
      assert(!empty());
      return _literals.back();
    }

    void printValues(std::ostream& ostr) const {
      for (size_t i = 0; i < _variableValues.size(); i++)
	ostr << toString(_variableValues[i]) << " ";
      ostr << endl;
    }

    void print(std::ostream& ostr) const {
      for (size_t i = 0; i < _literals.size(); i++)
	ostr << Literals::toString(_literals[i]) << " ";
      ostr << endl;
    }

    bool isTrue(Literal l) const {
      return Literals::isPositive(l) ? 
	isTrueVariable(Literals::variable(l)) : 
	isFalseVariable(Literals::variable(l));
    }
  
    bool isFalse(Literal l) const {
      return Literals::isPositive(l) ? 
	isFalseVariable(Literals::variable(l)) : 
	isTrueVariable(Literals::variable(l));
    }

    bool isUndef(Literal l) const {
      //   return !isTrue(l) && !isFalse(l);
      return isUndefVariable(Literals::variable(l));
    }

    ExtendedBoolean variableValue(Variable v) const {
      return _variableValues[v];
    }

    ExtendedBoolean literalValue(Literal l) const {
      return Literals::isPositive(l) ?
	_variableValues[Literals::variable(l)] :
	opposite(_variableValues[Literals::variable(l)]);
    }

    bool isTrueVariable(Variable v) const {
      return _variableValues[v] == TRUE;
    }

    bool isFalseVariable(Variable v) const {
      return _variableValues[v] == FALSE;
    }

    bool isUndefVariable(Variable v) const {
      return _variableValues[v] == UNDEF;
    }

    const std::vector<Literal>& getLiterals() {
      return _literals;
    }

  protected:
    bool containsLiteral(Literal l) const {
      return std::find(_literals.begin(), _literals.end(), l) != _literals.end();
    }

    std::vector<Literal> _literals;

    // Cached truth value of each variable
    std::vector<ExtendedBoolean> _variableValues;
  };

}//namespace ArgoSat
#endif
