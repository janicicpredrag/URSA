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
#include <cstring>

#include "VariableSelectionStrategy.hpp"
#include "VariableSelectionStrategyFirstUndefined.hpp"
#include "VariableSelectionStrategyMinisat.hpp"
#include "VariableSelectionStrategyRandomDecorator.hpp"

namespace ArgoSat {

  VariableSelectionStrategy* VariableSelectionStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    if (curr >= argc)
      throw std::string("VariableSelectionStrategy specification expected");
  
    if (strcmp(argv[curr], "random") == 0)
      return VariableSelectionStrategyRandomDecorator::createFromCmdLine(solver, argc, argv, ++curr);
    if (strcmp(argv[curr], "first_undefined") == 0)
      return VariableSelectionStrategyFirstUndefined::createFromCmdLine(solver, argc, argv, ++curr);
    if (strcmp(argv[curr], "minisat") == 0)
      return MinisatVariableSelectionStrategy::createFromCmdLine(solver, argc, argv, ++curr);

    throw std::string("Unknown VariableSelectionStrategy: ") + argv[curr];
  }

}//namespace ArgoSat
