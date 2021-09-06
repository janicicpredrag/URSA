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
#ifndef __LOG_SOLVER_LISTENER_H__
#define __LOG_SOLVER_LISTENER_H__

#include "SolverListener.hpp"

namespace ArgoSat {
  class LoggingSolverListener : public SolverListener {
  public:

    LoggingSolverListener(const Solver& solver, bool propagate = false) 
      : _solver(solver), _propagate (propagate) {
    }

    void init() {
      cout << "Init" << endl;
    }
  
    void onAssert(Literal l) {
      cout << "Assert: " << Literals::toString(l) << endl;
    }

    void onDecide(Literal l) {
      cout << "Decide: " << Literals::toString(l) << endl;
    }

    void onBacktrack(Literal l) {
      cout << "Backtrack: " << Literals::toString(l) << endl;
    }

    void onConflict(const Clause* conflictClause) {
      //    cout << "Trail: " << _solver.getTrail().toString() << endl;
      cout << "Conflict: " << conflictClause->toString() << endl;
    }

    void onExplain(Literal l, const Clause* clause) {
      cout << "Explain: " << Literals::toString(l) << " : " 
	   << clause->toString() << endl;
    }

    void onPropagate(Literal l, const Clause* clause) {
      if (_propagate) 
	cout << "Propagate: " << Literals::toString(l) << " <- " << clause->toString() << endl;
    }

    void onLearn(const Clause* clause) {
      cout << "Learn: " << clause->toString() << endl;
    }

    void onForget() {
      cout << "Forget" << endl;
    }

    void onRestart() {
      cout << "Restart" << endl;
    }
  private:
    const Solver& _solver;
    bool _propagate;
  };

  class DotLoggingSolverListener : public SolverListener {
  public:
    DotLoggingSolverListener(const Solver& solver, unsigned row_length = 80) 
      : _decisions(0), _solver(solver), 
	_row_length(row_length), _pos_in_row(0) {
    }

    void onAssert(Literal l) {
      if (_solver.getTrail().currentLevel() == 0) {
	break_row_if_neccessary();
	cout << "z";
	cout.flush();

      }
    }

    void onDecide(Literal l) {
      if (_decisions++ == 1000) {
	break_row_if_neccessary();
	cout << ".";
	cout.flush();
	_decisions = 0;
      }
    }

    void onRestart() {
      cout << "r" << endl;
      _pos_in_row = 0;
    }

    void onForget() {
      break_row_if_neccessary();
      cout << "f";
      cout.flush();
    }

  private:
    void break_row_if_neccessary() {
      if (++_pos_in_row == _row_length) {
	_pos_in_row = 1;
	cout << endl;
      }
    }

    unsigned _decisions;
    const Solver& _solver;
    unsigned _row_length;
    unsigned _pos_in_row;
  };

}//namespace ArgoSat
#endif
