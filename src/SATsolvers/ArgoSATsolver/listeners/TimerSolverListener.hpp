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
#ifndef __TIMER_SOLVER_LISTENER_H__
#define __TIMER_SOLVER_LISTENER_H__

#include "SolverListener.hpp"
#include "Clock.hpp"

namespace ArgoSat {
class TimerSolverListener : public SolverListener {
public:
  TimerSolverListener() {
    reset();
  }
	
  virtual void onConflict(const Clause* clause) {
    _conflicts++;
    logSpeed();
  }
	
  virtual void onAssert(Literal l) {
    _asserts++;
    logSpeed();
  }
	
private:
  void logSpeed() {
    double t = _clock.elapsed();
    if (t > 10.0) {
      reset();
    }
  }
	
  void reset() {
    _asserts = 0;
    _conflicts = 0;
    _clock.reset();
  }

  Clock _clock;
  unsigned _asserts;
  unsigned _conflicts;
};

}//namespace ArgoSat

#endif
