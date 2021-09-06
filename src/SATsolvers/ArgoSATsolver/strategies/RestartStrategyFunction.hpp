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
#ifndef __RESTART_STRATEGY_FUNCTION_H__
#define __RESTART_STRATEGY_FUNCTION_H__

#include <cmath>
#include "RestartStrategyConflictCounting.hpp"

namespace ArgoSat {
  class RestartStrategyFunction : public RestartStrategyConflictCounting {
  public:
    RestartStrategyFunction(Solver& solver)
      : RestartStrategyConflictCounting(solver) {
    }

  protected:
    void calculateConfilctsForFirstRestart() {
      _numConflictsForNextRestart = (unsigned) function(_numRestarts);
    }

    void calculateConfilctsForNextRestart() {
      _numConflictsForNextRestart = (unsigned) function(_numRestarts);
    }
  
    virtual double function(unsigned n) = 0;
  };

  class RestartStrategyExponentialFunction : public RestartStrategyFunction {
  public:
    RestartStrategyExponentialFunction(Solver& solver, 
				       double a = 100.0, double b = 1.5) 
      : RestartStrategyFunction(solver), _a(a), _b(b) {
    }

    static RestartStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);
  protected:
    double function(unsigned n) {
      return _a*pow(_b, (int)n);
    }

    double _a;
    double _b;
  };

  class RestartStrategyLinearFunction : public RestartStrategyFunction {
  public:
    RestartStrategyLinearFunction(Solver& solver,
				  double a = 100.0, double f = 100.0)
      : RestartStrategyFunction(solver), _f(f), _a(a) {
    }

    static RestartStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);

  protected:
    double _f;
    double _a;

    double function(unsigned n) {
      return _a + _f * n;
    }  
  };

}//namespace ArgoSat
#endif
