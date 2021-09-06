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
#include "ForgetStrategyMinisat.hpp"
#include "Solver.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {

  MinisatForgetStrategy::MinisatForgetStrategy(Solver& solver,
					       double percentClausesForFirstForget, 
					       double forgetInc) 
    : _solver(solver),
      _percentClausesForFirstForget(percentClausesForFirstForget),
      _forgetInc(forgetInc) {
    _solver.addListener(this);
  }

  MinisatForgetStrategy::~MinisatForgetStrategy() {
    _solver.removeListener(this);
  }

  void MinisatForgetStrategy::init() {
    _numClausesForNextForget = (size_t)(_percentClausesForFirstForget * _solver.getNumberOfInitialClauses()); 
  }

  bool MinisatForgetStrategy::shouldForget() {
    return _solver.getNumberOfLearntClauses() >= _numClausesForNextForget;
  }

  ForgetStrategy* MinisatForgetStrategy::createFromCmdLine(Solver& solver, int argc, char* argv[], int curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      return new MinisatForgetStrategy(solver);
    double p = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("MinisatForgetStrategy::Percent increment expected");
    double i = atof(argv[curr++]);

    return new MinisatForgetStrategy(solver, p, i);
  }

}//namespace ArgoSat
