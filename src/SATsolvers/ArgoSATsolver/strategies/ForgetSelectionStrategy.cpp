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

#include "ForgetSelectionStrategy.hpp"
#include "ForgetSelectionStrategyMinisat.hpp"
#include "ForgetSelectionStrategyClauseLength.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {
ForgetSelectionStrategy* ForgetSelectionStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int& curr) {
    if (curr >= argc)
	throw std::string("ForgetSelectionStrategy specification expected");

      if (strcmp(argv[curr], "minisat") == 0)
	return MinisatForgetSelectionStrategy::createFromCmdLine(solver, argc, argv, ++curr);

      if (strcmp(argv[curr], "clause_length") == 0)
	return ForgetSelectionStrategyClauseLength::createFromCmdLine(solver, argc, argv, ++curr);

      throw std::string("Unknown ForgetSelectionStrategy: ") + argv[curr];
}



}//namespace ArgoSat
