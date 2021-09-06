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
#ifndef __TIMEOUT_SOLVER_LISTENER_H__
#define __TIMEOUT_SOLVER_LISTENER_H__

#include <cstdlib>
#include "SolverListener.hpp"
#include "Clock.hpp"

namespace ArgoSat {
class TimeoutSolverListener : public SolverListener {
public:
  TimeoutSolverListener(size_t time) : 
    _time(time) {
  }
	
  virtual void onRestart() {
    if (_clock.elapsed() > _time) {
      cerr << "Timeout after: " << _clock.elapsed() << "s" << endl;
      exit(EXIT_FAILURE);
    } else {
      cerr << "Elapsed: " << _clock.elapsed() << "s" << endl;
    }
  }

	
private:
  size_t _time;
  Clock _clock;
};

}//namespace ArgoSat
#endif
