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
#ifndef __FORGET_STRATEGY_MINISAT_H__
#define __FORGET_STRATEGY_MINISAT_H__

#include "ForgetStrategy.hpp"
#include "SolverListener.hpp"

namespace ArgoSat {

  class Solver;

  class MinisatForgetStrategy : public ForgetStrategy, public SolverListener {
  public:
    MinisatForgetStrategy(Solver& solver,
			  double percentClausesForFirstForget = (1.0/3.0), 
			  double forgetInc = 1.1);

    ~MinisatForgetStrategy();

    virtual void init();

    virtual bool shouldForget();

    virtual void onRestart() {
      _numClausesForNextForget = (size_t)(_numClausesForNextForget * _forgetInc);
    }

    virtual void onForget() {
    }

    static ForgetStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);
  private:
    Solver& _solver;
    size_t _numClausesForNextForget;
    const float _percentClausesForFirstForget;
    const float _forgetInc;
  };

}//namespace ArgoSat

#endif
