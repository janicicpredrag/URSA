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
#include "ForgetStrategyFunction.hpp"
#include "Solver.hpp"
#include "Auxiliary.hpp"

namespace ArgoSat {
  ForgetStrategyFunction::ForgetStrategyFunction(Solver& solver) 
    : _solver(solver) {
    _solver.addListener(this);
  }

  ForgetStrategyFunction::~ForgetStrategyFunction() {
    _solver.removeListener(this);
  }

  bool ForgetStrategyFunction::shouldForget() {
    return _solver.getNumberOfLearntClauses() >= _numClausesForNextForget;
  }

  void ForgetStrategyFunction::calculateClausesForNextForget() {
    _numClausesForNextForget = (unsigned) function(_solver.getNumberOfInitialClauses(), _forgetCounter - 1);
  }

  ForgetStrategy* ForgetStrategyLinearFunction::createFromCmdLine(Solver& solver, int argc, char* argv[], int curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("ForgetStrategyLinearFunction :: a + b*n :: a expected");
    double a = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("ForgetStrategyLinearFunction :: a + b*n :: b expected");
    double b = atof(argv[curr++]);

    return new ForgetStrategyLinearFunction(solver, a, b);
  
  }

  ForgetStrategy* ForgetStrategyExponentialFunction::createFromCmdLine(Solver& solver, int argc, char* argv[], int curr) {
    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("ForgetStrategyExponentialFunction :: a * b^n :: a expected");
    double a = atof(argv[curr++]);

    if (curr >= argc || !isFloat(argv[curr]))
      throw std::string("ForgetStrategyLinearFunction :: a * b^n :: b expected");
    double b = atof(argv[curr++]);

    return new ForgetStrategyExponentialFunction(solver, a, b);
  }

}//namespace ArgoSat
