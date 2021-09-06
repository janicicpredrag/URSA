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
#ifndef __FORGET_STRATEGY_FUNCTION_H__
#define __FORGET_STRATEGY_FUNCTION_H__

#include "ForgetStrategy.hpp"
#include "SolverListener.hpp"

namespace ArgoSat {

  class Solver;

  class ForgetStrategyFunction : public ForgetStrategy, public SolverListener {
  public:

    ForgetStrategyFunction(Solver& solver);

    ~ForgetStrategyFunction();

    virtual void init() {
      _forgetCounter = 1;
      calculateClausesForNextForget();
    }
		
    virtual bool shouldForget();


    virtual void onRestart() {
    }

    virtual void onForget() {
      _forgetCounter++;
      calculateClausesForNextForget();
    }

  protected:
    Solver& _solver;

    virtual void calculateClausesForNextForget();

    virtual double function(unsigned a0, unsigned n) = 0;

    unsigned _forgetCounter;
    unsigned _numClausesForNextForget;
  };

  class ForgetStrategyLinearFunction : public ForgetStrategyFunction {
  public:
    ForgetStrategyLinearFunction(Solver& solver, double a = (1.0/2.0), double b = (1.0/20.0))
      : ForgetStrategyFunction(solver), _a(a), _b(b) {
    }

    virtual double function (unsigned a0, unsigned n) {
      return a0 * (_a + _b * n);
    }

    static ForgetStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);
  protected:
    double _a;
    double _b;
  };


  class ForgetStrategyExponentialFunction : public ForgetStrategyFunction {
  public:
    ForgetStrategyExponentialFunction(Solver& solver, double a = (1.0/2.0), double b = 1.001)
      : ForgetStrategyFunction(solver), _a(a), _b(b) {
    }

    virtual double function (unsigned a0, unsigned n) {
      return a0 * _a * pow(_b, n);
    }

    static ForgetStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);
  protected:
    double _a;
    double _b;
  };

}//namespace ArgoSat

#endif
