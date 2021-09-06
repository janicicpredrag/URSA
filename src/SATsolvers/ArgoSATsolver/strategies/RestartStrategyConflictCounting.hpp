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
#ifndef __RESTART_STRATEGY_CONFLICT_COUNTING_H__
#define __RESTART_STRATEGY_CONFLICT_COUNTING_H__


#include "RestartStrategy.hpp"
#include "SolverListener.hpp"

namespace ArgoSat {
  class Solver;

  class RestartStrategyConflictCounting : public RestartStrategy,
					  public SolverListener {
  public:
    RestartStrategyConflictCounting(Solver& solver);
    virtual ~RestartStrategyConflictCounting();

    virtual void init() {
      _numRestarts = 0;
      _numConflicts = 0;
      calculateConfilctsForFirstRestart();
    }

    virtual void onConflict(const Clause* /*conflictClause*/) {
      _numConflicts++;
    }

    virtual void onRestart() {
      _numRestarts++;
      _numConflicts = 0;
      calculateConfilctsForNextRestart();
    }

    virtual bool shouldRestart() {
      return _numConflicts >= _numConflictsForNextRestart;
    }

  protected:
    virtual void calculateConfilctsForFirstRestart() = 0;
    virtual void calculateConfilctsForNextRestart() = 0;

    Solver& _solver;
    size_t _numRestarts;
    size_t _numConflicts;
    size_t _numConflictsForNextRestart;
  };
}//namespace ArgoSat
#endif
