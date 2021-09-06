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
#ifndef __LITERAL_POLARITY_SELECTION_STRATEGY__
#define __LITERAL_POLARITY_SELECTION_STRATEGY__

#include "Literal.hpp"
#include "Auxiliary.hpp"
#include "LiteralOccurenceCounter.hpp"

#include "SolverListener.hpp"
#include "Solver.hpp"

namespace ArgoSat {
///////////////////////////////////////////////////////////////////
class LiteralPolaritySelectionStrategy {
public:
  virtual ~LiteralPolaritySelectionStrategy() {}
  virtual Literal getLiteral(Variable variable) = 0;

  static LiteralPolaritySelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

protected:
  Literal makeLiteral(Variable variable, bool positive) {
	return Literals::literal(variable, positive);
  }
};

///////////////////////////////////////////////////////////////////
class LiteralPolaritySelectionStrategyAlwaysTrue : public LiteralPolaritySelectionStrategy {
public:
  virtual Literal getLiteral(Variable variable) {
	return makeLiteral(variable, true);
  }

  static LiteralPolaritySelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);
};

///////////////////////////////////////////////////////////////////
class LiteralPolaritySelectionStrategyAlwaysFalse : public LiteralPolaritySelectionStrategy {
public:
  virtual Literal getLiteral(Variable variable) {
	return makeLiteral(variable, false);
  }

  static LiteralPolaritySelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);
};

///////////////////////////////////////////////////////////////////
class LiteralPolaritySelectionStrategyRandom : public LiteralPolaritySelectionStrategy {
public:
  LiteralPolaritySelectionStrategyRandom(float percentPositive) {
	assert(0 <= percentPositive & percentPositive <= 1);
	_percentPositive = percentPositive;
  }

  virtual Literal getLiteral(Variable variable) {
	return makeLiteral(variable, randFloat() <= _percentPositive);
  }
  
  static LiteralPolaritySelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

private:
  float _percentPositive;
};

class LiteralPolaritySelectionStrategyPolaritySaving : public LiteralPolaritySelectionStrategy, public SolverListener {
 public:
  LiteralPolaritySelectionStrategyPolaritySaving(Solver& solver, 
						 bool useLiteralCountingInitialization = true, 
						 std::string fileNameInitialization = "") 
    : _solver(solver), 
      _useLiteralCountingInitialization(useLiteralCountingInitialization),
      _useFileInitialization(fileNameInitialization),
      _inited (false) {
    _solver.addListener(this);
  }

  ~LiteralPolaritySelectionStrategyPolaritySaving() {
    _solver.removeListener(this);
  }

  virtual void init();

  virtual void onAssert(Literal literal) {
    if (!_inited)
      return;

    _savedPolarity[Literals::variable(literal)] = Literals::isPositive(literal);
  }

  virtual Literal getLiteral(Variable variable) {
    return makeLiteral(variable, _savedPolarity[variable]);
  }

  static LiteralPolaritySelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);
 private:
  Solver& _solver;
  bool _useLiteralCountingInitialization;
  std::string _useFileInitialization;
  bool _inited;

  std::vector<bool> _savedPolarity;
};

}//namespace ArgoSat
#endif
