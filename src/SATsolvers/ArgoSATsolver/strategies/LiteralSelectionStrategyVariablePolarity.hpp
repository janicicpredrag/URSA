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
#ifndef __LITERAL_SELECTION_STRATEGY_VARIABLE_POLARITY_H__
#define __LITERAL_SELECTION_STRATEGY_VARIABLE_POLARITY_H__

#include "LiteralSelectionStrategy.hpp"
#include "VariableSelectionStrategy.hpp"
#include "LiteralPolaritySelectionStrategy.hpp"

namespace ArgoSat {
  class LiteralSelectionStrategyVariablePolarity : public LiteralSelectionStrategy {
  public:
    LiteralSelectionStrategyVariablePolarity(VariableSelectionStrategy* v, LiteralPolaritySelectionStrategy* lp)
      : _variableSelectionStrategy(v), _literalPolaritySelectionStrategy(lp) {
    }
  
    ~LiteralSelectionStrategyVariablePolarity() {
      delete _variableSelectionStrategy;
      delete _literalPolaritySelectionStrategy;
    }

    void setVariableSelectionStrategy(VariableSelectionStrategy* strategy) {
      if (_variableSelectionStrategy)
	delete _variableSelectionStrategy;
      _variableSelectionStrategy = strategy;
    }

    void setLiteralPolaritySelectionStrategy(LiteralPolaritySelectionStrategy* strategy) {
      if (_literalPolaritySelectionStrategy)
	delete _literalPolaritySelectionStrategy;
      _literalPolaritySelectionStrategy = strategy;
    }

    virtual Literal getLiteral() {
      return _literalPolaritySelectionStrategy->getLiteral(_variableSelectionStrategy->getVariable());
    }

    static LiteralSelectionStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr);

  private:
    VariableSelectionStrategy* _variableSelectionStrategy;
    LiteralPolaritySelectionStrategy* _literalPolaritySelectionStrategy;
  };
}//namespace ArgoSat
#endif

