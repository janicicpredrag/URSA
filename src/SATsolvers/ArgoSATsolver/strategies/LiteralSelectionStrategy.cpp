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
#include <string.h>
#include "Solver.hpp"
#include "LiteralSelectionStrategy.hpp"
#include "LiteralSelectionStrategyVariablePolarity.hpp"

namespace ArgoSat {

  LiteralSelectionStrategy* LiteralSelectionStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    if (curr >= argc)
      throw std::string("LiteralSelectionStrategy specification expected");
    if (strcmp(argv[curr], "variablepolarity") == 0) {
      return LiteralSelectionStrategyVariablePolarity::createFromCmdLine(solver, argc, argv, ++curr);
    } else
      throw std::string("Unknown LiteralSelectionStrategy: ") + argv[curr];
  }

  LiteralSelectionStrategy* LiteralSelectionStrategyVariablePolarity::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    VariableSelectionStrategy* v = VariableSelectionStrategy::createFromCmdLine(solver, argc, argv, curr);
    LiteralPolaritySelectionStrategy* lp = LiteralPolaritySelectionStrategy::createFromCmdLine(solver, argc, argv, curr);
    return new LiteralSelectionStrategyVariablePolarity(v, lp);
  }

}//namespace ArgoSat
