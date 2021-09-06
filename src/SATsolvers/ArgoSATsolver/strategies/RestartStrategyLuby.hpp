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
#ifndef __RESTART_STRATEGY_LUBY_H__
#define __RESTART_STRATEGY_LUBY_H__

#include "RestartStrategyConflictCounting.hpp"

namespace ArgoSat {
  class RestartStrategyLuby: public RestartStrategyConflictCounting {
  public:
    RestartStrategyLuby(Solver& solver, size_t factor = 32) 
      : RestartStrategyConflictCounting(solver), 
	_factor(factor) {
    }

    static RestartStrategy* createFromCmdLine(Solver& solver, int argc, char* argv[], int curr);

  protected: 
    void calculateConfilctsForFirstRestart() {
      _numConflictsForNextRestart = luby_super(_numRestarts + 1)*_factor;
    }
 
    void calculateConfilctsForNextRestart() {
      _numConflictsForNextRestart = luby_super(_numRestarts + 1)*_factor;    
    }

    static RestartStrategy* createFromCmdLine(int argc, char* argv[], int curr);

    // taken from SATZ_rand source code
    size_t luby_super(size_t i) {
      size_t power;
      size_t k;
 
      // let 2^k be the least power of 2 >= (i+1)
      k = 1;
      power = 2;
      while (power < (i + 1)) {
	k += 1;
	power *= 2;
      }
      if (power == (i + 1))
	return (power / 2);
      return (luby_super(i - (power / 2) + 1));
    }
 
    size_t _factor;
  };

}//namespace ArgoSat

#endif
