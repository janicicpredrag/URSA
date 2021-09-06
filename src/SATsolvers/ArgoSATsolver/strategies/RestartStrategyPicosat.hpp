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
#ifndef __RESTART_STRATEGY_PICOSAT_H_
#define __RESTART_STRATEGY_PICOSAT_H_

#include "RestartStrategyConflictCounting.hpp"

namespace ArgoSat {

  class PicosatRestartStrategy : public RestartStrategyConflictCounting {
  public:
    PicosatRestartStrategy(Solver& solver, 
			   size_t numConflictsForFirstRestart = 100, 
			   double restartInc = 1.5) 
      : RestartStrategyConflictCounting(solver),
	_numConflictsForFirstRestart(numConflictsForFirstRestart),
	_restartInc(restartInc) {
    
    }

    void init() {
      _inner = _numConflictsForFirstRestart;
      _outer = _numConflictsForFirstRestart;
      RestartStrategyConflictCounting::init();
    }

    static RestartStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);

  protected:
    void calculateConfilctsForFirstRestart() {
      _numConflictsForNextRestart = _numConflictsForFirstRestart;
    }

    void calculateConfilctsForNextRestart() {
      if (_inner >= _outer) {
	_outer = (size_t)(_outer * _restartInc);
	_inner = _numConflictsForFirstRestart;
      } else {
	_inner = (size_t)(_inner * _restartInc);
      }
      _numConflictsForNextRestart = _inner;
    }

    size_t  _inner, _outer;
    const size_t _numConflictsForFirstRestart;
    const float _restartInc;
  };

}//namespace ArgoSat
#endif
